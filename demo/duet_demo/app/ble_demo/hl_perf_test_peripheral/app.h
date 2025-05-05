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
#define DEMO_LONG_RANGE (0)

#define RATE_125KB      (0)
#define RATE_500KB      (0)
#define RATE_2MB        (0)

#define THROUGHPUT_TEST (1)
#define DATA_LENGTH_EXTENSION (1)
#define APP_TX_OCTETS   (251)
#define APP_TX_TIME     (17040)
#define MAX_MTU         (257)
#define APP_UPDATE_CONNECT_INTERVAL  (48)//60ms
#define APP_UPDATE_CONNECT_EVENT     (96)//slots 60ms


#if THROUGHPUT_TEST
#define NUM_PKG       (10000)
#define NUM_PKG_SIZE  (244)
#define DEMO_SEND_RSSI  (0)
#else
#define NUM_PKG       (10)
#define NUM_PKG_SIZE  (10)
#define DEMO_SEND_RSSI  (1)
#endif

#define APP_RATE_SAVE_TAG                   (APP_DATA_SAVE_TAG_FIRST+0)
#define APP_RATE_SAVE_LEN                   (1)

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
void sonata_set_perf_test_param(uint16_t conn_intv,uint16_t ce_len);
void sonata_set_perf_test_pkt_num(uint16_t pkt_num);
void sonata_set_perf_test_pkt_size(uint8_t pkt_size);
/// @} APP


#endif // APP_H_
