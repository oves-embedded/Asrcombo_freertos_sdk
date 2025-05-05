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
#include "sonata_utils_api.h"
#include "lega_rtos_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATION
 ****************************************************************************************
 */
extern int __wrap_printf(const char* format, ...);
static void app_ble_on_complete_callback(uint16_t status);
static void app_ble_on(void);
extern void ble_reset_cmp(void);
static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam);
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */



/*
 * LOCAL FUNCTION DEFINITIONS    Callback functions
 ****************************************************************************************
 */


/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

static ble_complete_callback ble_complete_callbacks =
{
    .ble_complete_event                 = app_ble_complete_event_handler,
};

ble_gatt_callback ble_gatt_callbacks =
{
    .gatt_disc_svc                      = NULL,
    .gatt_disc_all_svc_complete         = NULL,
    .gatt_read                          = NULL,
    .gatt_read_complete                 = NULL,
};


/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{

}

/*
 * GLOBAL VARIABLE DEFINITIONS
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


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief
 * @param status
 */
void app_init(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    // register call back
    //sonata_ble_register_gap_callback(&ble_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);

    // register call back
    app_ble_on();
}

/*!
 * @brief
 * @param status
 */
static void app_ble_on_complete_callback(uint16_t status)
{
    APP_TRC("bluetooth on\r\n");


}

static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
            app_ble_on_complete_callback(status);
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

/*!
 * @brief
 */
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
    cmd.hl_trans_dbg = true;
    uint16_t ret = sonata_ble_on(&cmd);//app_ble_on_complete_callback
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


/// @} APP
