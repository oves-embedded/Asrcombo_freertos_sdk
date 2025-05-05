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
#include "app_custom_svc.h"
#include "lega_rtos_api.h"
#include "sonata_private_protocol_api.h"


#if BLE_DIS_SERVER
#include "sonata_prf_diss_api.h"
#endif //BLE_BATT_SERVER

#ifdef CLOUD_JOYLINK_SUPPORT
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "lega_wlan_api.h"

#include "joylink_sdk.h"
#include "joylink_adapter.h"
#include "joylink_utils.h"
#include "joylink_config.h"
#include "joylink_extern_user.h"
#endif

#include "ft_2_4g_hal.h"
/*
 * DEFINES
 ****************************************************************************************
 */
#define DEMO_ADV            (0)
#define DEMO_PRF_DISS       (1)
#define APP_ADV_INTV_MIN    192//*0.625=120ms
#define APP_ADV_INTV_MAX    192//*0.625=120ms
#define APP_BLE_MODE         0
#define APP_2_4G_MODE        1

/*!
 * @brief save local handle start index
 */
static uint16_t custom_svc_start_handle = 0;
//Mark for profile dis
static bool prf_add = false;

//curent mode BLE or 2.4G
uint8_t stack_mode = APP_2_4G_MODE;

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
extern void ble_reset_cmp(void);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief config legacy advertising
 */
void app_ble_config_legacy_advertising()
{
    //APP_TRC("APP: %s  \r\n", __FUNCTION__);

    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0xE2;
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    //msg->adv_param.adv_param.peer_addr.addr.addr:00
    param.addr_type = 0;
    param.adv_intv_min = APP_ADV_INTV_MIN;
    param.adv_intv_max = APP_ADV_INTV_MAX;
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
 * @brief set advertising data
 */
static void ble_hex_to_char(unsigned char data, char *h, char *l)
{
    unsigned char tmp = data;
    char h_tmp, l_tmp;

    tmp &= 0xff;
    l_tmp = (((tmp & 0xf) > 9) ? ('A' + ((tmp & 0xf) - 10) ) :
                                ( '0' + (tmp & 0xf) ) );
    h_tmp = ((((tmp >> 4) & 0xf) > 9) ? ('A' + (((tmp >> 4) & 0xf) - 10) ) :
                                ( '0' + ((tmp >> 4) & 0xf) ) );
    *h = h_tmp;
    *l = l_tmp;
    return;
}

static void app_ble_set_adv_data()
{
    //APP_TRC("APP: %s  \r\n", __FUNCTION__);
    char bd_addr[12]={0};
    uint8_t  *bt_addr= sonata_get_bt_address();
    ble_hex_to_char(bt_addr[2], bd_addr+6, bd_addr+7);
    ble_hex_to_char(bt_addr[1], bd_addr+8, bd_addr+9);
    ble_hex_to_char(bt_addr[0], bd_addr+10, bd_addr+11);

#ifdef CLOUD_JOYLINK_SUPPORT
    //Advertising data format
    uint8_t advData[] =
    {
      9, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'J', 'Y', 'L', '-', 'B', 'L', 'E','_', //suggust name length is 9
    };
#else
    //Advertising data format
    uint8_t advData[] =
    {
      15, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'B', 'L', 'E','_',
      bd_addr[6],bd_addr[7],bd_addr[8],bd_addr[9],bd_addr[10],bd_addr[11]
    };
#endif
    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {

       //APP_TRC("APP: %s  len:%02X\r\n", __FUNCTION__, sizeof(advData));
    }
}


static void app_ble_set_adv_rsp_data()
{
    //APP_TRC("APP: %s  \r\n", __FUNCTION__);
    char bd_addr[12]={0};
    uint8_t  *bt_addr= sonata_get_bt_address();
    ble_hex_to_char(bt_addr[2], bd_addr+6, bd_addr+7);
    ble_hex_to_char(bt_addr[1], bd_addr+8, bd_addr+9);
    ble_hex_to_char(bt_addr[0], bd_addr+10, bd_addr+11);

#ifdef CLOUD_JOYLINK_SUPPORT
    jl_gap_data_t gap_data;

    memset(&gap_data, 0x0, sizeof(jl_gap_data_t));

    jl_get_gap_config_data(&gap_data);

    uint8_t service_uuid16[2] = {0};
    uint8_t manufacture_data[14] = {0};
    memcpy(service_uuid16, gap_data.service_uuid16, 2);
    memcpy(manufacture_data, gap_data.manufacture_data, 14);
    //print current gap_data.
    APP_TRC("gap_data.service_uuid16: ");
    for (uint8_t i = 0; i < 2; i++)
    {
        APP_TRC(" %02x", gap_data.service_uuid16[i]);
    }
    APP_TRC("\r\n");

    APP_TRC("gap_data.manufacture_data: ");
    for (uint8_t i = 0; i < 14; i++)
    {
        APP_TRC(" %02x", gap_data.manufacture_data[i]);
    }
    APP_TRC("\r\n");

    uint8_t discover_name[8] =
    {
      'J', 'Y', 'L', '-', 'B', 'L', 'E','_',
//      bd_addr[6],bd_addr[7],bd_addr[8],bd_addr[9],bd_addr[10],bd_addr[11],
    };

    uint8_t manufacture_data_lenght = sizeof(manufacture_data);
    uint8_t service_uuid_lenght = sizeof(service_uuid16);
    uint8_t name_lenght = sizeof(discover_name);
    uint32_t adv_data_len = 2 + name_lenght
                        + 2 + manufacture_data_lenght
                        + 2 + service_uuid_lenght;

    uint8_t *advData = (uint8_t *)lega_rtos_malloc(adv_data_len);
    if(NULL == advData){
        APP_TRC("lega_rtos_malloc failed !!!");
        return;
    }

    int offset = 0;

    *(advData + offset) =  name_lenght + 1;
    *(advData + offset + 1) = SONATA_GAP_AD_TYPE_COMPLETE_NAME;//0x09
    memcpy(advData + offset + 2, discover_name, name_lenght);
    offset += name_lenght + 2;

    *(advData + offset) =  service_uuid_lenght + 1;
    *(advData + offset + 1) = SONATA_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID;//0x03
    memcpy(advData + offset + 2, service_uuid16, service_uuid_lenght);
    offset += service_uuid_lenght + 2;

#if 1
    *(advData + offset) =  manufacture_data_lenght + 1;
    *(advData + offset + 1) = SONATA_GAP_AD_TYPE_MANU_SPECIFIC_DATA;//0xff
    memcpy(advData + offset + 2, manufacture_data, manufacture_data_lenght);
#else
    *(advData + offset) =  manufacture_data_lenght + 1;
    *(advData + offset + 1) = GAP_AD_TYPE_MANU_SPECIFIC_DATA;//0xff
    uint8_t manufacture_data1[14] = {0x32, 0x34, 0x42, 0x37, 0x37, 0x35, 0x48, 0x9d, 0x11, 0x7c, 0xd8, 0xdc, 0x00, 0x50};
    memcpy(advData + offset + 2, manufacture_data1, manufacture_data_lenght);
#endif

    APP_TRC("gap_data.advData len: %ld data:", adv_data_len);
    for (uint8_t i = 0; i < adv_data_len; i++)
    {
        APP_TRC(" %02x", advData[i]);
    }
    APP_TRC("\r\n");

    //Call API
    uint16_t ret = sonata_ble_set_scan_response_data(adv_data_len, advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    //free ptr
    lega_rtos_free(advData);
#else
    //Advertising data format
    uint8_t advData[] =
    {
      15, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'B', 'L', 'E','_',
      bd_addr[6],bd_addr[7],bd_addr[8],bd_addr[9],bd_addr[10],bd_addr[11]
    };
    //Call API
    uint16_t ret = sonata_ble_set_scan_response_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
#endif
}


static void app_ble_start_advertising()
{
    //APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //Call api
    uint16_t ret = sonata_ble_start_advertising(0, 0);
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


/*!
 * @brief
 */
static void app_ble_on()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_gap_set_dev_config_cmd cmd = {0};
    cmd.role = SONATA_GAP_ROLE_PERIPHERAL;
    cmd.gap_start_hdl = 0;
    cmd.gatt_start_hdl = 0;
    cmd.renew_dur = 0x0096;
    cmd.privacy_cfg = 0;
    cmd.pairing_mode = SONATA_GAP_PAIRING_SEC_CON | SONATA_GAP_PAIRING_LEGACY;
    cmd.att_cfg = 0x0080;
    cmd.max_mtu = 128;
    cmd.max_mps = 0x2A0;
    cmd.max_nb_lecb = 0x0A;
    cmd.hl_trans_dbg = false;
    uint16_t ret = sonata_ble_on(&cmd);//Next event:SONATA_GAP_CMP_BLE_ON
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief add profiles
 */
static void app_add_profiles()
{
    //Add dis profile for example, SONATA_GAP_CMP_PROFILE_TASK_ADD will received when added
    if (prf_add == false)
    {
        #if DEMO_PRF_DISS
        sonata_prf_diss_add_dis();
        #endif

        //Next event:SONATA_GAP_CMP_PROFILE_TASK_ADD
    }

}

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
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
            if (DEMO_ADV)
            {
                app_ble_config_legacy_advertising();
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            app_ble_set_adv_data();
            break;
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
            app_ble_set_adv_rsp_data();
            break;
        case SONATA_GAP_CMP_SET_SCAN_RSP_DATA://0x01AA
            app_ble_start_advertising();
            break;
        case SONATA_GAP_CMP_ADVERTISING_START ://0x0F06
            app_add_profiles();
            break;
        case SONATA_GAP_CMP_PROFILE_TASK_ADD://0x011B
            prf_add = true;
            if (custom_svc_start_handle == 0)
            {
                app_custom_svc_add_request();
            }
            break;
        case SONATA_GATT_CMP_NOTIFY:
            APP_TRC("APP_COMPLETE: %s  SONATA_GATT_CMP_NOTIFY, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
            break;
        case SONATA_GAP_CMP_RESET:
            //APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_RESET, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
            prf_add = false;
            custom_svc_start_handle = 0;
            user_peri_deinit();
            ble_reset_cmp();
            break;
        case SONATA_GATT_CMP_INDICATE:
        #ifdef CLOUD_JOYLINK_SUPPORT
            if (jl_send_confirm() == 0)
                APP_TRC("jl_send_confirm OPT SUCCESS.\r\n");
            else
                APP_TRC("jl_send_confirm OPT FAIL.\r\n");
        #endif
            break;
        default:
            break;
    }

    return CB_DONE;
}

static uint16_t app_ble_rsp_event_handler(uint16_t opt_id, uint8_t status, uint16_t handle, uint16_t perm, uint16_t ext_perm, uint16_t length, void *param)
{
    //APP_TRC("APP_RESPONSE: %s  opt_id=%04X,\r\n", __FUNCTION__, opt_id);
    if (status != 0)
    {
        APP_TRC("APP_RESPONSE: %s  ERROR=%04X,\r\n", __FUNCTION__, status);
    }
    switch (opt_id)
    {
        case SONATA_GATT_ADD_SVC_RSP:
        {
            //APP_TRC("APP_RESPONSE: %s  handle=%04X,\r\n", __FUNCTION__, handle);
            //Should save the start handle id for future use
            custom_svc_start_handle = handle;
            app_custom_svc_set_start_handle(custom_svc_start_handle);
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
 * @brief
 * @param conidx
 * @param conhdl
 * @param reason
 * @return
 */
static uint16_t app_gap_disconnect_ind_callback(uint8_t conidx, uint16_t conhdl, uint8_t reason)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    if (DEMO_ADV)
    {
        app_ble_config_legacy_advertising();//SONATA_GAP_CMP_ADVERTISING_CONFIG
    }

#ifdef CLOUD_JOYLINK_SUPPORT
    jl_disconnet_reset();//SDK
    void joylink_dev_reset_return_st_flag(void); //reset ble adapter flag for send status to app.
    joylink_dev_reset_return_st_flag();
#endif

    return CB_DONE;
}


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
    //APP_TRC("APP_CB: %s  conidx=%02X, info_type=%02X\r\n", __FUNCTION__, conidx, info->req);
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
            //APP_TRC("APP_CB: SONATA_GET_PHY, tx_phy:%02X, rx_phy:%02X\r\n", info->info.le_phy.tx_phy, info->info.le_phy.rx_phy);
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
    //APP_TRC("APP_CB: %s  conidx=%02X,intv_min=%04X,intv_max=%04X,latency=%04X,time_out=%04X\r\n", __FUNCTION__, conidx, intv_min, intv_max, latency, time_out);
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
    //APP_TRC("APP_CB: %s  conidx=%02X,con_interval=%04X,con_latency=%04X,sup_to=%04X\r\n", __FUNCTION__, conidx, con_interval, con_latency, sup_to);

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


static uint16_t app_gap_connection_req_callback(uint8_t conidx,sonata_gap_connection_req_ind_t *req) {
    APP_TRC("APP_CB: %s  ", __FUNCTION__);
    APP_TRC("peer_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
    {
        APP_TRC("%02X ", req->peer_addr.addr[i]);
    }
    APP_TRC(" \r\n");

    sonata_ble_gatt_exchange_mtu(conidx);

    //SDK will send connection confirm message
    return CB_REJECT;

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
    if (handle > custom_svc_start_handle)
    {
        return app_custom_svc_read_request_handler(connection_id, handle);
    }

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
    if (handle > custom_svc_start_handle)
    {
        return app_custom_svc_write_request_handler(connection_id, handle, offset, length, value);
    }

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
    length = 128;
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

};

static ble_gatt_callback ble_gatt_callbacks =
{
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


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#ifdef CLOUD_JOYLINK_SUPPORT
void app_jl_ble_adapter_init(void)
{
    jl_gatt_data_t g_jl_gatt_data;
    jl_dev_info_t dev_info;

    memset(&g_jl_gatt_data, 0x0, sizeof(jl_gatt_data_t));
    memset(&dev_info, 0x0, sizeof(jl_dev_info_t));

    memcpy(dev_info.product_uuid, JLP_UUID, 6);

    lega_wlan_get_mac_address(dev_info.mac);

    APP_TRC("jl_init->dev_info->product_uuid: %s \r\n", dev_info.product_uuid);
    APP_TRC("jl_init->dev_info->mac_str: %s \r\n", dev_info.mac);
    APP_TRC("jl_init->dev_info->mac: ");
    for (uint8_t i = 0; i < 6; i++)
    {
        APP_TRC(" %02X", dev_info.mac[i]);
    }
    APP_TRC("\r\n");
    jl_init(&dev_info);

    jl_get_gatt_config_data(&g_jl_gatt_data);

    APP_TRC("g_jl_gatt_data->service_uuid128: ");
    for (uint8_t i = 0; i < 16; i++)
    {
        APP_TRC(" %02x", g_jl_gatt_data.service_uuid128[i]);
    }
    APP_TRC("\r\n");

    APP_TRC("g_jl_gatt_data->chra_uuid128_write: ");
    for (uint8_t i = 0; i < 16; i++)
    {
        APP_TRC(" %02x", g_jl_gatt_data.chra_uuid128_write[i]);
    }
    APP_TRC("\r\n");

    APP_TRC("g_jl_gatt_data->chra_uuid128_indicate: ");
    for (uint8_t i = 0; i < 16; i++)
    {
        APP_TRC(" %02x", g_jl_gatt_data.chra_uuid128_indicate[i]);
    }
    APP_TRC("\r\n");

    //example led for joylink test device verify --- GPIO6_INDEX
    joylink_led_init();
}
#endif

/**************************************

2.4G Demo

***************************************/

#define APP_2_4G_USERMODE  1  //0 tx-mode  1:rx-mode
uint8_t app_hal_2_4g_rx_msg_handler(void *param);
uint8_t app_hal_2_4g_timeout_msg_handler(void *param);
static uint8_t app_timer_handler(uint16_t id);
void app_repeat_tx_data(void);
void app_start_action(void);

static const sonata_api_app_msg_t myMsgs[] = {
	    {APP_RX_MSG, app_hal_2_4g_rx_msg_handler},
	    {APP_TIMEOUT_MSG, app_hal_2_4g_timeout_msg_handler},
};

static sonata_app_timer_callback_t app_timer_callbacks = {
        .timeout                  = app_timer_handler,
};

static sonata_app_timer_callback_t *app_timer_callback;

void app_2_4g_init(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //sonata_2_4g_driver_reg_error_cb(app_sonata_2_4g_error_callback);
}

void app_init(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_log_level_set(SONATA_LOG_ERROR);
    if(stack_mode == APP_2_4G_MODE)
    {
        ft_hal_init();
		sonata_api_register_app_timer_callback(&app_timer_callbacks);
		sonata_api_app_msg_register(&myMsgs[0]);
		sonata_api_app_msg_register(&myMsgs[1]);
    }
    else
    {
#ifdef CLOUD_JOYLINK_SUPPORT
        app_jl_ble_adapter_init();
#endif
        sonata_ble_register_gap_callback(&ble_gap_callbacks);
        sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
        sonata_ble_register_complete_callback(&ble_complete_callbacks);
        sonata_ble_register_response_callback(&ble_rsp_callbacks);

        app_ble_on();
    }
}

static uint8_t app_timer_handler(uint16_t id)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,id);

    if (id  == APP_TX_TIMER_ID)
    {
        app_repeat_tx_data();
    }
    else if (id == APP_OPS_TIMER_ID)
    {
        app_start_action();
    }
	else
    {
		if (app_timer_callback != NULL && app_timer_callback->timeout != NULL)
		{
			app_timer_callback->timeout(id);
		}
	}
    return 0;
}

uint16_t app_register_app_timer_callback(sonata_app_timer_callback_t *cb)
{
    app_timer_callback = cb;
    return API_SUCCESS;
}


uint8_t app_master_addr[SONATA_BD_ADDR_LEN] = {0x01, 0x23, 0x45, 0xFF, 0xFF, 0xC0};
uint8_t app_slave_1_addr[SONATA_BD_ADDR_LEN] = {0x02, 0x23, 0x45, 0xFF, 0xFF, 0xC0};
uint8_t app_slave_2_addr[SONATA_BD_ADDR_LEN] = {0x03, 0x23, 0x45, 0xFF, 0xFF, 0xC0};

#define APP_MAX_SLAVE_NUM   2
#define APP_PKT_HEAD_LENGTH   2
#define APP_PKT_TIME_LENGTH   2
#define APP_ACK_LENGTH   1
#define APP_SEQ_LENGTH   2
#define APP_DELAY_TIME  300
#define APP_RX_WINDOWS     5
#define APP_TX_MARGIN_TIME  300
#define APP_ONE_TEP_TIME (APP_HAL_TX_INTERVAL + APP_RX_WINDOWS + 2)

#define APP_PKT_ACK_LENGTH   (SONATA_BD_ADDR_LEN+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_ACK_LENGTH)

#define APP_PKT_MAX_LENGTH   256
uint8_t app_pkt_buf[APP_PKT_MAX_LENGTH];
uint8_t app_rx_buf[APP_PKT_MAX_LENGTH];

#define APP_PKT_HEAD_CODE   0x7375
#define APP_PKT_ACK_CODE   0x7476

uint8_t app_tx_ack[APP_MAX_SLAVE_NUM] = { 0 };
uint8_t app_ack_flag = 0;
uint8_t app_tx_length = 0;
uint16_t app_remin_time = 0;
uint8_t app_rx_role = 0xff;
uint16_t app_last_seq_no = 0;
uint8_t app_rx_length = 0;
uint16_t app_access_ch = 2424;
uint32_t app_access_word = 0x8E89BED6;//0x75577537;



/*

 head :7375    timestamp(ms)   ack 

*/

uint16_t app_seq = 1;
uint16_t app_time_code  = APP_DELAY_TIME;
uint32_t app_tx_start_time = 0;

void app_pack(uint8_t * data, uint8_t length, uint8_t * outbuf)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,length);
    uint16_t app_head_code = APP_PKT_HEAD_CODE;
    //uint16_t app_time_code = APP_DELAY_TIME;
    app_seq++;
    memset(outbuf,0,length+SONATA_BD_ADDR_LEN+APP_PKT_HEAD_LENGTH+APP_PKT_TIME_LENGTH+APP_ACK_LENGTH);
    memmove(outbuf,app_master_addr,SONATA_BD_ADDR_LEN);
    
    memmove(outbuf+SONATA_BD_ADDR_LEN,&app_head_code,APP_PKT_HEAD_LENGTH);
    memmove(outbuf+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN,&app_seq,APP_SEQ_LENGTH);
    
    memmove(outbuf+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+APP_ACK_LENGTH,&app_time_code,APP_PKT_TIME_LENGTH);
    memmove(outbuf+APP_PKT_HEAD_LENGTH+APP_PKT_TIME_LENGTH +APP_ACK_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH,data,length);
    app_tx_length= APP_PKT_HEAD_LENGTH+APP_PKT_TIME_LENGTH +APP_ACK_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+length;
    for(int i = 0 ; i < app_tx_length ; i++)
    {
        printf("%02x",outbuf[i]);
    }
    printf("\r\n");
    APP_TRC("tx len %d seq %d\r\n", app_tx_length,app_seq);
}

void app_pkt_updata_time(uint8_t * data, uint8_t length, uint16_t timestamp)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,length);

    memmove(data+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH  ,&timestamp,APP_PKT_TIME_LENGTH);
}

void app_pkt_updata_ack(uint8_t * data, uint8_t length, uint8_t* ack_array)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,length);

    uint8_t pkt_ack  = (ack_array[0]<<0) | (ack_array[1]<<1);
    memmove(data+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH,&pkt_ack,APP_ACK_LENGTH);
}

uint8_t app_hal_2_4g_rx_data_cb(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length)
{
    
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,length);
    if(length >= APP_PKT_ACK_LENGTH)
    {
    
       if(!memcmp(data+SONATA_BD_ADDR_LEN,app_slave_1_addr,SONATA_BD_ADDR_LEN))
       {
           uint16_t pkt_seq = *((uint16_t *)(data+  2 * SONATA_BD_ADDR_LEN));
           if(pkt_seq == app_seq)
           {
               app_tx_ack[0] = 1;
           }
       }
       else if(!memcmp(data+SONATA_BD_ADDR_LEN,app_slave_2_addr,SONATA_BD_ADDR_LEN))
       {
           uint16_t pkt_seq = *((uint16_t *)(data+  2 * SONATA_BD_ADDR_LEN));
           if(pkt_seq == app_seq)
           {
               app_tx_ack[ 1] = 1;
           }
       }
       if(app_tx_ack[ 0] == 1&& app_tx_ack[ 1] == 1)
       {
           app_ack_flag = 1;
       }
       
       APP_TRC("rx_data_  %d %d %d\r\n",app_ack_flag,app_tx_ack[ 0],app_tx_ack[ 1]);
    }
    return 0;       
}

uint8_t  app_hal_2_4g_rx_data_timeout(uint16_t channel,uint8_t status)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,channel);

    if(app_ack_flag == 1 )
    {
        return 0;
    }
    sonata_api_send_app_msg(APP_TIMEOUT_MSG, (void *)&channel);
    return 0;
}


uint8_t app_hal_2_4g_tx_data_cb(uint16_t channel,uint8_t status)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,channel);
    return ft_hal_2_4g_rx_data(channel,app_access_word,APP_RX_WINDOWS,app_hal_2_4g_rx_data_cb,app_hal_2_4g_rx_data_timeout);
}

uint8_t app_hal_2_4g_fast_tx_data_cb(uint16_t channel,uint8_t status)
{
    APP_TRC("APP: %s  %d\r\n", __FUNCTION__,channel);
     app_repeat_tx_data();
     return 0 ;
}


#define SONATA_MAX_CLOCK_TIME              ((1L<<28) - 1)
#define SONATA_CLK_SUB(clock_a, clock_b)     ((uint32_t)(((clock_a) - (clock_b)) & SONATA_MAX_CLOCK_TIME))
uint8_t app_tx_counter = 0;
uint16_t  app_one_time_step = APP_ONE_TEP_TIME;
uint32_t  app_one_tx_step = APP_HAL_TX_INTERVAL;
uint16_t  app_fast_tx_num = 10; 

void app_repeat_tx_data(void)
{
    APP_TRC("APP: %s %d %d\r\n", __FUNCTION__,app_remin_time,app_one_time_step);

   if(app_remin_time < app_one_time_step)
   {
       if(app_fast_tx_num > 0 && app_ack_flag != 1)
       {
           app_fast_tx_num--;
           app_pkt_updata_time(app_pkt_buf,app_tx_length,0);
           ft_hal_2_4g_tx_data(app_access_ch,app_access_word,app_pkt_buf,app_tx_length,app_hal_2_4g_fast_tx_data_cb);
           app_tx_counter ++;
           return;
       }
       printf("tx end 1 %d\r\n",app_tx_counter);
       return;
   }
   if(app_ack_flag == 1 )
   {
       printf("tx end 2 %d\r\n",app_tx_counter);
       return;
   }
    //app_remin_time-= app_one_time_step;

    uint32_t app_tx_current_time = sonata_get_sys_time();

   uint32_t app_tx_diff_time = (SONATA_CLK_SUB(app_tx_current_time, app_tx_start_time)) * 4 / 5;

   if( app_time_code < app_tx_diff_time)
   {
       if(app_fast_tx_num > 0)
       {
           app_fast_tx_num--;
           app_pkt_updata_time(app_pkt_buf,app_tx_length,0);
           ft_hal_2_4g_tx_data(app_access_ch,app_access_word,app_pkt_buf,app_tx_length,app_hal_2_4g_fast_tx_data_cb);
           app_tx_counter ++;
           return;
       }
       printf("tx end 3 %d\r\n",app_tx_counter);
       return;
   }
   app_remin_time = app_time_code - app_tx_diff_time;
   app_tx_counter++;
   //printf("repetat %d %d %d\r\n",app_remin_time,app_one_time_step,app_time_code);
   app_pkt_updata_time(app_pkt_buf,app_tx_length,app_remin_time);
   app_pkt_updata_ack(app_pkt_buf,app_tx_length,app_tx_ack);
   ft_hal_2_4g_tx_data(app_access_ch,app_access_word,app_pkt_buf,app_tx_length,app_hal_2_4g_tx_data_cb);    
}


int app_hal_2_4g_tx_data (uint16_t channel,uint8_t *data, uint8_t len)
{
   
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    app_access_ch = channel;
    app_ack_flag = 0;
  //  app_tx_length = 0;
    app_remin_time = APP_DELAY_TIME;
    app_one_time_step = APP_ONE_TEP_TIME;
    app_tx_counter = 1;
    app_fast_tx_num = 10;
    memset(app_tx_ack,0,APP_MAX_SLAVE_NUM);
    app_pack(data,len,app_pkt_buf);
    GLOBAL_INT_DISABLE();
    app_tx_start_time = sonata_get_sys_time();
    //sonata_api_app_timer_set(APP_HAL_TX_TIMER_ID, APP_HAL_TX_INTERVAL);
    ft_hal_2_4g_tx_data(app_access_ch,app_access_word,app_pkt_buf,app_tx_length,app_hal_2_4g_tx_data_cb);
    GLOBAL_INT_RESTORE();
    return 0;
}


uint8_t app_hal_2_4g_rx_msg_handler(void *param)
{
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    //uint8_t* pkt_buf = param;
    uint16_t pkt_app_timestamp = *((uint16_t *)(param));
    GLOBAL_INT_DISABLE();
    if(pkt_app_timestamp  >= 50 )
    {
        sonata_api_app_timer_set(APP_OPS_TIMER_ID, pkt_app_timestamp);
    }
    else
    {
        app_start_action();
    }
    GLOBAL_INT_RESTORE();
    return 0;
}

uint8_t app_hal_2_4g_timeout_msg_handler(void *param)
{
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    GLOBAL_INT_DISABLE();
    sonata_api_app_timer_set(APP_TX_TIMER_ID, app_one_tx_step);
    GLOBAL_INT_RESTORE();
    return 0;
}


void app_start_action(void)
{
    
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    for(int i=0;i< app_rx_length-APP_PKT_TIME_LENGTH;i++)
    {
        printf("%02x",app_rx_buf[i+APP_PKT_TIME_LENGTH]);
    }
  
}

uint8_t app_salve_2_4g_reply_cb(uint16_t channel,uint8_t status);

void delay_us_2(int32_t us)
{
    while(us>0)
    {
        us--;
        __asm("nop");
    }
    return;
}

void app_slave_reply(uint16_t seq_no)
{
    //uint16_t app_head_code = APP_PKT_ACK_CODE;
    
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    memset(app_pkt_buf,0,SONATA_BD_ADDR_LEN+APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_ACK_LENGTH);
    if(app_rx_role  ==  0 )
    {
        memmove(app_pkt_buf,app_slave_1_addr,SONATA_BD_ADDR_LEN);
    }
    else
    {
        delay_us_2(400);
        memmove(app_pkt_buf,app_slave_2_addr,SONATA_BD_ADDR_LEN);
    }
    memmove(app_pkt_buf+SONATA_BD_ADDR_LEN,app_master_addr,SONATA_BD_ADDR_LEN);
    memmove(app_pkt_buf+2 *SONATA_BD_ADDR_LEN,&seq_no,APP_SEQ_LENGTH);
    ft_hal_2_4g_tx_data(app_access_ch,app_access_word,app_pkt_buf,SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+SONATA_BD_ADDR_LEN+APP_ACK_LENGTH,app_salve_2_4g_reply_cb);
}

uint8_t app_slave_2_4g_rx_data_cb(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length)
{
    APP_TRC("APP: %s %d\r\n", __FUNCTION__,length);
    for(int i = 0 ; i < length ; i++)
    {
        APP_TRC("%02x  ",data[i]);
    }
    if(length < APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+APP_ACK_LENGTH+APP_PKT_TIME_LENGTH)
    {
        APP_TRC("APP: %s %d\r\n", __FUNCTION__,length);
        return 0;
    }

    if(!memcmp(data,app_master_addr,SONATA_BD_ADDR_LEN))
    {
       
        uint16_t pkt_app_seq = *((uint16_t *)(data+  SONATA_BD_ADDR_LEN + APP_PKT_HEAD_LENGTH));
        uint8_t pkt_ack = *((data+SONATA_BD_ADDR_LEN + APP_PKT_HEAD_LENGTH + APP_SEQ_LENGTH));
        
        APP_TRC("Find new pkt %d %d\r\n", pkt_app_seq,app_seq);
        if(app_seq != pkt_app_seq && app_rx_length < APP_PKT_MAX_LENGTH)
        {
            app_rx_length = length- (APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+APP_ACK_LENGTH);
            memmove(app_rx_buf,data+  +APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+APP_ACK_LENGTH,app_rx_length);
            sonata_api_send_app_msg(APP_RX_MSG, (void *)app_rx_buf);
        }
        uint16_t pkt_app_timestamp = *((uint16_t *)(data+  +APP_PKT_HEAD_LENGTH+SONATA_BD_ADDR_LEN+APP_SEQ_LENGTH+APP_ACK_LENGTH));
        if((app_seq != pkt_app_seq || (!(pkt_ack & (1UL < app_rx_role )))) && pkt_app_timestamp != 0)
        {
             app_slave_reply(pkt_app_seq);
             app_seq = pkt_app_seq;
        }
        app_seq = pkt_app_seq;
    }

    return 0;
}

uint8_t  app_slave_2_4g_rx_data_timeout(uint16_t channel,uint8_t status)
{
    APP_TRC("APP: %s \r\n", __FUNCTION__);

    //return ft_hal_2_4g_rx_data(channel,app_access_word,0,app_slave_2_4g_rx_data_cb,app_slave_2_4g_rx_data_timeout);
    return 0;
}

uint8_t app_salve_2_4g_reply_cb(uint16_t channel,uint8_t status)
{
    APP_TRC("APP: %s 0x%x\r\n", __FUNCTION__,status);
    return ft_hal_2_4g_rx_data(channel,app_access_word,0,app_slave_2_4g_rx_data_cb,app_slave_2_4g_rx_data_timeout);

   return 0;
}

int app_hal_2_4g_rx_data (uint8_t role,uint16_t channel)
{
   APP_TRC("APP: %s \r\n", __FUNCTION__);
   app_access_ch = channel;
   app_rx_role = role;
   return ft_hal_2_4g_rx_data(app_access_ch,app_access_word,0,app_slave_2_4g_rx_data_cb,app_slave_2_4g_rx_data_timeout);
}


/// @} APP
