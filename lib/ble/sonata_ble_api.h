/*
 * Copyright © 2023 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
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
 * @file sonata_ble_api.h
 *
 * @brief header file - ble common operation
 *
 ****************************************************************************************
 */


#ifndef _SONATA_BLE_API_H_
#define _SONATA_BLE_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_utils_api.h"
#include "sonata_gatt_api.h"
#include "sonata_gap_api.h"
#include "sonata_prf_diss_api.h"
#include "sonata_log.h"
/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
/**
 * @defgroup SONATA_BLE_API BLE_API
 * @{
 */


/// debug trace
#define SONATA_BLE_API_DBG    1
#if SONATA_BLE_API_DBG
#define SONATA_BLE_API_TRC    printf
#else
#define SONATA_BLE_API_TRC(...)
#endif //SONATA_BLE_API_DBG


#define SONATA_TASK_FIRST_MSG(task) ((uint16_t)((task) << 8))


/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */

/// App callback status
typedef enum
{
    CB_DONE           = 0, //Done in callback
    CB_REJECT         = 1, //Do nothing in callback, SDK will do it
    CB_MAX            =0XFF,
}CBStatus;

/// Result of sleep state.
enum sonata_ble_sleep_state
{
    /// Some activity pending, can not enter in sleep state
    SONATA_BLE_ACTIVE    = 0,
    /// CPU can be put in sleep state
    SONATA_BLE_CPU_SLEEP,
    /// IP could enter in deep sleep
    SONATA_BLE_DEEP_SLEEP,
};

/*
 * Type Definition
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*!
 * @brief
 */
extern uint32_t reset_error;
extern uint8_t g_slave_latency_suspend;
/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief Init ble module
 * @param hook @see sonata_ble_hook_t
 */
void sonata_ble_init(sonata_ble_hook_t hook);

/*!
 * @brief Set a bit in the prevent sleep bit field, in order to prevent the system from  going to sleep
 * @param prv_slp_bit Bit to be set in the prevent sleep bit field
 */
void sonata_ble_prevent_sleep_set(uint16_t prv_slp_bit);


/*!
 * @brief Clears a bit in the prevent sleep bit field, in order to allow the system
 *        going to sleep
 * @param prv_slp_bit Bit to be cleared in the prevent sleep bit field
 */
void sonata_ble_prevent_sleep_clear(uint16_t prv_slp_bit);

/*!
 * @brief get the value of prevent sleep bit field
 *
 */
uint16_t sonata_ble_prevent_sleep_get(void);

/*!
 * @brief sonata ble prevent sleep value get all 32 bits
 * @param void
 */
uint32_t sonata_ble_prevent_sleep_get_all(void);


/*!
 * @brief Reset ble module
 * @return API_SUCCESS
 */
uint16_t sonata_ble_reset();

/*!
 * @brief ble stack schedule
 */
void sonata_ble_schedule(void);

/*!
 * @brief ble enter sleep
 * @return Sleep state
 */
uint8_t sonata_ble_sleep(void);

/*!
 * @brief ble irq handler
 */
void sonata_ble_isr(void);

#if defined(CFG_PLF_SONATA)
uint32_t sonata_ble_get_sleep_duration_ms(void);
#endif

/*
 * FUNCTION DECLARATIONS             Connection help function End
 ****************************************************************************************
 */
//#ifdef CFG_RF_2_4G
typedef  void (*pf_sonata_2_4g_isr_t )(void);

extern pf_sonata_2_4g_isr_t pf_sonata_2_4g_isr;
extern uint32_t sonata_2_4g_open;

//#endif
/** @}*/


#endif //_SONATA_BLE_API_H_

