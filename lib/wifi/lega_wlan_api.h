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

/**
 ****************************************************************************************
 *
 * @file lega_wifi_api.h
 *
 * @brief WiFi API.
 *
 ****************************************************************************************
 */
#ifndef _LEGA_WIFI_API_H_
#define _LEGA_WIFI_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 *  @brief  wlan network interface enumeration definition.
 */
typedef enum {
    STA=0x1,        /*Act as a station which can connect to an access point*/
    SOFTAP,         /*Act as an access point, and other station can connect, 4 stations Max*/
    SNIFFER,        /*Act as a sniffer*/
} lega_wlan_type_e;

typedef enum {
    WLAN_DHCP_DISABLE = 0,  //use static ip address in station mode
    WLAN_DHCP_CLIENT,
    WLAN_DHCP_SERVER,
} lega_wlan_dhcp_mode_e;

typedef enum {
    EVENT_STATION_UP = 1,   /* used in station mode,indicate station associated
                              in open mode or 4-way-handshake done in WPA/WPA2 */
    EVENT_STATION_DOWN,     /* used in station mode, indicate station deauthed */
    EVENT_STA_CLOSE,        /* used in station mode, indicate station close done */
    EVENT_AP_UP,            /* used in softap mode, indicate softap enabled    */
    EVENT_AP_DOWN,          /* used in softap mode, indicate softap disabled   */
} lega_wifi_event_e;

/**
 *  @brief  Scan result using normal scan.
 */
typedef struct {
  uint8_t is_scan_adv;      /* no use currently */
  char ap_num;              /* number of access points found in scanning. */
  struct {
    char    ssid[32+1];     /* ssid max len:32. +1 is for '\0'. when ssidlen is 32  */
    char    ap_power;       /* Signal strength, min:0, max:100. */
    char    bssid[6];       /* BSSID of an access point. */
    char    channel;        /* RF frequency, 1-13 */
    uint8_t security;       /* Security type, @ref wlan_sec_type_t */
  } * ap_list;
} lega_wlan_scan_result_t;

typedef enum {
    WLAN_SECURITY_OPEN,     //NONE
    WLAN_SECURITY_WEP,      //WEP
    WLAN_SECURITY_WPA,      //WPA
    WLAN_SECURITY_WPA2,     //WPA2
    WLAN_SECURITY_AUTO,     //WPA or WPA2
    WLAN_SECURITY_MAX,
} lega_wlan_security_e;

/*used in event callback of station mode, indicate softap informatino which is connected*/
typedef struct {
    int     rssi;           /* rssi */
    char    ssid[32+1];     /* ssid max len:32. +1 is for '\0' when ssidlen is 32  */
    char    pwd[64+1];      /* pwd max len:64. +1 is for '\0' when pwdlen is 64 */
    char    bssid[6];       /* BSSID of the wlan needs to be connected.*/
    char    ssid_len;       /*ssid length*/
    char    pwd_len;        /*password length*/
    char    channel;        /* wifi channel 0-13.*/
    char    security;       /*refer to lega_wlan_security_e*/
} lega_wlan_ap_info_adv_t;

#ifdef LWIP_DUALSTACK
/* only used in ipv6 mode */
typedef struct {
   char     addr[40];        /* IPV6 IP addresses */
   char     state;           /* the state of IPV6 addr */
}lega_ip6_addr_t;
#endif

/*used in station and softap mode*/
typedef struct {
    char    start_ip[16];     /* start ip addr of dhcp pool in softap mode */
    char    end_ip[16];       /* end ip addr of dhcp pool in softap mode*/
    char    dhcp;             /* refer to lega_wlan_dhcp_mode_e */
    char    macaddr[16];      /* mac address on the target wlan interface, e.g. "AA1122334455"*/
    char    ip[16];           /* Local IP address on the target wlan interface, e.g. "192.168.1.100" */
    char    gate[16];         /* Router IP address on the target wlan interface, "192.168.1.1" */
    char    mask[16];         /* Netmask on the target wlan interface, "255.255.255.0" */
    char    dns[16];          /* no use currently , ASCII*/
    char    broadcastip[16];  /* no use currently , ASCII*/
#ifdef LWIP_DUALSTACK
    lega_ip6_addr_t  ip6[3];  /* IPV6 IP addresses , number of per netif is 3 */
#endif
} lega_wlan_ip_stat_t;

/*only used in station mode*/
typedef struct {
    int     is_connected;    /* The link to wlan is established or not, 0: disconnected, 1: connected. */
    int     wifi_strength;   /* Signal strength of the current connected AP */
    char    ssid[32+1];      /* ssid max len:32. +1 is for '\0'. when ssidlen is 32  */
    char    bssid[6];        /* BSSID of the current connected wlan */
    int     channel;         /* Channel of the current connected wlan */
} lega_wlan_link_stat_t;

/*used in open cmd of hal_wifi_module_t*/
typedef struct {
    char    wifi_mode;              /* refer to lega_wlan_type_e*/
    char    security;               /* security mode, refer to lega_wlan_security_e */
    char    wifi_ssid[32];          /* in station mode, indicate SSID of the wlan needs to be connected.
                                       in softap mode, indicate softap SSID*/
    char    wifi_key[64];           /* in station mode, indicate Security key of the wlan needs to be connected,
                                       in softap mode, indicate softap password.(ignored in an open system.) */
    int     key_len;                /* Security key length*/
    char    local_ip_addr[16];      /* in softap mode to config ip for dut.
                                       in station mode, to config static ip when set dhcp_mode to WLAN_DHCP_DISABLE.*/
    char    net_mask[16];           /* in softap mode to config net mask for dut.
                                       in station mode, to config static ip net mask when set dhcp_mode to WLAN_DHCP_DISABLE.*/
    char    gateway_ip_addr[16];    /* in softap mode to config gateway for dut.
                                       in station mode, to config static ip gateway when set dhcp_mode to WLAN_DHCP_DISABLE.*/
    char    dns_server_ip_addr[16]; /* no use currently */
    char    start_ip[16];           /* start ip addr of dhcp pool in softap mode */
    char    end_ip[16];             /* end ip addr of dhcp pool in softap mode*/
    char    dhcp_mode;              /* refer to lega_wlan_dhcp_mode_e*/
    char    channel;                /* softap channel in softap mode; connect channel in sta mode*/
    char    mac_addr[6];            /* connect bssid in sta mode*/
    char    reserved[32];           /* no use currently */
    int     wifi_retry_interval;    /* no use currently */
    int     wifi_retry_times;       /* used in station mode to config reconnecting times after disconnected*/
    int     interval;               /* used in softap mode to config beacon listen interval */
    int     hide;                   /* used in softap mode to config hidden SSID */
} lega_wlan_init_type_t;

typedef struct {
    int  rssi;                     /* rssi of connected ap for station mode */
    int  snr;                      /* snr of connected ap for station mode */
    int  per;                      /* per of tx data failed for station mode(ppm) */
    unsigned long long tx_total;   /* statistics total transmit pkt for station mode */
    unsigned long long tx_fail;    /* statistics total transmit pkt for station mode */
} lega_wireless_info_t;

typedef struct
{
    char ssid[32+1];
    char ssid_len;
    char chan;                 /*working channel*/
    char security;             /*softap mode security type, refer to lega_wlan_security_e*/
}lega_wlan_softap_info_t;

/**
 * @brief sta ip and mac address used in softap mode
 * sta_ip_addr: e.g. when ip addr==192.168.1.1<-->sta_ip_addr == 0x0101A8C0
 */
typedef struct{
    uint32_t sta_ip_addr;                           /* station ip addr  */
    uint8_t  sta_mac_addr[6];                       /* station mac addr */
}lega_wlan_client_addr_info_t;

/*store linked station info*/
typedef struct{
    int client_num;                                 /* linked station number */
    lega_wlan_client_addr_info_t sta[4];            /* linked station entry, max client number is 4 */
}lega_wlan_ap_client_info_t;

/*WLAN error status*/
typedef enum{
    WLAN_STA_MODE_BEACON_LOSS = 1,           //in sta mode, cannot receive beacon of peer connected AP for a long time
    WLAN_STA_MODE_AUTH_FAIL,                 //in sta mode, connect fail during auth
    WLAN_STA_MODE_ASSOC_FAIL,                //in sta mode, connect fail during association
    WLAN_STA_MODE_PASSWORD_ERR,              //in sta mode, connect fail as password error
    WLAN_STA_MODE_NO_AP_FOUND,               //in sta mode, connect fail as cannot find the connecting AP during scan
    WLAN_STA_MODE_DHCP_FAIL,                 //in sta mode, connect fail as dhcp fail
    WLAN_STA_MODE_CONN_RETRY_MAX,            //in sta mode, connect fail as reach the max connect retry times
}lega_wlan_err_status_e;

typedef enum
{
    SLEEP_TYPE_NONE,
    SLEEP_TYPE_MODEM,
    SLEEP_TYPE_LIGHT,
    SLEEP_TYPE_DEEP,
    SLEEP_TYPE_MAX,
}lega_sleep_type_e;

typedef struct
{
   uint32_t sleep_ms;
   uint8_t wakeup_gpio; //GPIO 12 and 13 valid
}lega_sleep_config_t;

#ifdef CFG_STA_AP_COEX
typedef enum
{
    STAP_COEX_STA_IDX,
    STAP_COEX_AP_IDX,
    STAP_COEX_MAX_IDX
}lega_stap_coex_e;
#endif

/** @brief  wifi init functin, user should call it before use any wifi cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_init(void);

/** @brief  wifi deinit functin, call it when donot use wifi any more to free resources
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_deinit(void);

/** @brief  used in station and softap mode, open wifi cmd
 *
 * @param init_info    : refer to lega_wlan_init_type_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_open(lega_wlan_init_type_t* init_info);

/** @brief  used in station and softap mode, close wifi cmd
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_close(void);

/** @brief  used in station mode, scan cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan(void);

/** @brief  used in station mode, scan cmd
 *
 * @param ssid    : target ssid to scan
 * @param is_scan_advance : not use curently
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan_active(const char *ssid, uint8_t is_scan_advance);

/** @brief  used in station mode, scan cmd
 *
 * @param ssid    : target ssid to scan, could be null
 * @param bssid   : target bssid to scan, could be null
 * @param channel : target channel to scan, could be null
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan_detail(char *ssid, int channel, char *bssid);

/** @brief  get mac address(in hex mode) of WIFI device
 *
 * @param mac_addr    : pointer to get the mac address
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_get_mac_address(uint8_t *mac_addr);

/** @brief  set mac address for WIFI device
 *
 *  @param mac_addr    : 6 bytes src mac array to set, DOES NOT support string or pointer
 *
 *  @return    0       : on success.
 *  @return    -1      : NULL pointer
 *  @return    -2      : mac address already exist in efuse, can't set
 *  @return    -3      : param is invalid
 */
int lega_wlan_set_mac_address(uint8_t *mac_addr);


/** @brief  used in station mode, get the ip information
 *
 * @param void
 * @return    NULL    : error occurred.
 * @return    pointer : ip status got.
 */
lega_wlan_ip_stat_t * lega_wlan_get_ip_status(void);


/** @brief  used in station mode, get link status information
 *
 * @param link_status    : refer to lega_wlan_link_stat_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_get_link_status(lega_wlan_link_stat_t *link_status);

/** @brief  used in station mode, get the associated ap information
 *
 * @param void
 * @return    NULL    : error occurred.
 * @return    pointer : associated ap info got.
 */
lega_wlan_ap_info_adv_t *lega_wlan_get_associated_apinfo(void);

/*used in sniffer mode, open sniffer mode
*  @return    0       : on success.
*  @return    other   : error occurred
*/
int lega_wlan_start_monitor(void);

/*used in sniffer mode, close sniffer mode
*  @return    0       : on success.
*  @return    other   : error occurred
*/
int lega_wlan_stop_monitor(void);

/** @brief  used in sniffer mode, set the sniffer channel, should call this cmd after call start_monitor cmd
 *
 * @param channel    : WIFI channel(1-13)
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_monitor_set_channel(int channel);

/** @brief  used to get current channel both in sta and ap mode
 *
 *  @return    1-14 : channel number.
 *  @return    0   :  no valid channel
 */
int lega_wlan_get_channel(void);

/** @brief  used in sta mode, set the ps bc mc and listen interval, called before connect to ap.
 *
 * @param listen_bc_mc    : true or false
 * @param listen_interval :1, 3, 10
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_set_ps_options(uint8_t listen_bc_mc, uint16_t listen_interval);

/** @brief  used in sta mode, set ps mode on/off
 *
 * @param ps_on    : true or false
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_set_ps_mode(uint8_t ps_on);


/*when use monitor mode, user should register this type of callback function to get the received MPDU*/
typedef void (*monitor_cb_t)(uint8_t*data, int len, int rssi);

/** @brief  used in sniffer mode, callback function to get received MPDU, should register before start_monitor
 *
 * @param fn    : refer to monitor_data_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_monitor_cb(monitor_cb_t fn);

/** @brief  used in station mode or sniffer mode, call this cmd to send a MPDU constructed by user
 *
 * @param buf    :  mac header pointer of the MPDU
 * @param len    :  length of the MPDU
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_send_raw_frame(uint8_t *buf, int len);

/*enable WIFI stack log, will be output by uart
*
* @return    0       : on success.
* @return    other   : error occurred
*/
int lega_wlan_start_debug_mode(void);

/*disable WIFI stack log
*
* @return    0       : on success.
* @return    other   : error occurred
*/
int lega_wlan_stop_debug_mode(void);

/*
 * The event callback function called at specific wifi events occurred by wifi stack.
 * user should register these callback if want to use the informatin.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef void (*lega_wlan_cb_ip_got)(lega_wlan_ip_stat_t *ip_status);
/** @brief  used in station mode, WIFI stack call this cb when get ip
 *
 * @param fn    : cb function type, refer to lega_wlan_ip_stat_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_ip_got_cb_register(lega_wlan_cb_ip_got fn);


typedef void (*lega_wlan_cb_stat_chg)(lega_wifi_event_e wlan_event);
/** @brief  used in station and softap mode,
 *          WIFI stack call this cb when status change, refer to lega_wifi_event_e
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_stat_chg_cb_register(lega_wlan_cb_stat_chg fn);

typedef void (*lega_wlan_cb_scan_compeleted)(lega_wlan_scan_result_t *result);
/** @brief  used in station mode,
 *          WIFI stack call this cb when scan complete
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_scan_compeleted_cb_register(lega_wlan_cb_scan_compeleted fn);

typedef void (*lega_wlan_cb_associated_ap)(lega_wlan_ap_info_adv_t *ap_info);
/** @brief  used in station mode,
 *          WIFI stack call this cb when associated with an AP, and tell the AP information
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_associated_ap_cb_register(lega_wlan_cb_associated_ap fn);

typedef void (*lega_wlan_cb_ap_peer_change)(lega_wlan_client_addr_info_t *peer_info, uint8_t is_connect);
/** @brief  used in softap mode,
 *          WIFI stack call this cb when peer station connect or disconnect.
 *          peer_info indicate peer station information, connect indicate status: 1 means connect, 0 means disconnect
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_ap_peer_change_cb_register(lega_wlan_cb_ap_peer_change fn);

/** @brief  calibration RCO clock for RTC
 *
 */
void lega_drv_rco_cal(void);

/** @brief  config to close DCDC PFM mode
 *
 */
void lega_drv_close_dcdc_pfm(void);

/** @brief  config to open DCDC PFM mode
 *
 */
void lega_drv_open_dcdc_pfm(void);

/** @brief  set country code to update country code, different country may have different channel list
 *          support channel 1-13 by default
 *          called after lega_wlan_init
 *
 * @param country  : country code string, currently support: "CN" "EU" "JP" "US"
 *
 * @return    0       : on success.
 * @return    -1      : parameter country illegal
 * @return    -2      : country code not supported
 */
int lega_wlan_set_country_code(char *country);

/** @brief  get country code, different country may have different channel list
 *
 * @return    pointer: pointer to an array with two elements, for example char cc[2] = {'C', 'N'}.
 */
char *lega_wlan_get_country_code(void);

/** @brief  get linked station ip and address in ap mode
 *  @param            : input param to get sta ip and mac addr
 */
void lega_get_client_ip_mac(lega_wlan_ap_client_info_t* sta_addr);

typedef void (*lega_wlan_err_stat_handler)(lega_wlan_err_status_e err_info);
/** @brief  user use to register err status callback function,
 *          WIFI stack call this cb when some error occured, refer to lega_wlan_err_status_e
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_err_stat_cb_register(lega_wlan_err_stat_handler fn);

/** @brief  get current wifi mode
 * *
 * @return  : current wifi mode, refer to lega_wlan_type_e
 */
int lega_wlan_get_wifi_mode(void);

/** @brief  used in softap mode, get softap basic information
 *
 * @param ptr    : refer to lega_wlan_softap_info_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_get_softap_info(lega_wlan_softap_info_t *ptr);

/** @brief  used in softap mode, deauth connected peer device
 *
 * @param mac_addr    : pointer to the mac address
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_softap_deauth_peer(uint8_t *mac);

/** @brief set certifacation type
 *
 *  @param type : cert type, 0x00: old srrc, 0x10: new srrc
 *
 *  @note For setting cert type API, user should call it before func lega_sram_rf_pta_init
 */
void lega_wlan_set_cert_type(uint8_t type);

/** @brief  get current certifacation type
 *
 *  @return value : cert type, 0x00: old srrc, 0x10: new srrc
 */
uint8_t lega_wlan_get_cert_type(void);

#ifdef __cplusplus
}
#endif

#endif  //_LEGA_WIFI_API_H_
