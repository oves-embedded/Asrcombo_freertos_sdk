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


#define DEMO_PRF_DISS   (1)
#define DEMO_PRF_BASS   (0)
#define DEMO_LONG_RANGE (1)
#define DEMO_SEND_RSSI  (1)
/*
 * ENUMERATIONS
 ****************************************************************************************
 */


typedef enum
{
    APP_TIMER_START          = (40),
    APP_TIMER_READ_RSSI      = (APP_TIMER_START + 1),

}app_timer_msg;



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

uint8_t app_get_connection_id();
void app_ble_get_peer_rssi();
/// @} APP


#endif // APP_H_
