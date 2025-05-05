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
#include "sonata_mesh_api.h"
#include "sonata_error_api.h"
#include "duet_timer.h"
#include "lega_rtos_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_RESET_TIMER_INTERVAL             (6000) //unit:ms
#define APP_TIMER_INTERVAL                   (1000) //unit:ms
#define APP_ATTATION_TIMER                   (3000) //unit:ms
#define APP_RESET_SWITCH_COUNT               (6)
#define APP_RESET_SAVE_TAG                   (APP_DATA_SAVE_TAG_FIRST+0)
#define APP_RESET_SAVE_LEN                   (1)
#define APP_UUID_MAC_OFFSET                  7

#define ASR_SZ_COMP_ID                       0x0917

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

extern int __wrap_printf(const char* format, ...);
extern void ble_reset_cmp(void);

static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam);
extern void sonata_mesh_api_init(void);

/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{
    sonata_mesh_api_init();
}

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

static mesh_prov_param_t init_prov_param = {
     .uuid  =  {
     //   0xA8, 0x01, //CID AliBaBa
     //   0xD1, //PID
     //   0x8A, 0x25, 0x00, 0x00, //ProductID: 9610
     //   0x57, 0x5D, 0x69, 0x63, 0xA7, 0xF8, //MAC:f8a763695d57
     //   0x00, //FeatureFlag
     //   0x00, 0x00//RFU
        0x17,0x09,0x1A,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00 },
     .p_uri = NULL,
     .static_oob = 0,
    .static_oob_len = 0,
    .oob_info = 0,
    .output_size = 0,
    .output_actions = 0,
    .input_size = 0,
    .input_actions = 0,
    .cid = ASR_SZ_COMP_ID,
    .pid = 0x001A,
    .vid = 0x0001,
    .loc=  0x0100,
};

static uint8_t static_auth_val[MESH_PROVISION_AUTH_MAX_NUM]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
static uint32_t mesh_feature = MESH_FEAT_RELAY_NODE_SUP|MESH_FEAT_PROXY_NODE_SUP;
static duet_timer_dev_t mesh_timer;
static uint8_t reset_flag = 0xff;
sonata_ble_hook_t app_hook = {
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




static void app_ble_on(void)
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
    uint16_t ret = sonata_ble_on(&cmd);//app_ble_on_complete_callback
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


static ble_complete_callback ble_complete_callbacks = {
        .ble_complete_event                 = app_ble_complete_event_handler,
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
STATUS mesh_core_evt_ind(mesh_core_evt_ind_t evt ,mesh_core_evt_ind_params_t * p_param)
{

    switch(evt)
    {
        case (MESH_PUBLIC_KEY_REQUEST):
        {

        }
        break;
        case MESH_AUTH_OOB_IND:
        {
            sonata_mesh_send_prov_auth_data_rsp(true,MESH_PROVISION_AUTH_MAX_NUM,static_auth_val);
        }
        break;
        case MESH_ATTATION_IND:
        {

        }
        break;
        case MESH_APPKEY_UPDATE:
        {

        }
        break;
        case MESH_APPKEY_BINDED:
        {

        }
        break;
        case MESH_GRPADDR_SUBSED:
        {

        }
        break;
        case MESH_PROVISION_STATE:
        {
            APP_TRC("MESH_PROVISION_STATE %d\r\n",p_param->prov_state_ind.state);
            if(p_param->prov_state_ind.state) // true means be provisioned success
            {
                sonata_mesh_save_data();
            }
        }
        break;
        case MESH_APPKEY_BINDING:
        {
            APP_TRC("MESH_APPKEY_BINDING add %u :element %u, modelid 0x%x, index %u \r\n  ",p_param->appkey_bind.bind,
                p_param->appkey_bind.element ,(unsigned int )p_param->appkey_bind.modelid,p_param->appkey_bind.appkey_global_index);
            sonata_mesh_save_data();
        }
        break;
        default: break;
    }
    return MESH_ERROR_NO_ERROR;

}

void  gens_onoff_cb(mesh_state_ind_t * p_state )
{
    if(p_state->type != MESH_STATE_GEN_ONOFF)
    {
        return;
    }
    if(p_state->state)
    {
        turn_on_led();
    }
    else
    {
        turn_off_led();
    }
}


void started_cb(void)
{
    // user_load_data(); // user

     if(!sonata_mesh_is_provisioned())
     {
          APP_TRC("auto mesh_provision_service_broadcast\r\n");
     }
     else
     {

         APP_TRC("sonata_mesh_proxy_service_broadcast\r\n");
         sonata_mesh_proxy_service_broadcast(MESH_PROXY_ADV_START_NET);
     }
     app_hal_init();
}



static void app_ble_on_callback(uint16_t status)
{
    APP_TRC("bluetooth on: 0x%X\r\n", status);
    mesh_set_params_t set_param;
    sonata_mesh_register_core_evt_ind(mesh_core_evt_ind);
    set_param.mesh_role.role = MESH_PROV_ROLE_DEVICE;
    sonata_mesh_param_set(MESH_ROLE,&set_param);

    set_param.addr_type.type = MESH_PRIVATE_STATIC_ADDR;
    sonata_mesh_param_set(MESH_ADDR_TYPE,&set_param);
    set_param.feature_param.feature = mesh_feature;
    sonata_mesh_param_set(MESH_FEATURE_SUPPORT,&set_param);

    // for mesh multi-node test
    mesh_get_params_t get_param;
    sonata_mesh_param_get(MESH_SYSTEM_ADDR,&get_param);
    memmove(init_prov_param.uuid+APP_UUID_MAC_OFFSET,get_param.addr.bd_addr,MESH_ADDR_LEN);
    set_param.prov_param = init_prov_param;
    sonata_mesh_param_set(MESH_PROV_PARAM,&set_param);
    sonata_mesh_model_register(MESH_MODELID_GENS_OO,0,1,gens_onoff_cb);
    sonata_mesh_start(started_cb);
}

static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
            app_ble_on_callback(status);
            break;
        case SONATA_GAP_CMP_RESET:
            user_peri_deinit();
            ble_reset_cmp();
            break;
        default:
            break;
    }

    return CB_DONE;
}



static void app_clean_data_cb(void)
{
    platform_reset(0);
}

CRITICAL_FUNC_SEG void app_reset_timer_cb(void *arg)
{
    sonata_fs_len_t len = APP_RESET_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RESET_SAVE_TAG;
    static uint32_t time_tick = 0;
    time_tick++;
    if(    reset_flag == 0xFE)
    {
        toggle_led();
        if(time_tick >= APP_ATTATION_TIMER/APP_TIMER_INTERVAL)
        {
            reset_flag = 0xFF;
            sonata_fs_write(tag,len,&reset_flag);
            sonata_mesh_clean_data(app_clean_data_cb);
        }
        return;
    }
    if(time_tick > APP_RESET_TIMER_INTERVAL/APP_TIMER_INTERVAL)
    {
       duet_timer_stop(&mesh_timer);
       reset_flag = 0xFF;
       sonata_fs_write(tag,len,&reset_flag);
       duet_timer_stop(&mesh_timer);
    }

}

static void app_timer_init(void)
{
    APP_TRC("app_timer_init\r\n");
    mesh_timer.port = DUET_TIMER1_INDEX;
    mesh_timer.config.period = APP_TIMER_INTERVAL*1000;
    mesh_timer.config.reload_mode = TIMER_RELOAD_AUTO;
    mesh_timer.config.cb = app_reset_timer_cb;
    mesh_timer.config.arg = NULL;
    mesh_timer.priv = NULL;
    duet_timer_init(&mesh_timer);
    duet_timer_start(&mesh_timer);
}


static void app_reset_process(void)
{
    sonata_fs_len_t len = APP_RESET_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RESET_SAVE_TAG;
    uint8_t status = sonata_fs_read(tag,&len,&reset_flag);
    APP_TRC("reset_flag %d  %d\r\n",reset_flag,status);
    if(reset_flag == 0xff)
    {
        reset_flag = 1;
    }
    else
    {
        reset_flag ++;
    }
    len = APP_RESET_SAVE_LEN;
    if(reset_flag == APP_RESET_SWITCH_COUNT)
    {
        reset_flag = 0xFE;
    }
    else
    {
        sonata_fs_write(tag,len,&reset_flag);
    }
    app_timer_init();
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_init(void)
{
    APP_TRC("app_init\r\n");
    sonata_log_level_set(SONATA_LOG_VERBOSE);
    app_reset_process();
    //sonata_ble_register_gap_callback(&ble_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);

    app_ble_on();

}



/// @} APP
