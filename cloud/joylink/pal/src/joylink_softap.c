#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "joylink.h"

//example.
#include "joylink_extern.h"
#include "joylink_extern_ota.h"
#include "joylink_extern_json.h"

//joylink.
#include "joylink_ret_code.h"

//pal.
#include "joylink_flash.h"
#include "joylink_log.h"
#include "joylink_memory.h"
#include "joylink_socket.h"
#include "joylink_softap.h"
#include "joylink_stdint.h"
#include "joylink_stdio.h"
#include "joylink_string.h"
#include "joylink_log.h"
#include "joylink_thread.h"
#include "joylink_time.h"

#include "joylink_sdk.h"

//lega board.
#include "lega_wlan_api.h"
// #include "lega_flash_kv.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define     CLOUD_JOYLINK_MAIN_TASK_NAME              "Joylink_Main"
#define     CLOUD_JOYLINK_MAIN_STACK_SIZE             (2048*15)

/****************************************************************************
 * Public Data
****************************************************************************/

/****************************************************************************
 * Private Data
****************************************************************************/
static lega_wlan_init_type_t wifi_info;

/****************************************************************************
 * Public Function
****************************************************************************/
/**
 * @brief:set wifi mode to AP mode.
 *
 * @returns:success 0, others failed
 */
int32_t jl_softap_enter_ap_mode(void)
{
    // open wifi whit AP mode
    //1. close wifi
    lega_wlan_close();//close and open again.
    jl_platform_msleep(1000);

    jl_platform_memset(&wifi_info, 0x00, sizeof(wifi_info));

    snprintf((char *)wifi_info.wifi_ssid, sizeof(wifi_info.wifi_ssid), AP_SSID);
    wifi_info.dhcp_mode = WLAN_DHCP_SERVER;
    wifi_info.wifi_mode = SOFTAP;

#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("ap ssid:%s\n", wifi_info.wifi_ssid);
#endif

    return lega_wlan_open(&wifi_info);
}


/**
 * @brief:System is epected to gxet product information that user regiested in Cloud platform.
 *
 * @param[out]: UUID , max length: 8
 * @param[out]: BRAND, max length: 8
 * @param[out]: CID,   max length: 8
 * @param[out]: dev_soft_ssid max length: 32
 *
 * @returns:success 0, others failed
 */
int32_t jl_softap_get_product_info(char *uuid, char *brand, char *cid, char *dev_soft_ssid)
{
    char *_uuid = JLP_UUID;
    jl_platform_strcpy(uuid, _uuid);

    char* _dev_soft_ssid = AP_SSID;
    jl_platform_strcpy(dev_soft_ssid, _dev_soft_ssid);

    char *_cid = JLP_CID;
    jl_platform_strcpy(cid, _cid);

    char *_brand = JLP_BRAND;
    jl_platform_strcpy(brand, _brand);

#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("jl_softap_get_product_info->uuid:%s\n", uuid);
    jl_platform_printf("jl_softap_get_product_info->dev_soft_ssid:%s\n", dev_soft_ssid);
    jl_platform_printf("jl_softap_get_product_info->cid:%s\n", cid);
    jl_platform_printf("jl_softap_get_product_info->brand:%s\n", brand);
#endif
    return 0;
}

static void jl_softap_lega_wlan_cb_ip_got_handler(lega_wlan_ip_stat_t *ip_status)
{
#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("ip: %s\n", ip_status->ip);
    jl_platform_printf("gate: %s\n", ip_status->gate);
    jl_platform_printf("mask: %s\n", ip_status->mask);
#endif

    //2. networking
    jl_thread_t joylink_main_task_id;
    joylink_main_task_id.thread_task = (threadtask) joylink_main_start;
    joylink_main_task_id.stackSize = CLOUD_JOYLINK_MAIN_STACK_SIZE;
    joylink_main_task_id.priority = JL_THREAD_PRI_DEFAULT;
    joylink_main_task_id.name = CLOUD_JOYLINK_MAIN_TASK_NAME;
    joylink_main_task_id.parameter = NULL;
    jl_platform_thread_start(&joylink_main_task_id);

    joylink_dev_lan_active_switch(1);//开启局域网激活模式
}

static void jl_softap_lega_wlan_scan_compeleted_handler(lega_wlan_scan_result_t *result)
{
#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("ap scaned number: %d\n", result->ap_num);
#endif
}

static void jl_softap_lega_wlan_cb_stat_chg_handler(lega_wifi_event_e wlan_event)
{
#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("status change: %d\n", wlan_event);
#endif
}

/**
 * @brief:System is expected to connect wifi router with the in parameters.
 *
 * @param[in]: ssid of wifi router
 * @param[in]: passwd  of wifi router
 *
 * @returns:success 0, others failed
 */
int32_t jl_softap_connect_wifi_router(char *ssid, char *passwd)
{
    int32_t ret;

    // step 1 close AP mode
    lega_wlan_close();
    jl_platform_msleep(1000);

    jl_platform_memset(&wifi_info, 0x00, sizeof(wifi_info));

    // step 2 connect to the router
    if (lega_wlan_scan_compeleted_cb_register(&jl_softap_lega_wlan_scan_compeleted_handler) != 0)
    {
        jl_platform_printf("jl_softap_lega_wlan_scan_compeleted_cb_register failure!\n");
    }
    if (lega_wlan_stat_chg_cb_register(&jl_softap_lega_wlan_cb_stat_chg_handler) != 0)
    {
        jl_platform_printf("jl_softap_lega_wlan_stat_chg_cb_register failure!\n");
    }
    if (lega_wlan_ip_got_cb_register(&jl_softap_lega_wlan_cb_ip_got_handler) != 0)
    {
        jl_platform_printf("jl_softap_lega_wlan_ip_got_cb_register failure!\n");
    }

    snprintf((char *)wifi_info.wifi_ssid, sizeof(wifi_info.wifi_ssid), ssid);
    snprintf((char *)wifi_info.wifi_key, sizeof(wifi_info.wifi_key), passwd);
    wifi_info.dhcp_mode = WLAN_DHCP_CLIENT;
    wifi_info.wifi_mode = STA;

#ifdef JOYLINK_SOFTAP_DEBUG_INFO
    jl_platform_printf("sta ssid:%s\n", wifi_info.wifi_ssid);
    jl_platform_printf("sta passwd:%s\n", wifi_info.wifi_key);
#endif

    ret = lega_wlan_open(&wifi_info);
//    lega_wlan_start_scan();

    return ret;
}

