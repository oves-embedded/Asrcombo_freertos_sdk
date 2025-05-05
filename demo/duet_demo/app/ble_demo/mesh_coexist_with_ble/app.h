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
void app_ble_stop_scanning(void);
void app_ble_config_initiating(uint8_t *peer_address, uint16_t interval, uint16_t latency, uint16_t timeout);
void app_ble_disconnect(uint8_t conidx);
uint16_t app_ble_stop_initiating(void);
void app_ble_config_write(uint8_t conidx, uint16_t peer_handle, uint8_t data_len, uint8_t interval);
void app_ble_stop_write(void);
uint8_t app_ble_get_max_connection(void);
bool app_ble_get_connection_status(uint8_t conidx);
void app_ble_start_scan_with_mesh(uint16_t intv, uint16_t wnd);
void vendor_packet_loss_test(int argc,int time,int number,uint8_t len);



/// @} APP


#endif // APP_H_
