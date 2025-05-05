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
 * @file app.h
 *
 * @brief Application entry point
 *
 ****************************************************************************************
 */

#ifndef APP_H_
#define APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_ble_hook.h"





/*
 * DEFINES
 ****************************************************************************************
 */



/*
 * MACROS
 ****************************************************************************************
 */
/// debug trace
#define APP_DBG    1
#if APP_DBG
#define APP_TRC    printf
#else
#define APP_TRC(...)
#endif //APP_DBG

#define APP_BLE_CENTRAL
/*
 * ENUMERATIONS
 ****************************************************************************************
 */




enum app_connect_state
{
    ///Connection succeeded
    APP_STATE_CONNECTED = 0,
    /// Link is disconnected
    APP_STATE_DISCONNECTED,
};



/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */




/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
extern sonata_ble_hook_t app_hook;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the BLE demo application.
 ****************************************************************************************
 */
void app_init(void);
void app_ble_config_scanning();
void app_ble_stop_scanning(void);
uint8_t app_get_connect_status(void);
uint16_t app_ble_advertising_stop();
void app_ble_config_initiating(void);
void app_ble_stop_initiating(void);
void app_ble_set_test_interval(uint16_t interval);
void app_ble_set_write_param(uint8_t *data,  uint8_t length);
void app_ble_set_test_target(uint8_t * target);
/// @} APP


#endif // APP_H_
