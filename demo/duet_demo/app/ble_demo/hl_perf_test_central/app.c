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
 * @file app.c
 *
 * @brief Application entry point
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#include "user_platform.h"
#include "sonata_ble_hook.h"
#include "sonata_log.h"
#include "lega_rtos_api.h"
#include "app_uart.h"
#include "user_platform.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define DEMO_SCAN (1)
#define DEMO_INIT (0)

#define DEMO_DISC (0)
#define DEMO_SCAN_AND_CONNECT  (1)  //Start scan, if target1 was scanned, then connect it
#define DEMO_LONG_RANGE (0)
/*!
 * @brief peer device's address, this demo will connect them one by one
 */
uint8_t targetAddr1[SONATA_GAP_BD_ADDR_LEN] = {0xf5, 0x48, 0x49, 0xdc, 0x59, 0x8c};
uint8_t targetAddr2[SONATA_GAP_BD_ADDR_LEN] = {0x11, 0x11, 0x00, 0x00, 0x11, 0x11};
uint8_t targetAddr3[SONATA_GAP_BD_ADDR_LEN] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0xAA};
uint8_t targetAddr4[SONATA_GAP_BD_ADDR_LEN] = {0xf5, 0x48, 0x49, 0xdc, 0x59, 0x8c};


static uint16_t demo_handle_id = 0X001E;
static bool bFound = false;
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

typedef enum
{
    CONN_TARGET_1,
    CONN_TARGET_2,
    CONN_TARGET_3,
    CONN_TARGET_4,
    CONN_TARGET_5,
} conn_step;

static conn_step gConnSteps = CONN_TARGET_1;


/*
 * EXTERNAL FUNCTION DECLARATION
 ****************************************************************************************
 */
extern int __wrap_printf(const char *format, ...);
extern void ble_reset_cmp(void);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief config legacy advertising
 */
static void app_ble_config_legacy_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0xE2;
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    //msg->adv_param.adv_param.peer_addr.addr.addr:00
    param.addr_type = 0;
    param.adv_intv_min = 160;
    param.adv_intv_max = 160;
    param.chnl_map = 0x07;
    param.phy = SONATA_GAP_PHY_LE_1MBPS;
    //Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    uint16_t ret = sonata_ble_config_legacy_advertising(SONATA_GAP_STATIC_ADDR, &param);
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief Config scanning
 */
static void app_ble_config_scanning()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    APP_TRC("500kbs scan config\r\n");
    uint16_t ret = sonata_ble_config_scanning(SONATA_GAP_STATIC_ADDR);
    //Next event:SONATA_GAP_CMP_SCANNING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief config initiating
 */
void app_ble_config_initiating()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //Call api to config init
    uint16_t ret = sonata_ble_config_initiating(SONATA_GAP_STATIC_ADDR);
    //Next event:SONATA_GAP_CMP_INITIATING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief set advertising data
 */
static void app_ble_set_adv_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t advData[] = {//Advertising data format
            8, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'B', 'L', 'E', 0
    };
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA

    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}
/*!
 * @brief Start advertising
 */
static void app_ble_start_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint16_t ret = sonata_ble_start_advertising(0, 0);
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief Start scanning
 */
static void app_ble_start_scanning()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    bFound = false;
    sonata_gap_scan_param_t param = {0};
    param.type = SONATA_GAP_SCAN_TYPE_GEN_DISC;
    // For continuous scan, use OBSERVER type, use duration to control scan timeer.
    // if duration=0, will scan for ever until sonata_ble_stop_scanning() called
    //param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;
    param.prop = SONATA_GAP_SCAN_PROP_ACTIVE_1M_BIT | SONATA_GAP_SCAN_PROP_PHY_1M_BIT;//0x05
    if (DEMO_LONG_RANGE)
    {
        APP_TRC("APP: %s  >>>>>>>>>>>>>>>>>>>>>>>>>>>>Long Range\r\n", __FUNCTION__);
        param.prop = SONATA_GAP_SCAN_PROP_PHY_CODED_BIT | SONATA_GAP_SCAN_PROP_ACTIVE_CODED_BIT;
    }
    param.dup_filt_pol = SONATA_GAP_DUP_FILT_EN;
    param.scan_param_1m.scan_intv = 0x0140;
    param.scan_param_1m.scan_wd = 0x00A0;
    param.scan_param_coded.scan_intv = 0x0140;
    param.scan_param_coded.scan_wd = 0x00A0;
    param.duration = 0;
    param.period = 0;
    uint16_t ret = sonata_ble_start_scanning(&param);
    //Scan result will show in app_gap_scan_result_callback()
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }

}
/*!
 * @brief Check MAC address
 * @param address
 * @return
 */
static bool app_ble_check_address(uint8_t *address)
{
    uint8_t error_address[SONATA_GAP_BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (address == NULL)
    {
        return false;
    }
    if (memcmp(address, error_address, SONATA_GAP_BD_ADDR_LEN) == 0)
    {
        return false;
    }
    return true;


}

/*!
 * @brief Start initiating
 */
static void app_ble_start_initiating(uint8_t *target)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    if (app_ble_check_address(target) == false)
    {
        APP_TRC("APP: %s, Target address is not right. Stop\r\n", __FUNCTION__);
        return;
    }
    sonata_gap_init_param_t param = {0};
    param.type = SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST;
    param.prop = SONATA_GAP_INIT_PROP_1M_BIT | SONATA_GAP_INIT_PROP_2M_BIT | SONATA_GAP_INIT_PROP_CODED_BIT;
    param.conn_to = 0;
    param.peer_addr.addr_type = SONATA_GAP_STATIC_ADDR; //  Addr
    memcpy(param.peer_addr.addr.addr, target, SONATA_GAP_BD_ADDR_LEN);

    if (param.prop & SONATA_GAP_INIT_PROP_1M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_1M_BIT \r\n", __FUNCTION__, param.prop);
        param.scan_param_1m.scan_intv = 0x0200;
        param.scan_param_1m.scan_wd = 0x0100;
        param.conn_param_1m.conn_intv_min = 0x0028;
        param.conn_param_1m.conn_intv_max = 0x0028;
        param.conn_param_1m.conn_latency = 0;
        param.conn_param_1m.supervision_to = 0x012C;
        param.conn_param_1m.ce_len_min = 0x0008;
        param.conn_param_1m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_2M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_2M_BIT \r\n", __FUNCTION__, param.prop);

        param.conn_param_2m.conn_intv_min = 0x0028;
        param.conn_param_2m.conn_intv_max = 0x0028;
        param.conn_param_2m.conn_latency = 0;
        param.conn_param_2m.supervision_to = 0x012C;
        param.conn_param_2m.ce_len_min = 0x0008;
        param.conn_param_2m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_CODED_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_CODED_BIT \r\n", __FUNCTION__, param.prop);
        param.scan_param_coded.scan_intv = 0x0200;
        param.scan_param_coded.scan_wd = 0x0100;
        param.conn_param_coded.conn_intv_min = 0x0028;
        param.conn_param_coded.conn_intv_max = 0x0028;
        param.conn_param_coded.conn_latency = 0;
        param.conn_param_coded.supervision_to = 0x012C;
        param.conn_param_coded.ce_len_min = 0x0008;
        param.conn_param_coded.ce_len_max = 0x0008;
    }

    uint16_t ret = sonata_ble_start_initiating(&param);
    //Next event:If connected, SONATA_GAP_CMP_INITIATING_DELETE event will be received
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


/*!
 * @brief BlE ON
 */
static void app_ble_on()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_gap_set_dev_config_cmd cmd = {0};
    cmd.role = SONATA_GAP_ROLE_ALL;
    cmd.gap_start_hdl = 0;
    cmd.gatt_start_hdl = 0;
    cmd.renew_dur = 0x0096;
    cmd.privacy_cfg = 0;
    cmd.pairing_mode = SONATA_GAP_PAIRING_SEC_CON | SONATA_GAP_PAIRING_LEGACY;
    cmd.att_cfg = 0x0080;
    cmd.max_mtu = 0x02A0;
    cmd.max_mps = 0x02A0;
    cmd.max_nb_lecb = 0x0A;
    cmd.hl_trans_dbg = false;
    uint16_t ret = sonata_ble_on(&cmd);//Next event:SONATA_GAP_CMP_BLE_ON
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    app_hal_init();
}

//static void app_disc_all_svc(){
//    sonata_ble_gatt_disc_all_svc(0);//app_disc_all_svc_complete_callback //app_gatt_disc_svc_callback
//}

/*!
 * @brief GATT Read
 * @param connection_id
 */
static void app_gatt_read(uint8_t connection_id)
{
    sonata_ble_gatt_read_by_handle(connection_id, demo_handle_id);//app_disc_read_complete_callback //app_gatt_read_callback
}


/*
 * LOCAL FUNCTION DEFINITIONS    Callback functions
 ****************************************************************************************
 */

static uint8_t gatt_read_cnt = 0;

//struct gap_bdaddr gap_addr[1];
//uint8_t white_addr[6]={0x11, 0x11, 0x00, 0x00, 0x11, 0x11};
/*!
 * @brief BLE complete event handler
 * @param opt_id @see sonata_ble_complete_type
 * @param status complete status
 * @param param param and deparam will be difference according to difference complete event.
 * @param dwparam param and deparam will be difference according to difference complete event.
 * @return
 */
static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    APP_TRC("APP_COMPLETE: %s  opt_id=%04X,status=%02X,param=%04X,dwparam=%lu\r\n", __FUNCTION__, opt_id, status, param, dwparam);
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01

            if (DEMO_SCAN)
            {
                app_ble_config_scanning();
            }
            if (DEMO_INIT)
            {
                app_ble_config_initiating();
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            app_ble_set_adv_data();
            break;
        case SONATA_GAP_CMP_SCANNING_CONFIG ://0x0F03
            app_ble_start_scanning();
            break;
        case SONATA_GAP_CMP_INITIATING_CONFIG ://0x0F04
            if (gConnSteps == CONN_TARGET_1)
            {
                APP_TRC("APP: %s  start connect target1:%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__,
                        targetAddr1[0],targetAddr1[1],targetAddr1[2],targetAddr1[3],targetAddr1[4],targetAddr1[5]);
                app_ble_start_initiating(targetAddr1);
            }
            else if (gConnSteps == CONN_TARGET_2)
            {
                APP_TRC("APP: %s  start connect target2\r\n", __FUNCTION__);
                app_ble_start_initiating(targetAddr2);
            }
            else if (gConnSteps == CONN_TARGET_3)
            {
                app_ble_start_initiating(targetAddr3);
            }
            else if (gConnSteps == CONN_TARGET_4)
            {
                app_ble_start_initiating(targetAddr4);
            }
            break;

        case SONATA_GAP_CMP_SCANNING_DELETE ://0x0F0F
            #if DEMO_SCAN_AND_CONNECT
            app_ble_config_initiating();
            #endif

            break;
        case SONATA_GAP_CMP_INITIATING_DELETE ://0x0F10
            APP_TRC("APP: %s  SONATA_GAP_CMP_INITIATING_DELETE,gConnSteps=%d\r\n", __FUNCTION__, gConnSteps);
            #if DEMO_DISC || DEMO_SCAN_AND_CONNECT
            sonata_ble_gatt_disc_all_characteristic(gConnSteps, 1, 0XFFFF);
            sonata_ble_gatt_disc_all_descriptor(gConnSteps, 1, 0XFFFF);
            #else
            //Connect with target, get peer devcies's name, name will show in app_gap_peer_att_info_callback()->case SONATA_GAP_DEV_NAME
            sonata_ble_gap_get_peer_info(gConnSteps, SONATA_GET_PEER_NAME);
            //Start to connect next target
            if (gConnSteps == CONN_TARGET_2)
            {
                gConnSteps = CONN_TARGET_5;//Skip target3 and target4.For quick debug
            }
            else
            {
                gConnSteps++;
                app_ble_config_initiating();
            }
            #endif
            break;
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
            app_ble_start_advertising();
            break;
        case SONATA_GATT_CMP_DISC_ALL_SVC://0x0402
        {
            uint8_t connection_id = param;
            app_gatt_read(connection_id);
            break;
        }
        case SONATA_GATT_CMP_READ ://0x0408
            gatt_read_cnt++;
            if (gatt_read_cnt < 10)
            {
                sonata_ble_gatt_read_by_handle(param, demo_handle_id);
            }
            break;
        case SONATA_GAP_CMP_RESET:
            APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_RESET, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
            ble_reset_cmp();
            break;
        default:
            break;
    }

    return CB_DONE;
}


/*!
 * @brief GAP scan result callback
 * @param result
 */
static uint16_t app_gap_scan_result_callback(sonata_gap_ext_adv_report_ind_t *result)
{
    APP_TRC("APP_CB: %s  ", __FUNCTION__);
    APP_TRC("target_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
    {
        APP_TRC("%02X ", result->target_addr.addr.addr[i]);
    }
    APP_TRC("  trans_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
    {
        APP_TRC("%02X ", result->trans_addr.addr.addr[i]);
    }
    if (memcmp(result->trans_addr.addr.addr, targetAddr1, SONATA_GAP_BD_ADDR_LEN) == 0)
    {
        toggle_led();
    }

    APP_TRC(" \r\n");
    #if DEMO_SCAN_AND_CONNECT
    if (bFound == false)
    {
        if (memcmp(result->trans_addr.addr.addr, targetAddr1, SONATA_GAP_BD_ADDR_LEN) == 0)
        {
            bFound = true;
            sonata_ble_stop_scanning();
            //If app adds gap_active_stopped() callback, SDK will callback when active stopped.App can restart or delete it.
            //If app not adds gap_active_stopped() callback,SDK will stop active and then delete it.
        }
    }

    #endif
    return CB_DONE;
}


/*!
 * @brief Deal with peer device get local information request.
 * @param opt @see asr_gap_dev_info
 */
static uint16_t app_gap_peer_get_local_info_callback(uint8_t conidx, sonata_gap_dev_info opt)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    switch (opt)
    {
        case SONATA_GAP_DEV_NAME:
        {
            uint8_t dev_name[] = {'A', 'S', 'R', 0};
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_name(conidx, 3, dev_name);
        }
            break;

        case SONATA_GAP_DEV_APPEARANCE:
        {
            uint16_t appearance = 0;
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_appearance(conidx, appearance);

        }
            break;

        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
        {
            sonata_ble_gap_send_get_dev_info_cfm_for_slv_pref_params(conidx, 8, 10, 0, 200);

        }
            break;

        default:
            break;
    }

    return CB_DONE;
}

/*!
 * @brief Disconnect indicate
 * @param conidx
 * @param conhdl
 * @param reason
 * @return
 */
static uint16_t app_gap_disconnect_ind_callback(uint8_t conidx, uint16_t conhdl, uint8_t reason)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    if(DEMO_SCAN_AND_CONNECT)
    {
        turn_off_led();
    }
    return CB_DONE;
}

static uint16_t app_gap_connection_req_callback(uint8_t conidx, sonata_gap_connection_req_ind_t *req)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    APP_TRC("peer_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i) { APP_TRC("%02X ", req->peer_addr.addr[i]); }
    APP_TRC(" \r\n");
    if(DEMO_SCAN_AND_CONNECT)
    {
        turn_on_led();
    }
    return CB_REJECT; //SDK will send connection confirm message
}


/*!
 * @brief Callback for get peer devices att information,
 * @param info
 */
static uint16_t app_gap_peer_att_info_callback(uint8_t conidx, sonata_gap_peer_att_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    sonata_gap_peer_att_info_ind_t *att_info = (sonata_gap_peer_att_info_ind_t *) info;
    APP_TRC("APP_CB: %s, req =0x%02X\r\n", __FUNCTION__, att_info->req);
    APP_TRC("APP_CB: %s, handle =0x%02X\r\n", __FUNCTION__, att_info->handle);
    switch (att_info->req)
    {
        case SONATA_GAP_DEV_NAME:
        {
            APP_TRC("APP  %s, Name[%d]:", __FUNCTION__, conidx);
            for (int i = 0; i < att_info->info.name.length; ++i)
            {
                APP_TRC("%c", att_info->info.name.value[i]);
            }
            APP_TRC("\r\n");
            if (gConnSteps == CONN_TARGET_5)
            {
                app_ble_config_legacy_advertising();
            }
        }
            break;
        case SONATA_GAP_DEV_APPEARANCE:
        {
            APP_TRC("APP_CB: %s, appearance =0x%04X\r\n", __FUNCTION__, att_info->info.appearance);
        }
            break;
        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
        {
            APP_TRC("APP_CB: %s, con_intv_min =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_min);
            APP_TRC("APP_CB: %s, con_intv_max =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_max);
            APP_TRC("APP_CB: %s, slave_latency =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.slave_latency);
            APP_TRC("APP_CB: %s, conn_timeout =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.conn_timeout);
        }
            break;
        case SONATA_GAP_DEV_CTL_ADDR_RESOL:
        {
            APP_TRC("APP_CB: %s, ctl_addr_resol =0x%02X\r\n", __FUNCTION__, att_info->info.ctl_addr_resol);
        }
            break;

        default:
            break;

    }
    return CB_DONE;
}

/*!
 * @brief Callback for get peer devices information,
 * @param conidx
 * @param info
 * @return
 */
static uint16_t app_gap_peer_info_callback(uint8_t conidx, sonata_gap_peer_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X, info_type=%02X\r\n", __FUNCTION__, conidx, info->req);
    switch (info->req)
    {
        case SONATA_GET_PEER_VERSION:
            APP_TRC("APP_CB: SONATA_GET_PEER_VERSION, compid:%04X,lmp_subvers:%04X,lmp_vers:%02X,\r\n",
                    info->info.version.compid, info->info.version.lmp_subvers, info->info.version.lmp_vers);
            break;
        case SONATA_GET_PEER_FEATURES:
            APP_TRC("APP_CB: SONATA_GET_PEER_FEATURES, features:");
            for (int i = 0; i < SONATA_GAP_LE_FEATS_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.features.features[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_PEER_CON_RSSI:
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_RSSI, rssi:%04X\r\n", info->info.rssi.rssi);

            break;
        case SONATA_GET_PEER_CON_CHANNEL_MAP:
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_CHANNEL_MAP, map:");
            for (int i = 0; i < SONATA_GAP_LE_CHNL_MAP_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.channel_map.ch_map.map[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_LE_PING_TO:
            APP_TRC("APP_CB: SONATA_GET_LE_PING_TO, timeout:%04X,\r\n", info->info.ping_to_value.timeout);
            break;
        case SONATA_GET_PHY:
            APP_TRC("APP_CB: SONATA_GET_PHY, tx_phy:%02X, rx_phy:%02X\r\n", info->info.le_phy.tx_phy, info->info.le_phy.rx_phy);
            break;
        case SONATA_GET_CHAN_SEL_ALGO:
            APP_TRC("APP_CB: SONATA_GET_CHAN_SEL_ALGO, chan_sel_algo:%04X,\r\n", info->info.sel_algo.chan_sel_algo);
            break;
        default:
            break;
    }

    return CB_DONE;
}


/*!
 * @brief Discover service callback
 * @param connection_id
 * @param start_hdl
 * @param end_hdl
 * @param uuid_len
 * @param uuid
 */
static uint16_t app_gatt_disc_svc_callback(uint8_t connection_id, uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, start_hdl=0x%04X, end_hdl =0x%04X, uuid=", __FUNCTION__, start_hdl, end_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief Discover characteristic callback
 * @param conidx
 * @param attr_hdl
 * @param pointer_hdl
 * @param prop
 * @param uuid_len
 * @param uuid
 * @return
 */
static uint8_t flag=1;
static uint16_t app_gatt_disc_char_callback(uint8_t conidx, uint16_t attr_hdl, uint16_t pointer_hdl, uint8_t prop, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    uint8_t enable_notify=1;
    if(flag)
    {
        sonata_ble_gatt_write(0,0x0026,0,0,1,&enable_notify);
        flag=0;
    }
    return CB_DONE;
}

/*!
 * @brief Discover description callback
 * @param conidx
 * @param attr_hdl
 * @param uuid_len
 * @param uuid
 * @return
 */
static uint16_t app_gatt_disc_desc_callback(uint8_t conidx, uint16_t attr_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 */
static uint16_t app_gatt_read_callback(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    APP_TRC("APP_CB: %s, handle=0x%04X, offset=0x%04X, VALUE=", __FUNCTION__,
            handle, offset);
    for (int i = 0; i < length; ++i)
    {
        APP_TRC("%02x", value[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief Mtu changed
 * @param connection_id
 * @param mtu
 * @return
 */
static uint16_t app_gatt_mtu_changed_callback(uint8_t connection_id, uint16_t mtu)
{
    APP_TRC("APP_CB: %s, mtu=0x%04X\r\n", __FUNCTION__, mtu);
    return CB_DONE;
}
static uint16_t count =0;
uint16_t gatt_event_callback(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value)
{
    count++;
    if(count%100==0)
    {
        printf("*\r\n");
    }
    return CB_DONE;
}
/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{
#if DEMO_PRF_MESH
    sonata_mesh_api_init();
#endif
#if DEMO_PRF_DISS
    sonata_prf_diss_init();
#endif
#if DEMO_PRF_BASS
    sonata_prf_bass_init();
#endif
}


/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

sonata_ble_hook_t app_hook =
{
    assert_err,
    assert_param,
    assert_warn,
    app_init,
    platform_reset,
    get_stack_usage,
    __wrap_printf,
    app_prf_api_init,
#ifdef SONATA_RTOS_SUPPORT
    (void *)lega_rtos_init_semaphore,
    (void *)lega_rtos_get_semaphore,
    (void *)lega_rtos_set_semaphore,
#endif
};

static ble_gap_callback ble_gap_callbacks =
{
    /*************** GAP Manager's callback ***************/
    //Must if use scan function, peer's information will show in this callback
    .gap_scan_result                    = app_gap_scan_result_callback,
    /*************** GAP Controller's callback  ***************/
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_info                  = app_gap_peer_info_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_att_info              = app_gap_peer_att_info_callback,
    //Optional, if peer device get local device's information, app can deal with it in this callback
    .gap_peer_get_local_info            = app_gap_peer_get_local_info_callback,
    //Optional
    .gap_disconnect_ind                 = app_gap_disconnect_ind_callback,
    .gap_connection_req                 = app_gap_connection_req_callback,
};

static ble_gatt_callback ble_gatt_callbacks =
{
    //Must if use discovery all servcie function
    .gatt_disc_svc                      = app_gatt_disc_svc_callback,
    //Must if use discovery all characteristic function
    .gatt_disc_char                     = app_gatt_disc_char_callback,
    //Must if use discovery all description function
    .gatt_disc_char_desc                = app_gatt_disc_desc_callback,
    //Must if use read attribute function
    .gatt_read                          = app_gatt_read_callback,
    //Optional, add this callback if app need to save changed mtu value
    .gatt_mtu_changed                   = app_gatt_mtu_changed_callback,
    .gatt_event                         = gatt_event_callback,
};

static ble_complete_callback ble_complete_callbacks =
{
    //Must, app can do next operation in this callback
    .ble_complete_event                 = app_ble_complete_event_handler,
};


static const sonata_api_app_msg_t myMsgs[] = {
        {APP_MSG_UART_CMD,    app_uart_cmd_handler},

};
/*!
 * @brief Init
 */
void app_init(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_log_level_set(SONATA_LOG_ERROR);

    sonata_ble_register_gap_callback(&ble_gap_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);

    app_ble_on();
    int size = sizeof(myMsgs) / sizeof(sonata_api_app_msg_t);
    for (int i = 0; i < size; ++i)
    {
        sonata_api_app_msg_register(&myMsgs[i]);
    }
    // at command init

}



/// @} APP
