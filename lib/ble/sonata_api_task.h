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
 * @file sonata_api_task.h
 *
 * @brief header file - asr api task
 *
 ****************************************************************************************
 */


#ifndef _SONATA_API_TASK_H_
#define _SONATA_API_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_utils_api.h"
#include "sonata_config.h"
/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/// Number of ASR API Task Instances
#define SONATA_API_TASK_IDX_MAX                 (1)

/// debug trace
#define SONATA_API_TASK_DBG        0
#if SONATA_API_TASK_DBG
#define SONATA_API_TASK_TRC    printf
#else
#define SONATA_API_TASK_TRC(...)
#endif //SONATA_API_TASK_DBG

/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */
/// states of asr api task
typedef enum
{
    /// initialization state
    SONATA_API_TASK_INIT,
    /// idle state
    SONATA_API_TASK_IDLE,
    /// busy state
    SONATA_API_TASK_BUSY,

    /// Number of defined states.
    SONATA_API_TASK_STATE_MAX
} sonata_api_task_state_t;



/// asr api task messages
typedef enum
{
    SONATA_API_DUMMY_MSG = STACK_TASK_FIRST_MSG(SONATA_TASK_ID_APP),
    /// message from application
    SONATA_API_APP_MSG,

    SONATA_API_LAST_MSG,
} sonata_api_task_msg_id;

typedef enum
{
    SONATA_INT_EVENT_BLE_DATA           = (0xD1),
    SONATA_INT_EVENT                    = (0xD2),
    SONATA_2_4G_PKT_MSG,
}sonata_api_internal_event;

typedef enum
{
    APP_INTERNAL_TIMER_START          = (15),
#ifdef SONATA_CFG_LOW_POWER
    APP_INTERNAL_TIMER_LP_RCO32K_CALI_PWON,
    APP_INTERNAL_TIMER_LP_RCO32K_CALI_PERIOD,
    APP_INTERNAL_TIMER_LP_RCO32K_CHECK,
    APP_INTERNAL_TIMER_TEMPR_CHECK,
    APP_INTERNAL_TIMER_RTOS_WAKEUP,
#endif

    APP_INTERNAL_TIMER_MAX            = (39),
}app_internel_timer_msg;


/*
 * Type Definition
 ****************************************************************************************
 */
/// Structure containing information about the handlers for an application subtask
typedef struct sonata_api_subtask_handlers
{
    /// Pointer to the message handler table
    const struct ke_msg_handler *p_msg_handler_tab;
    /// Number of messages handled
    uint16_t msg_cnt;
} sonata_api_subtask_handlers_t;

typedef struct sonata_app_timer_callback
{
    uint8_t (*timeout)(uint16_t id);

}sonata_app_timer_callback_t;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/*!
 * @brief
 */

void sonata_api_task_init(void);

uint16_t sonata_api_register_app_timer_callback(sonata_app_timer_callback_t *cb);

uint8_t sonata_app_internal_timer_handler(uint16_t id);

extern const sonata_api_subtask_handlers_t *p_app_mesh_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_diss_api_ke_msg_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_bass_api_ke_msg_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_otas_api_ke_msg_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_hogpd_api_ke_msg_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_hogprh_api_ke_msg_handlers;
extern const sonata_api_subtask_handlers_t *p_prf_sec_api_ke_msg_handlers;
#if (CORE_BLUETOOTH_RESOURCE)
extern pf_mem_config_t pf_mem_config_patch;
#endif

#ifdef SONATA_CFG_LOW_POWER
void sonata_api_lp_timer_triggle(void);
#endif

#endif //_SONATA_API_TASK_H_

