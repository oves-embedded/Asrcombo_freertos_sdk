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

#define APP_2_4G_STATE_IDLE  0
#define APP_2_4G_STATE_RX    1
#define APP_2_4G_STATE_TX    2
#define APP_MAX_CH_NUM       2
#define APP_MAX_BUF_LEN  128

#define APP_RX_WINSZ               5 //uints:ms
#define APP_RX_INTERVAL            10   //uints:ms
#define APP_RX_SCAN_CHG_INTERVAL   1000   //uints:ms
#define APP_RX_DELAY_TIMER_ID      1
#define APP_RX_CH_TIMER_ID         2
#define APP_RX_TIMEOUT_MSG         20
#define APP_RX_MSG            22
#define APP_TIMEOUT_MSG       23
#define APP_TX_TIMER_ID         13
#define APP_OPS_TIMER_ID        14

/*
 * MACROS
 ****************************************************************************************
 */
/// debug trace
#define APP_DBG    0
#if APP_DBG
#define APP_TRC    printf
#else
#define APP_TRC(...)
#endif //APP_DBG


/*
 * ENUMERATIONS
 ****************************************************************************************
 */






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


/// @} APP


#endif // APP_H_
