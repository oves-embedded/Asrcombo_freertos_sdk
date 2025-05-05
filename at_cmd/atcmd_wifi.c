/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef AT_USER_DEBUG

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "lega_at_api.h"
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#else
#include "lega_common.h"
#endif
#ifdef CFG_PLF_DUET
#include "duet_uart.h"
#include "duet_cm4.h"
#include "duet_flash.h"
#include "duet_flash_kv.h"
#define lega_flash_kv_set duet_flash_kv_set
#define lega_flash_kv_get duet_flash_kv_get
#define lega_flash_kv_del duet_flash_kv_del
#else
#include "lega_uart.h"
#include "lega_cm4.h"
#include "lega_flash.h"
#include "lega_flash_kv.h"
#endif
#include "FreeRTOS.h"
#include "lega_wlan_api.h"
#include "lwip/ip_addr.h"
#ifdef LWIP_APP_PING
#include "lwip_app_ping.h"
#endif
#ifdef LWIP_APP_IPERF
#include "lwip_app_iperf.h"
#endif
#include "lwip/netdb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "soc_init.h"
#include "core_cm4.h"

#ifdef CFG_INDIGO_ADAPTER
#include "indigo_adapter.h"
#include "eloop.h"
#include "uwifi_ops_adapter.h"
#include "wpas_psk.h"
#include "uwifi_interface.h"
#endif
enum open_mode_
{
    STA_MODE_=0x01,
    SAP_MODE_,
    SNIFFER_MODE_,
    NON_MODE_=0xFF,
};

char cur_mode[5][8] = {"null","station","softap","sniffer","non"};

extern uint32_t current_iftype;
extern _at_user_info at_user_info;
extern int lega_wlan_quick_connect();
static void lega_mac_addr_string_to_array(char *input_str, uint8_t *mac_addr);


/*
 ************************************************************
 *       at cmd of wifi open callback func start
 *
 ************************************************************
 */
void lega_at_wlan_err_stat_handler(lega_wlan_err_status_e err_info)
{
    at_rspinfor("wlan err sta:%d",err_info);
    switch(err_info)
    {
        case WLAN_STA_MODE_PASSWORD_ERR:
            lega_at_response(CONN_EAPOL_FAIL);
            break;
        case WLAN_STA_MODE_DHCP_FAIL:
            lega_at_response(CONN_DHCP_FAIL);
            break;
        case WLAN_STA_MODE_CONN_RETRY_MAX:
            lega_at_response(CONN_TIMEOUT);
            break;
        default:
            break;
    }
}
#ifdef CFG_INDIGO_ADAPTER
#ifdef WFA_CERTIFICATE
int8_t mfp_cap = -1;
#endif

int8_t g_sae_only = 0;

void lega_at_wifi_open_cb(lega_at_rsp_status_t status)
{
     switch (status)
    {
        case CONN_TIMEOUT:
        case CONN_EAPOL_FAIL:
        case CONN_DHCP_FAIL:
            lega_at_response(status);
            break;
        default :
            dbg(D_CRT, D_UWIFI_CTRL, "\r\n unsupport at rsp status \r\n");
            break;
    }
}
#endif

/*
 ************************************************************
 *      at cmd of wifi open callback func end
 *
 ************************************************************
 */

/*
 ************************************************************
 *              wifi event callback func start
 *
 ************************************************************
 */
#ifdef CFG_INDIGO_ADAPTER
struct wifi_scan_t *g_st_wifi_scan;
extern uint8_t is_scan_adv;
int g_at_wifi_scan_times = 0;
#define G_AT_WIFI_MAX_SCAN_TIMES 10
void lega_at_wifi_scan_ind(lega_wlan_scan_result_t *result)
{
    struct wifi_scan_t *temp1;
    uint8_t*temp2;

#ifndef CFG_DATA_ELEM
    dbg(D_CRT, D_UWIFI_CTRL, "wifi_scan_completed ap:%d", result->ap_num);
#endif
    if(at_user_info.at_scan == 1)
    {
        if (result->ap_num) {
            lega_at_response(CONFIG_OK);
            g_at_wifi_scan_times = 0;
            at_user_info.at_scan = 0;
        } else {
            if (g_st_wifi_scan && g_at_wifi_scan_times <= G_AT_WIFI_MAX_SCAN_TIMES) {
                g_at_wifi_scan_times++;

                dbg(D_CRT, D_UWIFI_CTRL, "retry scan one more 0x%x by ssid way", g_st_wifi_scan);
                is_scan_adv = 0;
                temp1= lega_rtos_malloc(sizeof(struct wifi_scan_t));
                temp2 = temp1->ssid = lega_rtos_malloc(g_st_wifi_scan->ssid_len);

                uwifi_msg_at2u_scan_req(g_st_wifi_scan);
                memcpy(temp1, g_st_wifi_scan, sizeof(*g_st_wifi_scan));
                temp1->ssid = temp2;
                memcpy(temp1->ssid, g_st_wifi_scan->ssid, g_st_wifi_scan->ssid_len);

                g_st_wifi_scan = temp1;
                at_user_info.at_scan = 1;
            } else {
                lega_at_response(CONFIG_FAIL);
                at_user_info.at_scan = 0;
            }
        }
    }
}
#else
void lega_at_wifi_scan_ind(lega_wlan_scan_result_t *result)
{
    if(at_user_info.at_scan == 1)
    {
        int i;
        for(i=0; i<result->ap_num; i++)
        {
            at_rspdata("scan ap=%2d | rssi=%3d | ch=%2d | secu=%d | bssid=%02x:%02x:%02x:%02x:%02x:%02x | ssid=%s\r\n",
                i,
                result->ap_list[i].ap_power - 256,
                result->ap_list[i].channel,
                result->ap_list[i].security,
                result->ap_list[i].bssid[0],result->ap_list[i].bssid[1],result->ap_list[i].bssid[2],
                result->ap_list[i].bssid[3],result->ap_list[i].bssid[4],result->ap_list[i].bssid[5],
                result->ap_list[i].ssid);
        }

        at_user_info.at_scan = 0;

        lega_at_response(CONFIG_OK);
    }
}
#endif

#ifdef CFG_INDIGO_ADAPTER
uint8_t g_station_up = 0;
extern uint8_t wpa3_transition_mode_switch;
#endif
void lega_at_wifi_connect_state(lega_wifi_event_e stat)
{
     switch (stat)
    {
        case EVENT_STATION_UP:
#ifdef CFG_INDIGO_ADAPTER
            wpa3_transition_mode_switch = 0;
            lega_at_response(CONFIG_OK);
            g_station_up = 1; 
#endif

#ifndef QUICK_CONNECT
            at_rspinfor("wifi_connect_done ");
#endif
            break;
        case EVENT_STATION_DOWN:
#ifdef CFG_INDIGO_ADAPTER
            wpa3_transition_mode_switch = 0;
            lega_at_response(CONFIG_OK);
            g_station_up = 0;
#endif
            at_rspinfor("wifi_disconnect_done");
            at_user_info.sta_connected = 0;
            break;
        case EVENT_AP_UP:
            at_rspinfor("wifi_softap_open_done");
            at_user_info.sap_opend = 1;
            lega_at_response(CONFIG_OK);
            break;
        case EVENT_AP_DOWN:
            at_rspinfor("wifi_softap_close_done");
            lega_at_response(CONFIG_OK);
            break;
        case EVENT_STA_CLOSE:
            at_rspinfor("wifi_sta_close_done");
            lega_at_response(CONFIG_OK);
            break;
        default :
            at_rspinfor("unsupport wifi_event_e");
            break;
    }
}

#ifdef LWIP_DUALSTACK
void lega_at_wifi_get_ip6_ind(lega_wlan_ip_stat_t *pnet)
{
    int i = 0;
    int preferred_ip6 = 0;
    for(i=0;i<3;i++)
    {
        if(pnet->ip6[i].state == 0x30) /* IP6_ADDR_PREFERRED */
        {
           at_rspinfor("Got ip v6 #%d : %s\n",i,pnet->ip6[i].addr);
            preferred_ip6 ++;
        }
    }
}
#endif

void lega_at_wifi_get_ip_ind(lega_wlan_ip_stat_t *pnet)
{
    at_rspinfor("Got ip : %s, gw : %s, mask : %s, mac : %s\r\n",
        pnet->ip, pnet->gate, pnet->mask, pnet->macaddr);

    lega_at_response(CONFIG_OK);

    at_user_info.sta_connected = 1;
}

void lega_at_wifi_ap_peer_change(lega_wlan_client_addr_info_t *peer_info, uint8_t connect)
{
    uint8_t *ip_addr = (uint8_t*)(&(peer_info->sta_ip_addr));

    at_rspdata("softap peer status=%d ip=%d.%d.%d.%d, mac=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        connect,
        ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3],
        peer_info->sta_mac_addr[0],peer_info->sta_mac_addr[1],peer_info->sta_mac_addr[2],peer_info->sta_mac_addr[3],
        peer_info->sta_mac_addr[4],peer_info->sta_mac_addr[5]);
}

void lega_at_wifi_event_cb_register(void)
{
    lega_wlan_scan_compeleted_cb_register(lega_at_wifi_scan_ind);
    lega_wlan_stat_chg_cb_register(lega_at_wifi_connect_state);
    lega_wlan_ip_got_cb_register(lega_at_wifi_get_ip_ind);
#ifdef LWIP_DUALSTACK
    lega_wlan_ip6_got_cb_register(lega_at_wifi_get_ip6_ind);
#endif
    lega_wlan_ap_peer_change_cb_register(lega_at_wifi_ap_peer_change);
}

/*
 ************************************************************
 *              Wifi Event Callback Func end
 *
 ************************************************************
 */

#define UWIFI_AUTOCONNECT_MAX_TRIES   0xFF
/*
 ************************************************************
 *                    WIFI AT CMD START
 *
 ************************************************************
 */
extern void lega_wlan_clear_pmk(void);
static int lega_at_wifi_open(int argc, char **argv)
{
    int32_t ret;
    uint32_t start_ip = 0;
    lega_wlan_init_type_t conf;
    memset(&conf,0,sizeof(lega_wlan_init_type_t));
    conf.wifi_mode = STA;

    at_rspinfor("doing...");
    switch(argc)
    {
        case 1:
            break;
        case 2:
            if (strcmp(argv[1],"sta") == 0)
            {
                if((current_iftype != NON_MODE_) && (current_iftype != STA_MODE_))
                {
                    dbg_at("cur mode is %s, close it first!",cur_mode[current_iftype]);
                    return CONFIG_FAIL;
                }
                else
                {
                    lega_wlan_open(&conf);
                    return CONFIG_OK;
                }
                break;
            }
            else
            {
                return PARAM_RANGE;
            }
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if ((strcmp(argv[1],"sta") == 0))
            {
                #ifndef CFG_STA_AP_COEX
                if((current_iftype != NON_MODE_) && (current_iftype != STA_MODE_))
                {
                    dbg_at("cur mode is %s, close it first!",cur_mode[current_iftype]);
                    return CONFIG_FAIL;
                }
                #endif
                conf.dhcp_mode = WLAN_DHCP_CLIENT;
                if(strlen((char *)argv[2]) > 32)
                {
                    return PARAM_RANGE;
                }
                if((strcmp(argv[0],"wifi_open_bssid") == 0))
                    lega_mac_addr_string_to_array(argv[2], (uint8_t*)conf.mac_addr);
                else
                    strcpy((char*)conf.wifi_ssid, (char*)argv[2]);

                if(argc >= 4)
                {
                    if((strlen(argv[3]) > 4)&&(strlen(argv[3]) < 65))
                        strcpy((char*)conf.wifi_key, (char*)argv[3]);
                    else if((strlen(argv[3]) == 1) && (convert_str_to_int(argv[3]) == 0))
                        memset(conf.wifi_key, 0, 64);
                    else
                    {
                        dbg_at("param4 error, input 0 or pwd(length >= 5)\n");
                        return PARAM_RANGE;
                    }
                }

                if(argc >= 5)
                {
                    conf.channel = convert_str_to_int(argv[4]);
                    if((conf.channel > 13) || (conf.channel < 1))
                    {
                        dbg_at("param5 error,channel range:1~13");
                        return PARAM_RANGE;
                    }
                }

                if(argc >= 6)
                {
                    conf.wifi_retry_times = convert_str_to_int(argv[5]);
                    if (conf.wifi_retry_times == LEGA_STR_TO_INT_ERR)
                    {
                        dbg_at("retry times error, should less than 255");
                        return PARAM_RANGE;
                    }
                    if (conf.wifi_retry_times > UWIFI_AUTOCONNECT_MAX_TRIES)
                    {
                        dbg_at("wifi retry time greater than 255, set it to be 255\n");
                        conf.wifi_retry_times = UWIFI_AUTOCONNECT_MAX_TRIES;
                    }
                }

                if((strcmp(argv[0],"wifi_open_def") == 0))
                    lega_flash_kv_set("sta_info", &conf, sizeof(conf), 1);

                if((at_user_info.flag_sta == 1) && (at_user_info.dis_dhcp == 1))
                {
                    memcpy(conf.local_ip_addr, at_user_info.staip, 16);
                    memcpy(conf.gateway_ip_addr, at_user_info.stagw, 16);
                    memcpy(conf.net_mask, at_user_info.stamask, 16);
                    if(strlen((const char*)conf.local_ip_addr))
                        conf.dhcp_mode = WLAN_DHCP_DISABLE;
                }
                else if((at_user_info.flag_sta == 0) && (at_user_info.dis_dhcp == 1))
                {
                    lega_wlan_ip_stat_t staip_info;
                    int32_t staip_len;
                    staip_len = sizeof(staip_info);
                    ret = lega_flash_kv_get("staip_info", &staip_info, &staip_len);
                    if(!ret)
                    {
                        memcpy(conf.local_ip_addr, staip_info.ip, 16);
                        memcpy(conf.gateway_ip_addr, staip_info.gate, 16);
                        memcpy(conf.net_mask, staip_info.mask, 16);
                        if(strlen((const char*)conf.local_ip_addr))
                            conf.dhcp_mode = WLAN_DHCP_DISABLE;
                    }
                }
            }
            else if((strcmp(argv[1],"ap") == 0))
            {
                #ifndef CFG_STA_AP_COEX
                #ifdef CFG_DUAL_AP
                if((current_iftype != NON_MODE_) && (current_iftype != SAP_MODE_))
                #else
                if(current_iftype != NON_MODE_)
                #endif
                {
                    dbg_at("cur mode is %s, close it first!",cur_mode[current_iftype]);
                    return CONFIG_FAIL;
                }
                #endif
                conf.wifi_mode = SOFTAP;
                if(strlen((char *)argv[2]) > 32)
                {
                    return PARAM_RANGE;
                }
                strcpy((char*)conf.wifi_ssid, (char*)argv[2]);

                if(argc >= 4)
                {
                    if((strlen(argv[3]) > 7)&&(strlen(argv[3]) < 65))
                        strcpy((char*)conf.wifi_key, (char*)argv[3]);
                    else if((strlen(argv[3]) == 1) && (convert_str_to_int(argv[3]) == 0))
                        memset(conf.wifi_key, 0, 64);
                    else
                    {
                        dbg_at("param4 error, input 0 or pwd(length >= 8)\n");
                        return PARAM_RANGE;
                    }
                }

                if(argc >= 5)
                {
                    conf.channel = convert_str_to_int(argv[4]);
                    if((conf.channel > 13) || (conf.channel < 1))
                    {
                        dbg_at("param5 error,channel range:1~13");
                        return PARAM_RANGE;
                    }
                }
                if(argc >= 6)
                {
                    conf.hide = convert_str_to_int(argv[5]);
                    if(conf.hide == LEGA_STR_TO_INT_ERR)
                    {
                        dbg_at("param6 error, hide err");
                        return PARAM_RANGE;
                    }
                }
                if (argc >= 7)
                {
                    conf.interval = convert_str_to_int(argv[6]);
                    if(conf.interval == LEGA_STR_TO_INT_ERR)
                    {
                        dbg_at("param7 error, interval err");
                        return PARAM_RANGE;
                    }
                }

                if((strcmp(argv[0],"wifi_open_def") == 0))
                    lega_flash_kv_set("sap_info", &conf, sizeof(conf), 1);

                if(at_user_info.flag_sap == 1)
                {
                    memcpy(conf.local_ip_addr, at_user_info.sapip, 16);
                    memcpy(conf.gateway_ip_addr, at_user_info.sapgw, 16);
                    memcpy(conf.net_mask, at_user_info.sapmask, 16);
                    ipaddr_aton(at_user_info.start_ip, (ip_addr_t*)(&start_ip));
                    if(start_ip)
                    {
                        memcpy(conf.start_ip, at_user_info.start_ip, 16);
                        memcpy(conf.end_ip, at_user_info.end_ip, 16);
                    }
                }
                else if(at_user_info.flag_sap == 0)
                {
                    lega_wlan_ip_stat_t sapip_info;
                    int32_t sapip_len;
                    sapip_len = sizeof(sapip_info);
                    ret = lega_flash_kv_get("sapip_info", &sapip_info, &sapip_len);
                    if(!ret)
                    {
                        memcpy(conf.local_ip_addr, sapip_info.ip, 16);
                        memcpy(conf.gateway_ip_addr, sapip_info.gate, 16);
                        memcpy(conf.net_mask, sapip_info.mask, 16);
                        if(sapip_info.start_ip[0] != 0xFF)
                        {
                            memcpy(conf.start_ip, sapip_info.start_ip, 16);
                            memcpy(conf.end_ip, sapip_info.end_ip, 16);
                        }
                    }
                }
            }
            else
            {
                dbg_at("param2 error,not support\r\n");
                return PARAM_RANGE;
            }
            break;
        default:
            dbg_at("param num error,1~7\r\n");
            return PARAM_RANGE;
    }
    lega_wlan_err_stat_cb_register(lega_at_wlan_err_stat_handler);

    lega_wlan_clear_pmk();
    lega_wlan_open(&conf);

    return RSP_NULL;
}

int32_t lega_at_wifi_setup(void)
{
    lega_wlan_init_type_t conf = {0};
    lega_wlan_ip_stat_t staip_info;
    lega_wlan_ip_stat_t sapip_info;
    int32_t staip_len;
    int32_t sapip_len;
    uint8_t sta_auto_enable;
    uint8_t sap_auto_enable;
    int32_t temp_len;
    int32_t ret = -1;

#ifdef QUICK_CONNECT
    ret = lega_wlan_quick_connect();
    if(ret == 0)
    {
        return ret;
    }
#endif

    temp_len = sizeof(sta_auto_enable);
    lega_flash_kv_get("sta_auto_enable", &sta_auto_enable, &temp_len);
    temp_len = sizeof(sap_auto_enable);
    lega_flash_kv_get("sap_auto_enable", &sap_auto_enable, &temp_len);

    temp_len = sizeof(conf);
    if(!sta_auto_enable)
    {
        ret = lega_flash_kv_get("sta_info", &conf, &temp_len);
        if(ret)
        {
            dbg_at("sta_info is NULL in kv. return\n");
            return ret;
        }
        conf.wifi_mode = STA;

        staip_len = sizeof(staip_info);
        ret = lega_flash_kv_get("staip_info", &staip_info, &staip_len);
        if(!ret)
        {
            memcpy(conf.local_ip_addr, staip_info.ip, 16);
            memcpy(conf.gateway_ip_addr, staip_info.gate, 16);
            memcpy(conf.net_mask, staip_info.mask, 16);
            if(strlen((const char*)conf.local_ip_addr))
                conf.dhcp_mode = WLAN_DHCP_DISABLE;
        }
        lega_wlan_open(&conf);
        ret = 0;
    }
    else if(!sap_auto_enable)
    {
        ret = lega_flash_kv_get("sap_info", &conf, &temp_len);
        if(ret)
        {
            dbg_at("sap_info is NULL in kv. return\n");
            return ret;
        }
        conf.wifi_mode = SOFTAP;

        sapip_len = sizeof(sapip_info);
        ret = lega_flash_kv_get("sapip_info", &sapip_info, &sapip_len);
        if(!ret)
        {
            memcpy(conf.local_ip_addr, sapip_info.ip, 16);
            memcpy(conf.gateway_ip_addr, sapip_info.gate, 16);
            memcpy(conf.net_mask, sapip_info.mask, 16);
            if(sapip_info.start_ip[0] != 0xFF)
            {
                memcpy(conf.start_ip, sapip_info.start_ip, 16);
                memcpy(conf.end_ip, sapip_info.end_ip, 16);
            }
        }

        lega_wlan_open(&conf);
        ret = 0;
    }
    return ret;
}

static int lega_at_wifi_close(int argc, char **argv)
{
    uint8_t echo_flag = 0;
    uint32_t iftype_tmp = current_iftype;
    if(iftype_tmp == NON_MODE_)
    {
        dbg_at("no wlan interface open, don't need close!");
        #ifdef CFG_INDIGO_ADAPTER
        return CONFIG_OK;
        #else
        return CONFIG_FAIL;
        #endif
    }
    else if(iftype_tmp == SNIFFER_MODE_)
    {
        dbg_at("cur mode is sniffer,use wifi_sniffer_stop close!");
        return CONFIG_FAIL;
    }
    echo_flag = at_user_info.uart_echo;
    memset(&at_user_info, 0, sizeof(at_user_info));
    at_user_info.uart_echo = echo_flag;

    lega_wlan_close();
#ifdef LWIP_APP_PING
    ping_raw_deinit();
#endif // LWIP_APP_PING

    return RSP_NULL;
}

static int lega_at_wifi_set_dhcp(int argc, char **argv)
{
    int dhcp;

    if(argc < 2)
    {
        return PARAM_MISS;
    }

    dhcp = convert_str_to_int(argv[1]);

    if(dhcp == 1)
    {
        at_user_info.dis_dhcp = 0;
        return CONFIG_OK;
    }
    else if(dhcp == 0)
    {
        at_user_info.dis_dhcp = 1;
        return CONFIG_OK;
    }
    else
    {
        return PARAM_RANGE;
    }
}
#ifdef CFG_INDIGO_ADAPTER
uint8_t g_start_dhcp_ip = 0;
#endif
static int lega_at_wifi_set_sta_ip(int argc, char **argv)
{
    int32_t ret;
    uint32_t ip = 0;
    uint32_t gw = 0;
    uint32_t mask = 0;
    lega_wlan_ip_stat_t staip_info = {0};

    if(argc < 4)
    {
        return PARAM_MISS;
    }

    if((strlen(argv[1])>16) || (strlen(argv[2])>16) || (strlen(argv[3])>16))
    {
        dbg_at("ip gate mask length error!");
        return PARAM_RANGE;
    }

    ipaddr_aton(argv[1], (ip_addr_t*)(&ip));
    ipaddr_aton(argv[2], (ip_addr_t*)(&gw));
    ipaddr_aton(argv[3], (ip_addr_t*)(&mask));

    if((gw&mask) != (ip&mask))
    {
        dbg_at("ip&mask must equal gateway&mask");
        return PARAM_RANGE;
    }

    if((strcmp(argv[0],"wifi_set_sta_ip") == 0))
    {
        memcpy(at_user_info.staip, argv[1], 16);
        memcpy(at_user_info.stagw, argv[2], 16);
        memcpy(at_user_info.stamask, argv[3], 16);
        at_user_info.flag_sta = 1;
        #ifdef CFG_INDIGO_ADAPTER
        g_start_dhcp_ip = 1;
        #endif
    }
    else if((strcmp(argv[0],"wifi_set_sta_ip_def") == 0))
    {
        memcpy(staip_info.ip, argv[1], 16);
        memcpy(staip_info.gate, argv[2], 16);
        memcpy(staip_info.mask, argv[3], 16);
        at_user_info.flag_sta = 0;
        ret = lega_flash_kv_set("staip_info",&staip_info,sizeof(staip_info), 1);
        if(ret)
        {
            dbg_at("save stainfo to kv failed! \r\n");
            return CONFIG_FAIL;
        }
    }

    return CONFIG_OK;
}

static int lega_at_wifi_set_softap_ip(int argc, char **argv)
{
    int32_t ret;
    uint32_t ip = 0;
    uint32_t gw = 0;
    uint32_t mask = 0;
    uint32_t start_ip4 = 0;
    uint32_t end_ip4 = 0;
    lega_wlan_ip_stat_t sapip_info = {0};

    if(argc < 4 || argc > 6)
    {
        return PARAM_MISS;
    }

    if((strlen(argv[1])>16) || (strlen(argv[2])>16) ||(strlen(argv[3])>16))
    {
        dbg_at("ip gate mask length error!");
        return PARAM_RANGE;
    }

    ipaddr_aton(argv[1], (ip_addr_t*)(&ip));
    ipaddr_aton(argv[2], (ip_addr_t*)(&gw));
    ipaddr_aton(argv[3], (ip_addr_t*)(&mask));

    if(ip != gw)
    {
        dbg_at("ip must equal to gateway \r\n");
        return PARAM_RANGE;
    }

    if(argc == 6)
    {
        ipaddr_aton(argv[4], (ip_addr_t*)(&start_ip4));
        ipaddr_aton(argv[5], (ip_addr_t*)(&end_ip4));

        if(((start_ip4&mask) != (ip&mask)) || ((end_ip4&mask) != (ip&mask)))
        {
            dbg_at("ip rang param error.\r\n");
            return PARAM_RANGE;
        }

        start_ip4 = (start_ip4 & 0xFF000000)>>24;
        end_ip4 = (end_ip4 & 0xFF000000)>>24;

        if((start_ip4 > 254) || (start_ip4 < 2))
        {
            dbg_at("param5 error,start_ip range 2~254.\r\n");
            return PARAM_RANGE;
        }
        if((end_ip4 > 254) || (end_ip4 < 2))
        {
            dbg_at("param6 error end_ip range:2~254.\r\n");
            return PARAM_RANGE;
        }
        if(start_ip4 > end_ip4)
        {
            dbg_at("start_ip must less than end_ip.\r\n");
            return PARAM_RANGE;
        }
        if(end_ip4 - start_ip4 < 4)
        {
            dbg_at("end_ip4 - start_ip4 < 4. softap mode support 4 stations  at least");
            return PARAM_RANGE;
        }
        if(end_ip4 - start_ip4 > 6)
        {
            dbg_at("end_ip is large.softap mode support max 4 stations one time");
            return PARAM_RANGE;
        }
    }

    if((strcmp(argv[0],"wifi_set_softap_ip") == 0))
    {
        memcpy(at_user_info.sapip, argv[1], 16);
        memcpy(at_user_info.sapgw, argv[2], 16);
        memcpy(at_user_info.sapmask, argv[3], 16);
        at_user_info.flag_sap = 1;
        if(argc == 6)
        {
            memcpy(at_user_info.start_ip, argv[4], 16);
            memcpy(at_user_info.end_ip, argv[5], 16);
        }
    }
    else if((strcmp(argv[0],"wifi_set_softap_ip_def") == 0))
    {
        memcpy(sapip_info.ip, argv[1], 16);
        memcpy(sapip_info.gate, argv[2], 16);
        memcpy(sapip_info.mask, argv[3], 16);
        at_user_info.flag_sap = 0;
        if(argc == 6)
        {
            memcpy(sapip_info.start_ip, argv[4], 16);
            memcpy(sapip_info.end_ip, argv[5], 16);
        }
        ret = lega_flash_kv_set("sapip_info",&sapip_info,sizeof(sapip_info), 1);
        if(ret)
        {
            dbg_at("save sapinfo to kv failed! \r\n");
            return CONFIG_FAIL;
        }
    }

    return CONFIG_OK;
}
static int lega_at_wifi_get_macaddr(int argc, char **argv)
{
    int ret;
    uint8_t macaddr[6];

    ret = lega_wlan_get_mac_address(macaddr);

    if(ret == 0)
    {
        at_rspdata("mac=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
        return CONFIG_OK;
    }
    else
        return CONFIG_FAIL;
}


#ifdef CFG_INDIGO_ADAPTER
#define LOOPBACK_TIMEOUT 300
int g_loop_stop_flag = 0;
int g_select_sem_flag = 0;// 0 get sem  ,1 release sem
static int lega_at_wifi_start_loopback_server(int argc, char **argv)
{
    int ret;
    lega_wlan_ip_stat_t *ip_stat;

    ip_stat = lega_wlan_get_ip_status();

    if(ip_stat == NULL)
    {
        return CONFIG_FAIL;
    }

    if (argc < 1)
        printf("Please supply enough parameters\r\n");

    g_loop_stop_flag = 1;
    while(g_select_sem_flag==1)
    {
        if(g_select_sem_flag==0)
            break;
        lega_rtos_delay_milliseconds(10);//sleep 10mm
    }

    ret = loopback_client_stop();
    g_loop_stop_flag = 0;
    if(ret==0)
        ret = loopback_client_start( argv[1], ip_stat->ip, LOOPBACK_TIMEOUT);

    if(ret == 0)
        ret = lega_indigo_init();//eloop_run() data_handle
    if(ret == 0)
    {
        //lega_at_response(CONFIG_OK);
        return CONFIG_OK;
    }
    else{
        //lega_at_response(CONFIG_FAIL);
        return CONFIG_FAIL;
    }

}
static int lega_at_wifi_stop_loopback_server(int argc, char **argv)
{

    int ret;
    g_loop_stop_flag = 1;

    while(g_select_sem_flag==1)
    {
        if(g_select_sem_flag==0)
            break;
        lega_rtos_delay_milliseconds(10);//sleep 10mm
    }


    ret = loopback_client_stop();

    if(ret == 0)
    {
        //lega_at_response(CONFIG_OK);
        return CONFIG_OK;
    }
    else{
        //lega_at_response(CONFIG_FAIL);
        return CONFIG_FAIL;
    }

}
lega_semaphore_t g_lega_at_wifi_sta_associate_sema;
lega_semaphore_t g_lega_at_wifi_sta_disconnect_sema;

extern int g_lega_at_wifi_sta_associate_ret;
static int lega_at_wifi_sta_associate(int argc, char **argv)
{
    struct wifi_conn_t con_req;
    struct rwnx_hw *rwnx_hw = uwifi_get_rwnx_hw();
    char ssid[64], pwd[64];
    // 0 -- wifi_sta_associate
    // 1 -- sta
    //lega_rtos_init_semaphore(&g_lega_at_wifi_sta_associate_sema, 0);
    strcpy((char*)ssid, (char*)argv[2]);
    con_req.ssid = ssid;
    con_req.ssid_len = strlen(argv[2]);
    if(argc >= 4)
    {
        if((strlen(argv[3]) > 7)&&(strlen(argv[3]) < 65)) {
            strcpy((char*)pwd, (char*)argv[3]);
            con_req.password = pwd;
            con_req.pwd_len = strlen(argv[3]);
        } else if((strlen(argv[3]) == 1) && (atoi(argv[3]) == 0)) {
            memset(con_req.password, 0, 64);
            con_req.pwd_len = 64;
        } else
        {
            dbg(D_CRT, D_AT, "param4 error, input 0 or pwd(length >= 8)\n");
            //lega_at_response(PARAM_RANGE);
            return PARAM_RANGE;
        }
    }

    if(argc >= 5)
    {
        con_req.channel = atoi(argv[4]);
        if((con_req.channel > 14) || (con_req.channel < 1))
        {
            dbg(D_CRT, D_AT, "param5 error,channel range:1~13");
            //lega_at_response(PARAM_RANGE);
            return PARAM_RANGE;
        }
    }



    switch (atoi(argv[5])) {
    case 0:
        con_req.encrypt = WIFI_ENCRYP_OPEN;
        break;
    case 1:
        con_req.encrypt = WIFI_ENCRYP_WEP;
        break;
    case 2:
        con_req.encrypt = WIFI_ENCRYP_WPA;
        break;
    case 3:
        con_req.encrypt = WIFI_ENCRYP_WPA2;
        break;
    case 4:
        con_req.encrypt = WIFI_ENCRYP_WPA3;
        break;
    case 5:
        con_req.encrypt = WIFI_ENCRYP_AUTO;
        break;
    }
    #ifdef WFA_CERTIFICATE
    if(argc >= 7)
    {
        mfp_cap = atoi(argv[6]);
    }
    #endif
    struct scan_result* pscanResult;
    pscanResult = lega_wlan_get_scan_ap(con_req.ssid, con_req.ssid_len, con_req.encrypt);
    printf("%s 11 pscanResult:0x%x\r\n", __func__, pscanResult);
    pscanResult = rwnx_hw->phead_scan_result;

    while(NULL != pscanResult)
    {
        //ignore it. the mfp not equal
        if(pscanResult->mfp == NL80211_MFP_NO &&  mfp_cap == NL80211_MFP_REQUIRED)
        {
            pscanResult = pscanResult->next_ptr;
            continue;
        }
        if(!memcmp(con_req.ssid, pscanResult->ssid.ssid, pscanResult->ssid.ssid_len) &&
            (pscanResult->ssid.ssid_len == con_req.ssid_len)) {

            con_req.encrypt = pscanResult->encryp_protocol;
            break;
        } else
            pscanResult = pscanResult->next_ptr;
    }
    printf("%s 22 pscanResult:0x%x\r\n", __func__, pscanResult);
    if (NULL == pscanResult) {
        con_req.encrypt = WIFI_ENCRYP_WPA2;
    }

#ifdef CFG_INDIGO_ADAPTER
    if(argc >= 8)
    {
        g_sae_only = atoi(argv[7]);
    }
    if (g_sae_only == 1)
        con_req.encrypt = WIFI_ENCRYP_WPA3;//as now the scan result set it in WIFI_ENCRYP_WPA2 when sae
    if (g_sae_only == 2 &&( con_req.encrypt == WIFI_ENCRYP_OPEN||con_req.encrypt == WIFI_ENCRYP_AUTO))//only
        con_req.encrypt = WIFI_ENCRYP_WPA2;//for WPA2_10153 case
#endif
    //lega_at_register_wifi_connect_cb(lega_at_wifi_open_cb);
    lega_wlan_err_stat_cb_register(lega_at_wlan_err_stat_handler);
    lega_wlan_clear_pmk();
    uwifi_connect_req(&con_req);
    return CONFIG_OK;
#if 0
    if (lega_rtos_get_semaphore(&g_lega_at_wifi_sta_associate_sema, 3000)){
        lega_at_response(CONN_EAPOL_FAIL);
        lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_associate_sema);
        return CONN_EAPOL_FAIL;
    } else {
        if (g_lega_at_wifi_sta_associate_ret == 0) {
            lega_at_response(CONFIG_OK);
            lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_associate_sema);
            return CONFIG_OK;
        } else {
            lega_at_response(CONN_EAPOL_FAIL);
            lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_associate_sema);
            return CONN_EAPOL_FAIL;
        }
    }
#endif
}

extern uint8_t g_start_dhcp_ip;
extern int g_lega_at_wifi_sta_disconnect_ret;
extern uint8_t g_station_up;
static int lega_at_wifi_sta_disconnect(int argc, char **argv)
{
    g_start_dhcp_ip = 0;
    //lega_rtos_init_semaphore(&g_lega_at_wifi_sta_disconnect_sema, 0);
    uwifi_disconnect();
    printf("%s up:%d\r\n", __func__, g_station_up);
    //g_station_up = 2;
    while (g_station_up != 0)
        lega_rtos_delay_milliseconds(30);
    //lega_at_response(CONFIG_OK);
    return CONFIG_OK;
    #if 0
    if (lega_rtos_get_semaphore(&g_lega_at_wifi_sta_disconnect_sema, 3000)){
        lega_at_response(CONN_EAPOL_FAIL);
        lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_disconnect_sema);
        return CONN_EAPOL_FAIL;
    } else {
        if (g_lega_at_wifi_sta_disconnect_ret == 0) {
            lega_at_response(CONFIG_OK);
            lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_disconnect_sema);
            return CONFIG_OK;
        } else {
            lega_at_response(CONN_EAPOL_FAIL);
            lega_rtos_deinit_semaphore(&g_lega_at_wifi_sta_disconnect_sema);
            return CONN_EAPOL_FAIL;
        }
    }
    #endif
}

extern struct rwnx_hw g_rwnx_hw;
uint8_t g_vif_idx;

static int lega_at_wifi_sta_dhcp(int argc, char **argv)
{
    //struct rwnx_hw *rwnx_hw = &g_rwnx_hw;
    //struct rwnx_vif *rwnx_vif = rwnx_hw->vif_table[g_vif_idx];
    enum lwip_dhcp_event evt = DHCP_CLIENT_UP;

    switch (atoi(argv[2])) {
    case 0:
        evt = DHCP_CLIENT_UP;
        break;
    case 1:
        evt = DHCP_CLIENT_DOWN;
        break;
    default:
        evt = DHCP_CLIENT_UP;
        break;
    }
    lwip_comm_dhcp_event_handler(evt);

    //uwifi_msg_rxu2u_handshake_start(WLAN_OPMODE_INFRA, rwnx_vif);
    //lega_at_response(CONFIG_OK);
    return CONFIG_OK;
}

extern uint8_t g_station_up;
static int lega_at_wifi_sta_is_connect(int argc, char **argv)
{
    if (g_station_up) {
        dbg(D_CRT, D_AT, "up=%d\r\n", g_station_up);
        //lega_at_response(CONFIG_OK);
        return CONFIG_OK;
    } else {
        dbg(D_CRT, D_AT, "up=%d", g_station_up);
        //lega_at_response(CONN_EAPOL_FAIL);
        return CONN_EAPOL_FAIL;
    }
}
#endif//CFG_INDIGO_ADAPTER

static uint8_t mac_addr_asc_to_hex(char param)
{
    uint8_t val;

    if((param >= 48) && (param <= 57))
        val = param-48;
    else if((param >= 65) && (param <= 70))
        val = param-55;
    else if((param >= 97) && (param <= 102))
        val = param-87;
    else
    {
        dbg_at("wifi_set_mac_addr:error param\r\n");
        val = 0xA;
    }
    return val;
}

static void lega_mac_addr_string_to_array(char *input_str, uint8_t *mac_addr)
{
    int i;
    for(i = 0; i < (6<<1); i++)
    {
        if(i%2 || i==1)
            mac_addr[i>>1] = mac_addr_asc_to_hex(input_str[i]) | (mac_addr[i>>1]&0xF0);
        else
            mac_addr[i>>1] = (mac_addr_asc_to_hex(input_str[i])<<4) | (mac_addr[i>>1]&0xF);
    }
}

static int lega_at_wifi_set_macaddr(int argc, char **argv)
{
    uint8_t macaddr[6];
    lega_at_rsp_status_t rsp = CONFIG_OK;
    int ret=0;

    if(argc == 2)
    {
        if(strlen(argv[1]) == (6<<1))
        {
            lega_mac_addr_string_to_array(argv[1], macaddr);
            ret = lega_wlan_set_mac_address(macaddr);

            if(!ret)
            {
                at_rspdata("mac=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);

                rsp = CONFIG_OK;
            }
            else if(ret == -2)
            {
                uint8_t exiaddr[6];
                dbg_at("mac address already exist in efuse !\r\n");

                memset(exiaddr, 0, sizeof(exiaddr));
                if(lega_wlan_get_mac_address(exiaddr) == 0)
                {
                    at_rspdata("mac efuse=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                        exiaddr[0],exiaddr[1],exiaddr[2],exiaddr[3],exiaddr[4],exiaddr[5]);
                }
                rsp = CONFIG_FAIL;
            }
            else
            {
                rsp = CONFIG_FAIL;
            }
        }
        else
        {
            dbg_at("mac addr len error\r\n");
            rsp = PARAM_RANGE;
        }
    }
    else
    {
        dbg_at("wifi_set_mac_addr:error param\r\n");
        rsp = PARAM_MISS;
    }

    return rsp;
}
static int lega_at_softap_get_sta_info(int argc, char **argv)
{
    uint8_t index = 0;
    uint8_t* sta_ip;
    uint8_t* sta_mac;
    lega_wlan_ap_client_info_t sta_addr_info;

    if(current_iftype == SAP_MODE_)
    {
        lega_get_client_ip_mac(&sta_addr_info);

        for(index = 0; index < sta_addr_info.client_num;index++)
        {
            sta_ip = (uint8_t *)&sta_addr_info.sta[index].sta_ip_addr;
            sta_mac = sta_addr_info.sta[index].sta_mac_addr;

            at_rspdata("sta%d ip=%d.%d.%d.%d, mac=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                index,
                sta_ip[0],sta_ip[1],sta_ip[2],sta_ip[3],
                sta_mac[0],sta_mac[1],sta_mac[2],sta_mac[3],sta_mac[4],sta_mac[5]);
        }

        return CONFIG_OK;
    }
    else
    {
        dbg_at("error mode, Please open softap");
        return CONFIG_FAIL;
    }
}


static int lega_at_softap_deauth_peer(int argc, char **argv)
{
    uint8_t macaddr[6];
    int ret;

    if(current_iftype == SAP_MODE_)
    {
        if(argc >= 2)
        {
            if(strlen(argv[1]) == (6<<1))
            {
                lega_mac_addr_string_to_array(argv[1], macaddr);
                ret = lega_wlan_softap_deauth_peer(macaddr);

                if(ret == 0)
                {
                    return CONFIG_OK;
                }
                else
                {
                    return CONFIG_FAIL;
                }
            }
            else
            {
                dbg_at("mac addr len error\r\n");
                return PARAM_RANGE;
            }
        }
        else
        {
            return PARAM_MISS;
        }
    }
    else
    {
        dbg_at("error mode, Please open softap");
        return CONFIG_FAIL;
    }
}

static int lega_at_get_softap_info(int argc, char **argv)
{
    lega_wlan_softap_info_t info = {0};
    int ret;

    if(current_iftype == SAP_MODE_)
    {
        ret = lega_wlan_get_softap_info(&info);
        if(ret == 0)
        {
            at_rspdata("softap ssid=%s, ch=%d, secu=%d\r\n",
                info.ssid, info.chan, info.security);
            return CONFIG_OK;
        }
        else
        {
            return CONFIG_FAIL;
        }
    }
    else
    {
        dbg_at("error mode, Please open softap");
        return CONFIG_FAIL;
    }
}

static int lega_at_get_wifi_mode(int argc, char **argv)
{
    int mode = lega_wlan_get_wifi_mode();
    at_rspdata("mode=%d\r\n", mode);

    return CONFIG_OK;
}

static int lega_at_get_ip(int argc, char **argv)
{
    lega_wlan_ip_stat_t *ip_stat;

    ip_stat = lega_wlan_get_ip_status();

    if(ip_stat == NULL)
    {
        return CONFIG_FAIL;
    }
    else
    {
        #ifdef CFG_INDIGO_ADAPTER
        at_rspdata("+ip=%s\n",ip_stat->ip);
        #else
        at_rspdata("ip=%s, gw=%s\r\n",ip_stat->ip,ip_stat->gate);
        #endif


        return CONFIG_OK;
    }
}

static int lega_at_get_sta_link_info(int argc, char **argv)
{
    lega_wlan_link_stat_t link_stat = {0};
    lega_wlan_ap_info_adv_t *ap_info;
    int ret;

    ret = lega_wlan_get_link_status(&link_stat);
    if(ret)
    {
        return CONFIG_FAIL;
    }
    else
    {
        if(link_stat.is_connected)
        {
            ap_info = lega_wlan_get_associated_apinfo();
            if(ap_info == NULL)
            {
                at_rspdata("stainfo conn=%d\r\n", link_stat.is_connected);
                return CONFIG_FAIL;
            }
            else
            {
                at_rspdata("stainfo conn=%d, ssid=%s, ch=%d, rssi=%d, secur=%d\r\n",
                    link_stat.is_connected,
                    ap_info->ssid, ap_info->channel, ap_info->rssi, ap_info->security);
                return CONFIG_OK;
            }
        }
        else
        {
            at_rspdata("stainfo conn=%d\r\n", link_stat.is_connected);
            return CONFIG_OK;
        }
    }
}

/*********************************************************************
 * @scan cmd:
 *
 *    wifi_scan            :   scan all channel (1~13)
 *
 *********************************************************************/
#ifdef CFG_INDIGO_ADAPTER
extern uint8_t is_scan_adv;
extern struct wifi_scan_t *g_st_wifi_scan;
extern int scan_done_flag;
static int lega_at_wifi_scan(int argc, char **argv)
{
    uint8_t mode = 0;
    uint8_t *temp;
    if((current_iftype == STA_MODE) || (current_iftype == SAP_MODE))
    {
        struct wifi_scan_t *st_wifi_scan = lega_rtos_malloc(sizeof(struct wifi_scan_t));
        g_st_wifi_scan = lega_rtos_malloc(sizeof(struct wifi_scan_t));
        if (st_wifi_scan == NULL || g_st_wifi_scan == NULL)
        {
            dbg(D_ERR, D_UWIFI_CTRL, "wifi_scan malloc fail!");
            return ;
        }

        st_wifi_scan->scanmode = SCAN_ALL_CHANNEL;
        st_wifi_scan->freq = 0;
        st_wifi_scan->ssid_len = 0;
        st_wifi_scan->ssid = NULL;

        if(argc > 1)
        {
            mode = atoi(argv[1]);
            if(mode == 0)
                dbg(D_INF, D_AT, "scan all channel");

            if(argc > 2)
            {
                if((mode == 1) || (mode == 2))
                {
                    st_wifi_scan->ssid_len = strlen(argv[2]);
                    st_wifi_scan->ssid = lega_rtos_malloc(strlen(argv[2]));
                    g_st_wifi_scan->ssid = lega_rtos_malloc(strlen(argv[2]));
                    temp = g_st_wifi_scan->ssid;
                    if (st_wifi_scan->ssid == NULL || g_st_wifi_scan->ssid == NULL)
                    {
                        dbg(D_ERR, D_UWIFI_CTRL, "wifi_scan malloc ssid fail!");
                        lega_rtos_free(st_wifi_scan);
                        st_wifi_scan = NULL;

                        return CONFIG_FAIL;
                    }
                    memcpy(st_wifi_scan->ssid, argv[2], strlen(argv[2]));
                    st_wifi_scan->scanmode = SCAN_SSID;
                }
            }
            if((argc > 3) && (mode == 2))
            {
                st_wifi_scan->freq = atoi(argv[3])*5 +2407;
                st_wifi_scan->scanmode = SCAN_FREQ;
            }
            if((argc > 4))
            {
                mfp_cap = atoi(argv[4]);
            }


        }

        is_scan_adv = 0;

        uwifi_msg_at2u_scan_req(st_wifi_scan);
        at_user_info.at_scan = 1;
        memcpy(g_st_wifi_scan, st_wifi_scan, sizeof(*st_wifi_scan));
        g_st_wifi_scan->ssid = temp;
        memcpy(g_st_wifi_scan->ssid, st_wifi_scan->ssid, st_wifi_scan->ssid_len);

        while(!scan_done_flag)
                lega_rtos_delay_milliseconds(30);
        scan_done_flag = 0;
    }
    else
        return CONFIG_FAIL;
}
#else
static int lega_at_wifi_scan(int argc, char **argv)
{
    char* ssid = 0;
    int channel = 0;
    char  bssid[6] = {0};
    char* invalidBssid = "000000000000";
    char *wildcard = "0";

    if((current_iftype == STA_MODE_) || (current_iftype == SAP_MODE_))
    {
        if(argc > 1) //channel
        {
            channel = convert_str_to_int(argv[1]);
            if(channel == LEGA_STR_TO_INT_ERR)
            {
                dbg_at("channel error");
                return PARAM_RANGE;
            }

            if(argc > 2) //channel bssid
            {
                if(strcmp(argv[2], invalidBssid) != 0 && strcmp(argv[2], wildcard) != 0)
                {
                    if(strlen(argv[2]) == (6<<1))
                    {
                        lega_mac_addr_string_to_array(argv[2], (uint8_t*)bssid);
                        dbg_at("scan bssid %02x:%02x:%02x:%02x:%02x:%02x",
                            bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5]);
                    }
                    else
                    {
                        dbg_at("scan bssid len error");
                    }
                }

                if(argc > 3) //channel bssid ssid
                {
                    ssid = argv[3];
                }
            }
        }

        lega_wlan_start_scan_detail(ssid, channel, bssid);
        at_user_info.at_scan = 1;
        return RSP_NULL;
    }
    else
    {
        return CONFIG_FAIL;
    }
}
#endif
extern int lega_wlan_set_txdatarate (char auto_rate, char mode, char rate_idx, char sgi);
static int lega_at_wifi_set_txdatarate (int argc, char **argv)
{
    int rate = 0;
    uint8_t short_gi = 0;

    if(strcmp(argv[1],"auto") == 0)
    {
        lega_wlan_set_txdatarate(1,0,0,0);

        return CONFIG_OK;
    }

    if(argc < 3)
    {
        return PARAM_MISS;
    }

    if(strcmp(argv[1],"b") == 0)
    {
        rate = convert_str_to_int(argv[2]);
        if(rate == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data rate err\n");
            return PARAM_RANGE;
        }

        lega_wlan_set_txdatarate(0,'b',rate,0);

        return CONFIG_OK;
    }
    else if(strcmp(argv[1],"g") == 0)
    {
        rate = convert_str_to_int(argv[2]);
        if(rate == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data rate err\n");
            return PARAM_RANGE;
        }

        lega_wlan_set_txdatarate(0,'g',rate,0);

        return CONFIG_OK;
    }
    else if(strcmp(argv[1],"n") == 0)
    {
        short_gi = 0;

        rate = convert_str_to_int(argv[2]);
        if(rate == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data rate err\n");
            return PARAM_RANGE;
        }

        if(argc >= 4)
        {
            if(strcmp(argv[3],"short") == 0)
                short_gi = 1;
            else if(strcmp(argv[3],"long") == 0)
                short_gi = 0;
            else
            {
                dbg_at("unsupport gi type\n");
                return PARAM_RANGE;
            }
        }

        lega_wlan_set_txdatarate(0,'n',rate,short_gi);

        return CONFIG_OK;
    }
    else
    {
        dbg_at("unsupport mode");
        return PARAM_RANGE;
    }
}

static int lega_at_wifi_enable_log(int argc, char **argv)
{
    lega_wlan_start_debug_mode();
    return CONFIG_OK;
}

static int lega_at_wifi_disable_log(int argc, char **argv)
{
    lega_wlan_stop_debug_mode();
    return CONFIG_OK;
}

static int lega_at_wifi_auto_enable(int argc, char **argv)
{
    uint8_t enable;
    int tempValue;
    char auto_flag[2][8] = {"enable","disable"};
    if(argc != 3)
    {
        dbg_at("wifi_auto_enable err param\r\n");
        return PARAM_MISS;
    }

    if ((strcmp(argv[1],"sta") == 0))
    {
        tempValue = convert_str_to_int(argv[2]);
        if(tempValue == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("enable param err\n");
            return PARAM_RANGE;
        }
        // 0:enable 1:disable  as flash memory default 0xFF
        enable = tempValue;
        if(enable)
        {
            enable = 0;
        }
        else
        {
            enable = 1;
        }
        lega_flash_kv_set("sta_auto_enable", &enable, sizeof(enable), 1);
    }
    else if ((strcmp(argv[1],"ap") == 0))
    {
        tempValue = convert_str_to_int(argv[2]);
        if(tempValue == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("enable param err\n");
            return PARAM_RANGE;
        }
        // 0:enable 1:disable  as flash memory default 0xFF
        enable = tempValue;
        if(enable)
        {
            enable = 0;
        }
        else
        {
            enable = 1;
        }
        lega_flash_kv_set("sap_auto_enable", &enable, sizeof(enable), 1);
    }
    else
    {
        dbg_at("wifi_auto_enable err param\r\n");
        return PARAM_RANGE;
    }

    dbg_at("wifi %s auto_connect %s\r\n",argv[1],auto_flag[enable]);
    return CONFIG_OK;
}

#ifdef CFG_SNIFFER_SUPPORT
static int lega_at_wifi_sniffer_set_channel(int argc, char **argv)
{
    uint8_t channel = 0;

    if (argc <= 1)
    {
        dbg_at("sniffer set channel parameter error");
        return PARAM_MISS;
    }

    channel = convert_str_to_int(argv[1]);

    if ((channel <= 0) || (channel > 13))
    {
        dbg_at("sniffer set channel wrong channel:%d", channel);
        return PARAM_RANGE;
    }

    lega_wlan_monitor_set_channel(channel);
    return CONFIG_OK;
}

void lega_at_monitor_rx_cb(uint8_t*data, int len, int rssi)
{
    static uint32_t i = 0;
    if((i++)%20 == 0)
    {
        /* defined for debug */
        at_rspdata("rx:0x%x len:%d rssi:%d", *data, len, rssi);
    }
}

static int lega_at_wifi_sniffer_start(int argc, char **argv)
{
    int ret;
    int mode = lega_wlan_get_wifi_mode();
    if(mode != NON_MODE_)
    {
        dbg_at("current mode is %s,close it first!",cur_mode[mode]);
        return CONFIG_FAIL;
    }
    lega_wlan_register_monitor_cb(lega_at_monitor_rx_cb);

    ret = lega_wlan_start_monitor();
    if(ret == 0)
        return CONFIG_OK;
    else
        return CONFIG_FAIL;
}

static int lega_at_wifi_sniffer_stop(int argc, char **argv)
{
    int ret = 0;
    int mode = lega_wlan_get_wifi_mode();
    if(mode != SNIFFER)
    {
        dbg_at("current mode is %s,use wifi_close!",cur_mode[mode]);
        return CONFIG_FAIL;
    }
    lega_wlan_register_monitor_cb(NULL);

    ret = lega_wlan_stop_monitor();
    if(ret)
        return CONFIG_FAIL;
    else
        return CONFIG_OK;
}
#endif

#ifdef LWIP_APP_PING
static int lega_at_wifi_ping(int argc, char **argv)
{
    struct hostent *hostent;
    int a[4] = {0};
    int ping_data_size = 0;
    int ret = 0;

    if(argc == 1)
    {
        return PARAM_MISS;
    }
    /*check host name or ip*/
    if (inet_addr(argv[1]) == INADDR_NONE)
    {
        hostent = gethostbyname(argv[1]);
        if (hostent == NULL)
        {
            dbg_at("get host by name error\n");
            return CONFIG_FAIL;
        }
        sscanf(ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[0]),"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]);
    }
    else
    {
        argv++;
        sscanf(*argv,"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]);
    }
    if(argc==3)
    {
        argv++;
        ping_data_size = convert_str_to_int(*argv);
        if(ping_data_size == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("ping data size err\n");
            return PARAM_RANGE;
        }
    }

    IP_ADDR4(&at_user_info.at_ping,a[0],a[1],a[2],a[3]);

    ret = ping(&at_user_info.at_ping,ping_data_size);

    if (ret == -1)
    {
        return PARAM_RANGE;
    }
    else if (ret == -3)
    {
        return CONFIG_FAIL;
    }
    else
    {
        return WAIT_PEER_RSP;
    }
}
#endif

#ifdef LWIP_APP_IPERF
static int lega_at_wifi_iperf(int argc, char **argv)
{
    lega_wifi_iperf(argc, argv);
    return WAIT_PEER_RSP;
}
#endif

/*
 ************************************************************
 *                    WIFI AT CMD END
 *
 ************************************************************
 */
cmd_entry comm_wifi_open = {
    .name = "wifi_open",
    .help = "cmd format:   wifi_open\r\n\
cmd function: open wifi station mode\r\n\
cmd format:   wifi_open sta SSID\r\n\
cmd function: open wifi station mode and connect unencrypted AP\r\n\
cmd format:   wifi_open sta SSID PASSWORD\r\n\
cmd function: open wifi station mode and connect encrypted AP\r\n\
cmd format:   wifi_open sta SSID PASSWORD CHANNEL\r\n\
cmd function: open wifi station mode and connect encrypted AP on given channel\r\n\
cmd format:   wifi_open ap SSID 0 CHANNEL\r\n\
cmd function: open wifi unencrypted softap mode on given channel\r\n\
cmd format:   wifi_open ap SSID PASSWORD CHANNEL\r\n\
cmd function: open wifi encrypted softap mode on given channel\r\n\
cmd format:   wifi_open ap SSID PASSWORD CHANNEL HIDDEN_MODE BEACONINTERVAL\r\n\
cmd function: open wifi encrypted softap mode on given channel, set hidden mode(0/1) and beacon interval\r\n",
    .function = lega_at_wifi_open,
};
cmd_entry comm_wifi_open_def = {
    .name = "wifi_open_def",
    .help = "cmd format:   wifi_open_def\r\n\
cmd function: open wifi station mode and save info to flash kv part\r\n\
cmd format:   wifi_open_def sta SSID\r\n\
cmd function: open wifi station mode and connect unencrypted AP\r\n\
cmd format:   wifi_open_def sta SSID PASSWORD\r\n\
cmd function: open wifi station mode and connect encrypted AP\r\n\
cmd format:   wifi_open sta SSID PASSWORD CHANNEL\r\n\
cmd function: open wifi station mode and connect encrypted AP on given channel\r\n\
cmd format:   wifi_open_def ap SSID 0 CHANNEL\r\n\
cmd function: open wifi unencrypted softap mode on given channel\r\n\
cmd format:   wifi_open_def ap SSID PASSWORD CHANNEL\r\n\
cmd function: open wifi encrypted softap mode on given channel\r\n\
cmd format:   wifi_open_def ap SSID PASSWORD CHANNEL HIDDEN_SSID BEACONINTERVAL\r\n\
cmd function: open wifi encrypted softap mode on given channel, set hidden mode(0/1) and beacon interval\r\n",
    .function = lega_at_wifi_open,
};
cmd_entry comm_wifi_open_bssid = {
    .name = "wifi_open_bssid",
    .help = "cmd format:   wifi_open_bssid\r\n\
cmd function: open wifi station mode\r\n\
cmd format:   wifi_open_bssid sta BSSID\r\n\
cmd function: open wifi station mode and connect unencrypted AP\r\n\
cmd format:   wifi_open_bssid sta BSSID PASSWORD\r\n\
cmd function: open wifi station mode and connect encrypted AP\r\n\
cmd format:   wifi_open_bssid sta BSSID PASSWORD CHANNEL\r\n\
cmd function: open wifi station mode and connect encrypted AP on given channel\r\n",
    .function = lega_at_wifi_open,
};
cmd_entry comm_wifi_close = {
    .name = "wifi_close",
    .help = "cmd format:   wifi_close\r\n\
cmd function: close wifi station mode or softap mode\r\n",
    .function = lega_at_wifi_close,
};
cmd_entry comm_wifi_set_dhcp = {
    .name = "wifi_set_dhcp",
    .help = "cmd format:   wifi_set_dhcp 0/1\r\n\
cmd function: enable or disable dhcp\r\n" ,
    .function = lega_at_wifi_set_dhcp,
};
cmd_entry comm_wifi_set_sta_ip = {
    .name = "wifi_set_sta_ip",
    .help = "cmd format:   wifi_set_sta_ip ip gateway netmask\r\n\
cmd function: set sta mode static ip config\r\n",
    .function = lega_at_wifi_set_sta_ip,
};
cmd_entry comm_wifi_set_sta_ip_def = {
    .name = "wifi_set_sta_ip_def",
    .help = "cmd format:   wifi_set_sta_ip_def ip gateway netmask\r\n\
cmd function: set sta mode static ip config and save in flash kv part\r\n",
    .function = lega_at_wifi_set_sta_ip,
};
cmd_entry comm_wifi_set_softap_ip = {
    .name = "wifi_set_softap_ip",
    .help = "cmd format:   wifi_set_softap_ip ip gateway netmask [ip_start] [ip_end].\r\n\
cmd function: set softap mode ip config \r\n",
    .function = lega_at_wifi_set_softap_ip,
};
cmd_entry comm_wifi_set_softap_ip_def = {
    .name = "wifi_set_softap_ip_def",
    .help = "cmd format:   wifi_set_softap_ip_def ip gateway netmask [ip_start] [ip_end].\r\n\
cmd function: set softap mode ip config and save in flash kv part\r\n",
    .function = lega_at_wifi_set_softap_ip,
};
cmd_entry comm_wifi_set_auto_enable = {
    .name = "wifi_set_auto_enable",
    .help = "cmd format:   wifi_set_auto_enable sta/ap 0/1\r\n\
cmd function: auto start softap or sta after reset\r\n",
    .function = lega_at_wifi_auto_enable,
};
cmd_entry comm_wifi_set_mac_addr = {
    .name = "wifi_set_mac_addr",
    .help = "cmd format:   wifi_set_mac_addr MAC_ADDR(e.g. AA00CCEFCDAB)\r\n\
cmd function: set default mac address of device, if mac address is not written to efuse\r\n",
    .function = lega_at_wifi_set_macaddr,
};
cmd_entry comm_wifi_get_mac_addr = {
    .name = "wifi_get_mac_addr",
    .help = "cmd format:   wifi_get_mac_addr\r\n\
cmd function: get mac address of device\r\n",
    .function = lega_at_wifi_get_macaddr,
};
#ifdef CFG_INDIGO_ADAPTER
cmd_entry comm_wifi_start_loopback_server = {
    .name = "wifi_start_loopback_server",
    .help = "cmd format:   wifi_start_loopback_server\r\n\
cmd function: start loopback server for indigo proj\r\n",
    .function = lega_at_wifi_start_loopback_server,
};
cmd_entry comm_wifi_stop_loopback_server = {
    .name = "wifi_stop_loopback_server",
    .help = "cmd format:   wifi_stop_loopback_server\r\n\
cmd function: stop loopback server for indigo proj\r\n",
    .function = lega_at_wifi_stop_loopback_server,
};

cmd_entry comm_wifi_sta_associate = {
    .name = "wifi_sta_associate",
    .help = "cmd format:   wifi_sta_associate\r\n\
cmd function: do sta associate for indigo proj\r\n",
    .function = lega_at_wifi_sta_associate,
};

cmd_entry comm_wifi_sta_disconnect = {
    .name = "wifi_sta_disconnect",
    .help = "cmd format:   wifi_sta_disconnect\r\n\
cmd function: do sta disconnect for indigo proj\r\n",
    .function = lega_at_wifi_sta_disconnect,
};

cmd_entry comm_wifi_sta_dhcp = {
    .name = "wifi_sta_dhcp",
    .help = "cmd format:   wifi_sta_dhcp\r\n\
cmd function: do sta eapol and dhcp for indigo proj\r\n",
    .function = lega_at_wifi_sta_dhcp,
};

cmd_entry comm_wifi_sta_is_connect = {
    .name = "wifi_sta_is_connect",
    .help = "cmd format:   wifi_sta_is_connect\r\n\
cmd function: check sta is connect for indigo proj\r\n",
    .function = lega_at_wifi_sta_is_connect,
};

#endif//CFG_INDIGO_ADAPTER
cmd_entry comm_wifi_get_peer_sta_info = {
    .name = "wifi_get_peer_sta_info",
    .help = "cmd format:   wifi_get_peer_sta_info\r\n\
cmd function: used in softap mode. to get peer stations connect info. including ip_addr and mac_addr\r\n",
    .function = lega_at_softap_get_sta_info,
};
cmd_entry comm_wifi_softap_deauth_peer = {
    .name = "wifi_ap_deauth_peer",
    .help = "cmd format:   wifi_ap_deauth_peer MAC_ADDR(e.g. AA00CCEFCDAB)\r\n\
cmd function: deauth peer station in softap mode\r\n",
    .function = lega_at_softap_deauth_peer,
};
cmd_entry comm_wifi_get_softap_info = {
    .name = "wifi_get_softap_info",
    .help = "cmd format:   wifi_get_softap_info\r\n\
cmd function: used in softap mode. to get channel, ssid and security type of dut softap\r\n",
    .function = lega_at_get_softap_info,
};
cmd_entry comm_wifi_get_ip = {
    .name = "wifi_get_ip",
    .help = "cmd format:   wifi_get_ip\r\n\
cmd function: used in sta and softap mode. to get ip and gateway\r\n",
    .function = lega_at_get_ip,
};
cmd_entry comm_wifi_get_sta_link_info = {
    .name = "wifi_get_sta_info",
    .help = "cmd format:   wifi_get_sta_info\r\n\
cmd function: used in sta mode. call after connected AP to get link info\r\n",
    .function = lega_at_get_sta_link_info,
};
cmd_entry comm_wifi_get_wifi_mode = {
    .name = "wifi_get_mode",
    .help = "cmd format:   wifi_get_mode\r\n\
cmd function: used in sta/softap/sniffer mode. to get current wifi mode\r\n",
    .function = lega_at_get_wifi_mode,
};
cmd_entry comm_wifi_scan = {
    .name = "wifi_scan",
    .help = "cmd format:   wifi_scan\r\n\
cmd function: scan AP on all channel, used in station mode\r\n",
    .function = lega_at_wifi_scan,
};
cmd_entry comm_wifi_set_txrate = {
    .name = "wifi_set_txrate",
    .help = "cmd format:   wifi_set_txrate mode [rate] [gi]\r\n\
mode: b,g,n,auto. when use auto mode, param rate and gi no need.\r\n\
rate: 1,2,5,11 for b, 6,9,12,18,24,36,48,54 for g, 0~7 for n.\r\n\
gi:   long or short for n mode,default long-gi.\r\n\
cmd function: set tx data-frame rate\r\n",
    .function = lega_at_wifi_set_txdatarate,
};
cmd_entry comm_wifi_enable_log = {
    .name = "wifi_enable_log",
    .help = "cmd format:   wifi_enable_log\r\n\
cmd function: enable wifi stack log\r\n",
    .function = lega_at_wifi_enable_log,
};
cmd_entry comm_wifi_disable_log = {
    .name = "wifi_disable_log",
    .help = "cmd format:   wifi_disable_log\r\n\
cmd function: disable wifi stack log\r\n",
    .function = lega_at_wifi_disable_log,
};
#ifdef CFG_SNIFFER_SUPPORT
cmd_entry comm_wifi_sniffer_start = {
    .name = "wifi_sniffer_start",
    .help = "cmd format:   wifi_sniffer_start\r\n\
cmd function: open wifi sniffer mode\r\n",
    .function = lega_at_wifi_sniffer_start,
};
cmd_entry comm_wifi_sniffer_set_chan = {
    .name = "wifi_sniffer_set_chan",
    .help = "cmd format:   wifi_sniffer_set_channel CHANNEL(e.g. 1)\r\n\
cmd function: set channel of sniffer mode\r\n",
    .function = lega_at_wifi_sniffer_set_channel,
};
cmd_entry comm_wifi_sniffer_stop = {
    .name = "wifi_sniffer_stop",
    .help = "cmd format:   wifi_sniffer_stop\r\n\
cmd function: close wifi sniffer mode\r\n",
    .function = lega_at_wifi_sniffer_stop,
};
#endif
#ifdef LWIP_APP_PING
cmd_entry comm_ping = {
    .name = "ping",
    .help = "cmd format:   ping IP_ADDR(e.g. 192.168.1.1)\r\n\
cmd function: ping given ip address\r\n",
    .function = lega_at_wifi_ping,
};
#endif
#ifdef LWIP_APP_IPERF
cmd_entry comm_iperf = {
    .name = "iperf",
    .help = "cmd format:   iperf -c host [-p port] [-t times] [-u] [-b bandwith] [-a]\r\n\
cmd format:   iperf -s [-p port] [-u] [-a]\r\n\
cmd function:\r\n\
-c host :run as iperf client connect to host\r\n\
-s      :run as iperf server\r\n\
-S tos  :TOS value(one of below): BE BK VI VO\r\n\
-B host :multicast host\r\n\
-p port :client connect to/server port default 5001\r\n\
-u      :use udp do iperf client/server\r\n\
         If -u not enable, use tcp default\r\n\
-b      :set transmission bandwith in Kbits/sec, udp client only\r\n\
-t      :set transmission timeout in seconds, client only\r\n\
-a      :abort iperf client/server/all\r\n\
-d      :tx packets delay time(ms) default 10\r\n",
    .function = lega_at_wifi_iperf,
};
#endif

void lega_at_wifi_cmd_register(void)
{
    lega_at_cmd_register(&comm_wifi_open);
    lega_at_cmd_register(&comm_wifi_open_def);
    lega_at_cmd_register(&comm_wifi_open_bssid);
    lega_at_cmd_register(&comm_wifi_close);
    lega_at_cmd_register(&comm_wifi_set_dhcp);
    lega_at_cmd_register(&comm_wifi_set_sta_ip);
    lega_at_cmd_register(&comm_wifi_set_sta_ip_def);
    lega_at_cmd_register(&comm_wifi_set_softap_ip);
    lega_at_cmd_register(&comm_wifi_set_softap_ip_def);
    lega_at_cmd_register(&comm_wifi_set_auto_enable);
    lega_at_cmd_register(&comm_wifi_get_mac_addr);
    #ifdef CFG_INDIGO_ADAPTER
    lega_at_cmd_register(&comm_wifi_start_loopback_server);
    lega_at_cmd_register(&comm_wifi_stop_loopback_server);
    lega_at_cmd_register(&comm_wifi_sta_associate);
    lega_at_cmd_register(&comm_wifi_sta_disconnect);
    lega_at_cmd_register(&comm_wifi_sta_dhcp);
    lega_at_cmd_register(&comm_wifi_sta_is_connect);
    #endif
    lega_at_cmd_register(&comm_wifi_set_mac_addr);
    lega_at_cmd_register(&comm_wifi_get_peer_sta_info);
    lega_at_cmd_register(&comm_wifi_get_softap_info);
    lega_at_cmd_register(&comm_wifi_softap_deauth_peer);
    lega_at_cmd_register(&comm_wifi_get_wifi_mode);
    lega_at_cmd_register(&comm_wifi_get_ip);
    lega_at_cmd_register(&comm_wifi_get_sta_link_info);
    lega_at_cmd_register(&comm_wifi_scan);
    lega_at_cmd_register(&comm_wifi_set_txrate);
    lega_at_cmd_register(&comm_wifi_enable_log);
    lega_at_cmd_register(&comm_wifi_disable_log);
#ifdef CFG_SNIFFER_SUPPORT
    lega_at_cmd_register(&comm_wifi_sniffer_start);
    lega_at_cmd_register(&comm_wifi_sniffer_stop);
    lega_at_cmd_register(&comm_wifi_sniffer_set_chan);
#endif // CFG_SNIFFER_SUPPORT
#ifdef LWIP_APP_PING
    lega_at_cmd_register(&comm_ping);
#endif // LWIP_APP_PING
#ifdef LWIP_APP_IPERF
    lega_at_cmd_register(&comm_iperf);
#endif // LWIP_APP_IPERF
}
#endif
