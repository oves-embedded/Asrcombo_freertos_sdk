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
#include "arch.h"                   // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#ifndef ALIOS_SUPPORT
#include "user_platform.h"
#endif
#include "sonata_ble_hook.h"
//#include "sonata_gap_api_int.h"
#include "sonata_gatt_api.h"

#include "sonata_log.h"
#include "lega_rtos_api.h"

#include "app.h"
#if BLE_BATT_SERVER
#include "sonata_prf_bass_api.h"
#endif //BLE_BATT_SERVER

/*
 * DEFINES
 ****************************************************************************************
 */
#define APP_BLE_ADV_ON   1
#define APP_BLE_ADV_OFF  0
/*!
 * @brief save local handle start index
 */
//Mark for profile dis
uint8_t ble_adv_set_state = APP_BLE_ADV_OFF;
uint8_t app_connected_state = APP_STATE_DISCONNECTED;
uint8_t target_address[SONATA_GAP_BD_ADDR_LEN] = {0x9F, 0xEA, 0xA3, 0xDC, 0x59, 0x8C};//{0x8C, 0x59, 0xDC, 0xA3, 0xEA, 0x9F};
uint8_t ble_connect_id = 0xFF;
uint16_t read_handle = 0xFF;
uint16_t write_handle = 0xFF;
uint8_t write_uuid[16] = {0x02,0xB0,0x0};
uint8_t read_uuid[16] = {0xF0,0x80,0x0};

bool bFound = false;

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * EXTERNAL FUNCTION DECLARATION
 ****************************************************************************************
 */
extern int __wrap_printf(const char *format, ...);
static uint8_t app_timer_handler(uint16_t id);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static sonata_app_timer_callback_t app_timer_callbacks = {
        .timeout                  = app_timer_handler,
};


/*!
 * @brief config legacy advertising
 */
void app_ble_config_legacy_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_ADV_PROP_UNDIR_CONN_MASK;
    //param.max_tx_pwr = 0xE2;
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    //    msg->adv_param.adv_param.peer_addr.addr.addr:00
    param.addr_type = SONATA_GAP_STATIC_ADDR;
    param.adv_intv_min = 0x01E0;
    param.adv_intv_max = 0x01E0;
    param.chnl_map = 0x07;
    param.phy = SONATA_GAP_PHY_LE_1MBPS;
    uint16_t ret = sonata_ble_config_legacy_advertising(SONATA_GAP_STATIC_ADDR, &param);//Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

#if 0
/*!
 * @brief set advertising data
 */
static uint16_t app_ble_set_adv_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t advData[] = { //Advertising data format
            8, GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'D', 'T', 'S', 0
    };
    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    return ret;
}

static uint16_t app_ble_set_scansponse_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t advData[] = { //Advertising data format
            8, GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'R', 'S', 'P', 0
    };

    //Call API
    uint16_t ret = sonata_ble_set_scan_response_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    return ret;
}
#endif

uint16_t app_ble_start_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //Call api
    uint16_t ret = sonata_ble_start_advertising(0, 0);
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    return ret;

}

uint16_t app_ble_advertising_stop()
{
    ble_adv_set_state = APP_BLE_ADV_OFF;
    //Call api
    uint16_t ret = sonata_ble_stop_advertising();
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    return ret;
}


uint16_t app_ble_stop_adv()
{
    APP_TRC("APP: %s   \r\n", __FUNCTION__);
    ble_adv_set_state = APP_BLE_ADV_OFF;

    //Call api

    uint16_t ret = sonata_ble_stop_advertising();
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    return ret;
}



/*!
 * @brief Config scanning
 */
void app_ble_config_scanning()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint16_t ret = sonata_ble_config_scanning(SONATA_GAP_STATIC_ADDR);
    //Next event:SONATA_GAP_CMP_SCANNING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


/*!
 * @brief Start scanning
 */
void app_ble_start_scanning(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    bFound = false;
    sonata_gap_scan_param_t param = {0};
    param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;
    // For continuous scan, use OBSERVER type, use duration to control scan timeer.
    // if duration=0, will scan for ever until sonata_ble_stop_scanning() called
    //param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;
    param.prop = SONATA_GAP_SCAN_PROP_PHY_CODED_BIT | SONATA_GAP_SCAN_PROP_ACTIVE_CODED_BIT;//0x05
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


void app_ble_stop_scanning(void)
{
    sonata_ble_stop_scanning();
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
void app_ble_start_initiating(uint8_t *target)
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
        param.conn_param_1m.supervision_to = 0x02BC;
        param.conn_param_1m.ce_len_min = 0x0008;
        param.conn_param_1m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_2M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_2M_BIT \r\n", __FUNCTION__, param.prop);

        param.conn_param_2m.conn_intv_min = 0x0028;
        param.conn_param_2m.conn_intv_max = 0x0028;
        param.conn_param_2m.conn_latency = 0;
        param.conn_param_2m.supervision_to = 0x02BC;
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
        param.conn_param_coded.supervision_to = 0x02BC;
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
 * @brief
 * @return
 */
void app_ble_stop_initiating(void)
{
    sonata_ble_stop_initiating();
}

void app_ble_disconnect(void)
{
    sonata_ble_gap_disconnect(ble_connect_id,0);
    ble_connect_id = 0xFF;
}

uint8_t app_get_connect_status(void)
{
    if(app_connected_state == APP_STATE_CONNECTED)
    {
         return 1;
    }
    return 0;
}


/*!
 * @brief
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
    cmd.max_mtu = 250;
    cmd.max_mps = 0x02A0;
    cmd.max_nb_lecb = 0x0A;
    cmd.hl_trans_dbg = false;
    uint16_t ret = sonata_ble_on(&cmd);//Next event:SONATA_GAP_CMP_BLE_ON
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}
#ifdef CFG_MRFOTA_TEST
extern void ble_reset_cmp(void);
#endif
/*
 * LOCAL FUNCTION DEFINITIONS    Callback functions
 ****************************************************************************************
 */

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
    uint16_t cb_result = CB_DONE;
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
            app_ble_config_scanning();
            break;
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            break;
        case SONATA_GAP_CMP_SCANNING_CONFIG ://0x0F03
            app_ble_start_scanning();
            break;
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
           break;
        case SONATA_GAP_CMP_SET_SCAN_RSP_DATA:
            break;
        case SONATA_GAP_CMP_ADVERTISING_START ://0x0F06
             APP_TRC("ble_adv   set state %d\r\n",ble_adv_set_state);
        break;
        case SONATA_GAP_CMP_ADVERTISING_STOP:
            break;
        case  SONATA_GAP_CMP_ADVERTISING_DELETE:
            APP_TRC("SONATA_GAP_CMP_ADVERTISING_DELETE!!!!\r\n");
            break;
        case SONATA_GAP_CMP_PROFILE_TASK_ADD://0x011B
            break;
            case SONATA_GAP_CMP_SCANNING_STOP://0x0F08
            cb_result = CB_REJECT; //delete scan instance
            break;
            case SONATA_GAP_CMP_SCANNING_DELETE ://0x0F0F
            app_ble_config_initiating();
            break;
            case SONATA_GAP_CMP_INITIATING_CONFIG ://0x0F04
            app_ble_start_initiating(target_address);
            break;
            case SONATA_GAP_CMP_INITIATING_STOP:
            cb_result = CB_REJECT; //delete scan instance
              break;
        case SONATA_GAP_CMP_INITIATING_DELETE ://0x0F10
            sonata_ble_gatt_disc_all_characteristic(ble_connect_id, 1, 0XFFFF);
            //sonata_ble_gatt_disc_all_descriptor(ble_connect_id, 1, 0XFFFF);
            break;
        case SONATA_GATT_CMP_NOTIFY:
            APP_TRC("APP_COMPLETE: %s  SONATA_GATT_CMP_NOTIFY, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
            break;
        case SONATA_GATT_CMP_DISC_ALL_SVC://0x0402
            //sonata_ble_gatt_read_by_handle(param, demo_handle_id);
            break;
        case SONATA_GATT_CMP_READ ://0x0408
            break;
        case SONATA_GAP_CMP_RESET:
            //APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_RESET, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
#ifdef CFG_MRFOTA_TEST
            ble_reset_cmp();
#endif
            break;
        default:
            break;
    }

    return cb_result;
}

static uint16_t app_ble_rsp_event_handler(uint16_t opt_id, uint8_t status, uint16_t handle, uint16_t perm, uint16_t ext_perm, uint16_t length, void *param)
{
    APP_TRC("APP_RESPONSE: %s  opt_id=%04X,\r\n", __FUNCTION__, opt_id);
    if (status != 0)
    {
        APP_TRC("APP_RESPONSE: %s  ERROR=%04X,\r\n", __FUNCTION__, status);
    }
    switch (opt_id)
    {
        case SONATA_GATT_ADD_SVC_RSP:
        {
            APP_TRC("APP_RESPONSE: %s  handle=%04X,\r\n", __FUNCTION__, handle);
            //Should save the start handle id for future use
            break;
        }
        default:
            break;
    }

    return CB_DONE;
}


/*!
 * @brief get devcie informations
 * @param info_type @see sonata_gap_local_dev_info
 * @param info
 */
static uint16_t app_get_dev_info_callback(sonata_gap_local_dev_info info_type, void *info)
{
    switch (info_type)
    {
        case SONATA_GET_DEV_VERSION:
        {
            #if APP_DBG
            sonata_gap_dev_version_ind_t *dev_info = (sonata_gap_dev_version_ind_t *) info;
            APP_TRC("APP_CB: %s, hci_ver =0x%02X\r\n", __FUNCTION__, dev_info->hci_ver);
            APP_TRC("APP_CB: %s, lmp_ver =0x%02X\r\n", __FUNCTION__, dev_info->lmp_ver);
            APP_TRC("APP_CB: %s, host_ver =0x%02X\r\n", __FUNCTION__, dev_info->host_ver);
            APP_TRC("APP_CB: %s, hci_subver =0x%04X\r\n", __FUNCTION__, dev_info->hci_subver);
            APP_TRC("APP_CB: %s, lmp_subver =0x%04X\r\n", __FUNCTION__, dev_info->lmp_subver);
            APP_TRC("APP_CB: %s, host_subver =0x%04X\r\n", __FUNCTION__, dev_info->host_subver);
            APP_TRC("APP_CB: %s, manuf_name =0x%04X\r\n", __FUNCTION__, dev_info->manuf_name);
            #endif //SONATA_API_TASK_DBG
        }
            break;
        case SONATA_GET_DEV_BDADDR:
        {
            #if APP_DBG
            sonata_gap_dev_bdaddr_ind_t *param = (sonata_gap_dev_bdaddr_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_DEV_BDADDR:", __FUNCTION__);
            for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
            { APP_TRC("%02X ", param->addr.addr.addr[i]); }
            APP_TRC("\r\n");
            #endif //SONATA_API_TASK_DBG
        }
            break;

        case SONATA_GET_DEV_ADV_TX_POWER:
        {
            #if APP_DBG
            sonata_gap_dev_adv_tx_power_ind_t *param = (sonata_gap_dev_adv_tx_power_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_DEV_ADV_TX_POWER power_lvl =0x%02X\r\n", __FUNCTION__, param->power_lvl);
            #endif //SONATA_API_TASK_DBG
        }
            break;
        case SONATA_GET_WLIST_SIZE:
        {
            #if APP_DBG
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_WLIST_SIZE size =0x%02X\r\n", __FUNCTION__, param->size);
            #endif //SONATA_API_TASK_DBG

            break;
        }
        case SONATA_GET_ANTENNA_INFO:
        {
            #if APP_DBG
            sonata_gap_antenna_inf_ind_t *param = (sonata_gap_antenna_inf_ind_t *) info;
            APP_TRC(">>> SONATA_GET_ANTENNA_INFO supp_switching_sampl_rates =0x%02X, antennae_num =0x%02X, max_switching_pattern_len =0x%02X, max_cte_len =0x%02X\r\n",
                    param->supp_switching_sampl_rates, param->antennae_num, param->max_switching_pattern_len, param->max_cte_len);
            #endif //SONATA_API_TASK_DBG
        }
            break;

        case SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_sugg_dflt_data_len_ind_t *param = (sonata_gap_sugg_dflt_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN suggted_max_tx_octets =0x%02X, suggted_max_tx_time =0x%02X\r\n",
                    param->suggted_max_tx_octets, param->suggted_max_tx_time);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_MAX_LE_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_max_data_len_ind_t *param = (sonata_gap_max_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_MAX_LE_DATA_LEN suppted_max_tx_octets =0x%04X, suppted_max_tx_time =0x%04X, suppted_max_rx_octets =0x%04X, suppted_max_rx_time =0x%04X\r\n",
                    param->suppted_max_tx_octets, param->suppted_max_tx_time, param->suppted_max_rx_octets, param->suppted_max_rx_time);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_PAL_SIZE:
        {
            #if APP_DBG
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_PAL_SIZE size =0x%02X\r\n", __FUNCTION__, param->size);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_RAL_SIZE:
        {
            #if APP_DBG
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_RAL_SIZE size =0x%02X\r\n", __FUNCTION__, param->size);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_NB_ADV_SETS:
        {
            #if APP_DBG
            sonata_gap_nb_adv_sets_ind_t *param = (sonata_gap_nb_adv_sets_ind_t *) info;
            APP_TRC("APP_CB: %s, SONATA_GET_NB_ADV_SETS nb_adv_sets =0x%02X\r\n", __FUNCTION__, param->nb_adv_sets);
            #endif //SONATA_API_TASK_DBG

            break;
        }
        case SONATA_GET_MAX_LE_ADV_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_max_adv_data_len_ind_t *param = (sonata_gap_max_adv_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_MAX_LE_ADV_DATA_LEN param->length=0x%02X\r\n", param->length);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_DEV_TX_PWR:
        {
            #if APP_DBG
            sonata_gap_dev_tx_pwr_ind_t *param = (sonata_gap_dev_tx_pwr_ind_t *) info;
            APP_TRC(">>> SONATA_GET_DEV_TX_PWR min_tx_pwr =0x%04X, max_tx_pwr =0x%04X\r\n",
                    param->min_tx_pwr, param->max_tx_pwr);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_DEV_RF_PATH_COMP:
        {
            #if APP_DBG
            sonata_gap_dev_rf_path_comp_ind_t *param = (sonata_gap_dev_rf_path_comp_ind_t *) info;
            APP_TRC(">>> SONATA_GET_DEV_RF_PATH_COMP tx_path_comp =0x%04X, rx_path_comp =0x%04X\r\n",
                    param->tx_path_comp, param->rx_path_comp);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        default:
            APP_TRC("APP_CB: %s  No progress for info_type=%02X\r\n", __FUNCTION__, info_type);
            break;

    }
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
            uint8_t dev_name[] = "msm_ble_demo";
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_name(conidx, sizeof(dev_name), dev_name);
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
    APP_TRC(" \r\n");
    if (memcmp(result->trans_addr.addr.addr, target_address, SONATA_GAP_BD_ADDR_LEN) == 0)
    {
        sonata_ble_stop_scanning();
        //If app adds gap_active_stopped() callback, SDK will callback when active stopped.App can restart or delete it.
        //If app not adds gap_active_stopped() callback,SDK will stop active and then delete it.
    }
    return CB_DONE;
}


/*!
 * @brief
 * @param conidx
 * @param conhdl
 * @param reason
 * @return
 */
static uint16_t app_gap_disconnect_ind_callback(uint8_t conidx, uint16_t conhdl, uint8_t reason)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    app_connected_state = APP_STATE_DISCONNECTED;
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
static uint16_t app_gatt_disc_char_callback(uint8_t conidx, uint16_t attr_hdl, uint16_t pointer_hdl, uint8_t prop, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    if(!memcmp(uuid,write_uuid,uuid_len))
    {
        write_handle = attr_hdl;
        APP_TRC("write_handle %d\r\n",write_handle);
    }
    if(!memcmp(uuid,read_uuid,uuid_len))
    {
        read_handle = attr_hdl;
        APP_TRC("read_handle %d\r\n",read_handle);
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
#if 0
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
#endif

/*!
 * @brief
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
            APP_TRC("APP  %s, Name:", __FUNCTION__);
            for (int i = 0; i < att_info->info.name.length; ++i)
            {
                APP_TRC("%c", att_info->info.name.value[i]);
            }
            APP_TRC("\r\n");

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
 * @brief
 * @param conidx
 * @param intv_min
 * @param intv_max
 * @param latency
 * @param time_out
 */
static uint16_t app_gap_param_update_req_callback(uint8_t conidx, uint16_t intv_min, uint16_t intv_max, uint16_t latency, uint16_t time_out)
{
    APP_TRC("APP_CB: %s  conidx=%02X,intv_min=%04X,intv_max=%04X,latency=%04X,time_out=%04X\r\n", __FUNCTION__, conidx, intv_min, intv_max, latency, time_out);
    sonata_ble_gap_send_param_update_cfm(conidx, true, 2, 4);
    return CB_DONE;
}

/*!
 * @brief
 * @param conidx
 * @param con_interval
 * @param con_latency
 * @param sup_to
 * @return
 */
static uint16_t app_gap_param_updated_callback(uint8_t conidx, uint16_t con_interval, uint16_t con_latency, uint16_t sup_to)
{
    APP_TRC("APP_CB: %s  conidx=%02X,con_interval=%04X,con_latency=%04X,sup_to=%04X\r\n", __FUNCTION__, conidx, con_interval, con_latency, sup_to);

    return CB_DONE;

}

/*!
 * @brief
 * @param conidx
 * @param length
 * @param name
 * @return
 */
static uint16_t app_gap_peer_set_local_device_name_callback(uint8_t conidx, uint16_t length, uint8_t *name)
{
    sonata_ble_gap_send_set_dev_name_cfm(conidx, SONATA_HL_GAP_ERR_REJECTED);
    return CB_DONE;
}


static uint16_t app_gap_connection_req_callback(uint8_t conidx,sonata_gap_connection_req_ind_t *req)
{
    APP_TRC("APP_CB: %s  ", __FUNCTION__);
    APP_TRC("peer_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i) { APP_TRC("%02X ", req->peer_addr.addr[i]); }
    APP_TRC(" \r\n");
    if(!memcmp(req->peer_addr.addr, target_address,SONATA_GAP_BD_ADDR_LEN))
    {

       ble_connect_id = conidx;
       APP_TRC("ble_connect_id %d \r\n",ble_connect_id);
    }
    sonata_ble_gatt_exchange_mtu(conidx);
    app_connected_state = APP_STATE_CONNECTED;
    return CB_REJECT; //SDK will send connection confirm message

}

/*!
 * @brief
 * @param connection_id
 * @param handle
 * @return
 */
static uint16_t app_gatt_read_request_callback(uint8_t connection_id, uint16_t handle)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=0x%04X,\r\n", __FUNCTION__, handle);
    uint16_t length = 0;
    uint8_t *value = NULL;
    sonata_ble_gatt_send_read_confirm(connection_id, handle, SONATA_HL_ATT_ERR_NO_ERROR, length, value);
    return CB_REJECT;
}

/*!
 * @brief
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
static uint16_t app_gatt_write_request_callback(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=0x%04X\r\n", __FUNCTION__, handle);

    sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_NO_ERROR);
    return CB_DONE;
}

/*!
 * @brief
 * @param connection_id
 * @param mtu
 * @return
 */
static uint16_t app_gatt_mtu_changed_callback(uint8_t connection_id, uint16_t mtu)
{
    APP_TRC("APP_CB: %s, mtu=0x%04X\r\n", __FUNCTION__, mtu);
    return CB_DONE;
}


/*!
 * @brief
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
static uint16_t app_gatt_att_info_req_ind_callback(uint8_t connection_id, uint16_t handle)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=0x%04X\r\n", __FUNCTION__, handle);
    uint16_t length = 0;
    uint8_t  status;
    length = 200;
    status = SONATA_HL_ATT_ERR_NO_ERROR;
    sonata_ble_gatt_send_att_info_confirm(connection_id,handle,length,status);
    return CB_DONE;
}


/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{
#if BLE_DIS_SERVER
    sonata_prf_diss_init();
#endif
#if BLE_BATT_SERVER
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
#ifdef ALIOS_SUPPORT
    printf,
#else
    __wrap_printf,
#endif
    app_prf_api_init,
#ifdef SONATA_RTOS_SUPPORT
    (void *)lega_rtos_init_semaphore,
    (void *)lega_rtos_get_semaphore,
    (void *)lega_rtos_set_semaphore,
#endif

};
uint16_t gap_active_stopped_callback(uint8_t actv_idx, uint8_t type,uint8_t reason,uint8_t per_adv_stop)
{
    APP_TRC("APP_CB: %s, reason=0x%04X\r\n", __FUNCTION__,reason);
    if(SONATA_GAP_ACTV_TYPE_ADV == type)
    {

    }
    if(SONATA_GAP_ACTV_TYPE_SCAN == type)
    {
        return CB_REJECT; //delete scan instance
    }
    if(SONATA_GAP_ACTV_TYPE_INIT == type)
    {
        return CB_REJECT; //delete init instance
    }
    return CB_DONE;
}

static ble_gap_callback ble_gap_callbacks =
{
    /*************** GAP Manager's callback ***************/

    //Must if use scan function, peer's information will show in this callback
    .gap_scan_result                    = app_gap_scan_result_callback,
    //Optional, use for get local devcie informations when call sonata_ble_get_dev_info()
    .get_local_dev_info                 = app_get_dev_info_callback,

        /*************** GAP Controller's callback  ***************/
    //Optional
    .gap_param_update_req               = app_gap_param_update_req_callback,
    //Optional
    .gap_param_updated                  = app_gap_param_updated_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_info                  = app_gap_peer_info_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_att_info              = app_gap_peer_att_info_callback,
    //Optional, if peer device get local device's information, app can deal with it in this callback
    .gap_peer_get_local_info            = app_gap_peer_get_local_info_callback,
    //Optional
    .gap_disconnect_ind                 = app_gap_disconnect_ind_callback,
    //Optional, if peer device set local device's name, app can deal with it in this callback
    .gap_peer_set_local_device_name     = app_gap_peer_set_local_device_name_callback,
    //Optional, app can save peer mac address in this callback when connected
    .gap_connection_req                 = app_gap_connection_req_callback,
    .gap_active_stopped                 = gap_active_stopped_callback,

};

static ble_gatt_callback ble_gatt_callbacks =
{
    //Must if use discovery all servcie function
    .gatt_disc_svc                      = app_gatt_disc_svc_callback,
    //Must if use discovery all characteristic function
    .gatt_disc_char                     = app_gatt_disc_char_callback,
    //Must if use discovery all description function
    .gatt_disc_char_desc                = app_gatt_disc_desc_callback,
    //Optional, add this callback if app need to save changed mtu value
    .gatt_mtu_changed                   = app_gatt_mtu_changed_callback,
    //Must,If app add custom service, app should add this callback to deal with peer device read request
    .gatt_read_req                      = app_gatt_read_request_callback,
    //Must,If app add custom service, app should add this callback to deal with peer device write request
    .gatt_write_req                     = app_gatt_write_request_callback,

    .gatt_att_info_req                  = app_gatt_att_info_req_ind_callback,
};

static ble_complete_callback ble_complete_callbacks =
{
    //Must, app can do next operation in this callback
    .ble_complete_event                 = app_ble_complete_event_handler,
};

static ble_response_callback ble_rsp_callbacks =
{
    //Must,IF app add custom service, add should save this service's start handler id,
    //this id will be used in app_gatt_read_request_callback() and app_gatt_write_request_callback()
    .ble_rsp_event                      = app_ble_rsp_event_handler,
};


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

void app_init(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_log_level_set(SONATA_LOG_VERBOSE);

    sonata_ble_register_gap_callback(&ble_gap_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);
    sonata_ble_register_response_callback(&ble_rsp_callbacks);

    app_ble_on();

    sonata_api_register_app_timer_callback(&app_timer_callbacks);
}

uint16_t test_count = 0;
uint8_t  adv_value[32] = {0x02,0x01,0x06,0x06,0x09,0x6d,0x69,0x64,0x64,0x64,0x12,0xFF,0xA8,0x06,0x01 \
        ,0x31,0x38,0x32,0x37,0x33,0x36,0x34,0x35,0x46,0x43,0x30,0x30,0x30,0x34};
uint16_t adv_length = 29;
uint16_t test_interval = 0;
uint8_t  write_value[256] = {0x1,0x2,0x3,0x4,0x5};
uint8_t write_length = 0;

void app_ble_set_test_count(uint16_t counter)
{
    test_count = counter;
}

void app_ble_set_test_target(uint8_t * target)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    memcpy(target_address,target,SONATA_GAP_BD_ADDR_LEN);
    for(int i =0 ; i < SONATA_GAP_BD_ADDR_LEN; i++)
    {
         APP_TRC("%x ",target_address[i]);
    }
    APP_TRC("\r\n ");
}

void app_ble_set_test_write_uuid(uint8_t * uuid)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    memcpy(write_uuid,uuid,2);
    for(int i =0 ; i < 2; i++)
    {
         APP_TRC("%x ",write_uuid[i]);
    }
    APP_TRC("\r\n ");
}

void app_ble_set_test_read_uuid(uint8_t * uuid)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    memcpy(read_uuid,uuid,2);
    for(int i =0 ; i < 2; i++)
    {
         APP_TRC("%x ",read_uuid[i]);
    }
    APP_TRC("\r\n ");
}

void app_ble_set_test_interval(uint16_t interval)
{
    test_interval = interval;
}


void app_ble_set_write_param(uint8_t *data,  uint8_t length)
{
    memmove(write_value,data,length);
    write_length = length;
}


static uint8_t app_timer_handler(uint16_t id)
{
    APP_TRC("APP: %s  . id=%d\r\n", __FUNCTION__, id);


    if (id == 8)
    {
        static uint8_t scan_flag = 0;

        if(scan_flag == 0)
        {
            scan_flag = 1;
            app_ble_config_scanning();
            sonata_api_app_timer_set(8,100);
            sonata_api_app_timer_active(8);
        }
        else
        {
            app_ble_start_scanning();
            scan_flag = 0;
            sonata_api_app_timer_clear(8);
         }
    }

    if (id == 3)
    {
        app_ble_config_initiating();
    }

    if (id == 4)
    {
        sonata_ble_gatt_write(ble_connect_id, write_handle + 1,0,0,write_length,write_value);

        //sonata_ble_gatt_write_no_response(ble_connect_id, write_handle + 1,0,0,128,write_value);
        sonata_api_app_timer_set(4,test_interval);
        sonata_api_app_timer_active(4);
    }
    return 0;

}



/// @} APP
