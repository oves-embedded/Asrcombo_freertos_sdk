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
#include "data_transport_svc.h"
#include "lega_rtos_api.h"


#if BLE_BATT_SERVER
#include "sonata_prf_bass_api.h"
#endif //BLE_BATT_SERVER

/*
 * DEFINES
 ****************************************************************************************
 */
#define LE_PHY_500_LEN 1
#define LE_PHY_500_TAG 0x85
/*!
 * @brief save local handle start index
 */
static uint16_t custom_svc_start_handle = 0;
//Mark for profile dis
static bool prf_add = false;

static uint8_t connection_id = 0;

static uint8_t rate_2mb = RATE_2MB;
static uint8_t rate_125kb = RATE_125KB;
static uint8_t rate_500kb = RATE_500KB;
static uint8_t rate_flag = 0xff;
extern uint8_t pkt_num_size;
#if THROUGHPUT_TEST
extern uint8_t dle_flag;
static uint16_t app_conn_intv=APP_UPDATE_CONNECT_INTERVAL;
static uint16_t app_ce_len=APP_UPDATE_CONNECT_EVENT;
#endif
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
static void app_ble_config_legacy_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    APP_TRC("1Mbs rate adv config\r\n");
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

static void app_ble_config_extended_advertising()
{
    APP_TRC("APP: %s  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n", __FUNCTION__);
    sonata_gap_extended_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_EXT_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0xE2;//Todo
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    param.prim_adv_intv_min = 32;
    param.prim_adv_intv_max = 32;
    param.prim_chnl_map = 0x07;
    if((rate_125kb)||(rate_500kb)||(DEMO_LONG_RANGE))
    {
        param.prim_phy = SONATA_GAP_PHY_LE_1MBPS;
        param.second_max_skip = 0;
        param.second_phy = SONATA_GAP_PHY_LE_CODED - 1;//3; //GAPM_PHY_TYPE_LE_CODED;
        param.second_adv_sid = 0; //Todo
        if(rate_125kb||(DEMO_LONG_RANGE))
        {
            APP_TRC("RATE_125KB adv config\r\n");
        }
        else
        {
           APP_TRC("RATE_500KB adv config\r\n");
        }
    }
    if(rate_2mb)
    {
        param.prim_phy = SONATA_GAP_PHY_LE_1MBPS;
        param.second_max_skip = 0;
        param.second_phy = 2;//
        param.second_adv_sid = 0; //Todo
        APP_TRC("RATE_2MB adv config\r\n");
    }

    uint16_t ret = sonata_ble_config_extended_advertising(SONATA_GAP_STATIC_ADDR, &param);//Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

static void app_ble_set_adv_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t advData[] = { //Advertising data format
            8, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'D', 'T', 'S', 0
    };
    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
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
    cmd.role = SONATA_GAP_ROLE_ALL;
    cmd.gap_start_hdl = 0;
    cmd.gatt_start_hdl = 0;
    cmd.renew_dur = 0x0096;
    cmd.privacy_cfg = 0;
    cmd.pairing_mode = SONATA_GAP_PAIRING_SEC_CON | SONATA_GAP_PAIRING_LEGACY;
    cmd.att_cfg = 0x0080;
    #if THROUGHPUT_TEST
    cmd.max_mtu = MAX_MTU;
    #else
    cmd.max_mtu = dts_get_preferred_mtu();
    #endif
    cmd.max_mps = 0x02A0;
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

        #if DEMO_PRF_BASS
        sonata_prf_batt_add_bas();
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
    #if THROUGHPUT_TEST
    #else
    APP_TRC("APP_COMPLETE: %s  opt_id=%04X,status=%02X,param=%04X,dwparam=%lu\r\n", __FUNCTION__, opt_id, status, param, dwparam);
    #endif
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01

            if ((DEMO_LONG_RANGE)||(rate_125kb)||(rate_500kb)||(rate_2mb))
            {
                //sonata_ll_set_txpwr(10);
                app_ble_config_extended_advertising();
            }
            else
            {
                app_ble_config_legacy_advertising();
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            app_ble_set_adv_data();
            break;
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
            app_ble_start_advertising();
            break;
        case SONATA_GAP_CMP_ADVERTISING_START ://0x0F06
            app_add_profiles();
            break;
        case SONATA_GAP_CMP_PROFILE_TASK_ADD://0x011B
            prf_add = true;
            if (custom_svc_start_handle == 0)
            {
                dts_add();
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_DELETE:
            sonata_ble_gatt_exchange_mtu(connection_id);
            break;
        case SONATA_GATT_CMP_NOTIFY:
            #if THROUGHPUT_TEST
            #else
            APP_TRC("APP_COMPLETE: %s  SONATA_GATT_CMP_NOTIFY, seq:%d \r\n", __FUNCTION__, (uint16_t) dwparam);
            #endif
            dts_data_send_done(status);
            break;
        case SONATA_GAP_CMP_RESET:
            //APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_RESET, seq:%d \r\n",__FUNCTION__,(uint16_t)dwparam);
            ble_reset_cmp();
            break;
        default:
            break;
    }

    return CB_DONE;
}

void app_ble_get_peer_rssi()
{
    sonata_ble_gap_get_peer_info(connection_id, SONATA_GET_PEER_CON_RSSI);

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
            custom_svc_start_handle = handle;
            dts_set_start_handle(custom_svc_start_handle);
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
    if ((DEMO_LONG_RANGE)||(rate_125kb)||(rate_500kb)||(rate_2mb))
    {
        app_ble_config_extended_advertising();
    }
    else
    {
        app_ble_config_legacy_advertising();
    }
    turn_off_led();
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
        {
            uint8_t rssi = 0 - info->info.rssi.rssi;
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_RSSI, rssi:%d, convert rssi=%d\r\n", info->info.rssi.rssi, rssi);
            dts_send_dec_data(rssi);
        }
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
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i) { APP_TRC("%02X ", req->peer_addr.addr[i]); }
    APP_TRC(" \r\n");
    connection_id = conidx;

//    sonata_gap_connection_cfm_t cfm = {0};
//    cfm.auth = GAP_AUTH_REQ_NO_MITM_NO_BOND;
//    sonata_ble_gap_send_connection_cfm(conidx, &cfm);
//    return CB_DONE;

    #if DEMO_PRF_BASS
    sonata_prf_batt_enable_prf(conidx);
    #endif
    turn_on_led();
    #if THROUGHPUT_TEST
    sonata_ble_gap_update_connection_params(conidx, 0, app_conn_intv,app_conn_intv,0,0x012C, app_ce_len, app_ce_len);

    if(dle_flag)
    {
        sonata_ble_gap_set_le_pkt_size(connection_id, APP_TX_OCTETS, APP_TX_TIME);
    }
    #endif

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
    if (handle > custom_svc_start_handle)
    {
        return dts_read_request_handler(connection_id, handle);
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
        return dts_write_request_handler(connection_id, handle, offset, length, value);
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
    dts_set_mtu(mtu);
    return CB_DONE;
}


uint8_t app_get_connection_id()
{
    return connection_id;
}


#if THROUGHPUT_TEST
#else
static uint8_t app_timer_handler(uint16_t id)
{
    APP_TRC("APP: %s  . id=%d\r\n", __FUNCTION__, id);

    if (id >= APP_TIMER_START)
    {
        dts_timer_handler(id);
    }

    return 0;
}
#endif
void sonata_set_perf_test_param(uint16_t conn_intv,uint16_t ce_len)
{
    #if THROUGHPUT_TEST
    app_conn_intv=conn_intv;
    app_ce_len=ce_len;
    #endif
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

#if THROUGHPUT_TEST
#else
static sonata_app_timer_callback_t app_timer_callbacks = {
        .timeout                  = app_timer_handler,
};
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void app_init(void)
{
    // Initialize the LE Coded PHY 500 Kbps selection (false: 125 Kbps, true: 500 Kbps), 125 Kbps by default
    sonata_fs_len_t len = APP_RATE_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RATE_SAVE_TAG;
    sonata_fs_read(tag,&len,&rate_flag);
    if(rate_flag==1)
    {
        rate_2mb   = 1;
        rate_125kb = 0;
        rate_500kb = 0;
        printf("RATE_2M\r\n");
    }
    else if(rate_flag==2)
    {
        rate_2mb   = 0;
        rate_125kb = 1;
        rate_500kb = 0;
        printf("RATE_125KB\r\n");
    }
    else if(rate_flag==3)
    {
        rate_2mb   = 0;
        rate_125kb = 0;
        rate_500kb = 1;
        printf("RATE_500KB\r\n");

    }
    else
    {
        rate_2mb   = 0;
        rate_125kb = 0;
        rate_500kb = 0;
        printf("RATE_1M\r\n");
    }
    if(rate_500kb)
    {
        uint8_t phy_500=1;
        sonata_fs_len_t len = LE_PHY_500_LEN;
        sonata_fs_tag_t tag = LE_PHY_500_TAG;
        sonata_fs_write(tag,len,&phy_500);
    }
    if((rate_125kb)||(DEMO_LONG_RANGE))
    {
        uint8_t phy_500=0;
        sonata_fs_len_t len = LE_PHY_500_LEN;
        sonata_fs_tag_t tag = LE_PHY_500_TAG;
        sonata_fs_write(tag,len,&phy_500);
    }
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_log_level_set(SONATA_LOG_ERROR);

    sonata_ble_register_gap_callback(&ble_gap_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);
    sonata_ble_register_response_callback(&ble_rsp_callbacks);
    #if THROUGHPUT_TEST
    #else
    sonata_api_register_app_timer_callback(&app_timer_callbacks);
    #endif
    app_ble_on();
    app_hal_init();
    // at command init

}



/// @} APP
