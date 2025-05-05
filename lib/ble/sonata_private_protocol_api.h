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
 * @file sonata_private_protocol_api.h
 *
 * @brief header file - asr utilities
 *
 ****************************************************************************************
 */


#ifndef _SONATA_PRIVATE_PROTOCOL_API_H_
#define _SONATA_PRIVATE_PROTOCOL_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include "sonata_error_api.h"


/**
 * @defgroup SONATA_PRIVATE_PROTOCOL_API
 * @{
 */

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */


/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */


/*!
* @brief app msg operation type
*/
typedef enum
{
    SONATA_2_4G_1M_PHY,
    SONATA_2_4G_2M_PHY,
}SONATA_2_4G_PHY;

typedef enum
{
    SONATA_2_4G_RX,
    SONATA_2_4G_TX,
}SONATA_2_4G_DIRECTION;

typedef enum
{
    SONATA_2_4G_MODE,
    SONATA_BLE_MODE,
}SONATA_PP_MODE;


/*
 * Type Definition
 ****************************************************************************************
 */

typedef uint8_t sonata_2_4g_ch_t;

/// rx callback
typedef uint8_t (*PF_SONATA_2_4G_RX_CALLBACK)(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length);

/// tx callback
typedef uint8_t (*PF_SONATA_2_4G_TX_CALLBACK)(uint16_t channel,uint8_t status);

/// timeout callback
typedef uint8_t (*PF_SONATA_2_4G_TIMEOUT_CALLBACK)(uint16_t channel,uint8_t status);

/// rx errot callback
typedef uint8_t (*PF_SONATA_2_4G_RX_ERROR_CALLBACK)(uint16_t err_code);


/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief Enable 2.4G private protocol
 * @return API_SUCCESS or API_FAILURE
 */
uint16_t sonata_2_4g_enable(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief Disable 2.4G private protocol
 * @return API_SUCCESS
 */
uint16_t sonata_2_4g_disable(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol rx error cb
 * @return API_SUCCESS
 */
uint16_t sonata_2_4g_reg_rx_error_cb(PF_SONATA_2_4G_RX_ERROR_CALLBACK rx_error_cb);


/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol tx
 * @return API_SUCCESS
 */
uint16_t sonata_2_4g_tx_data(uint16_t channel ,uint32_t access_word, uint8_t *data, uint8_t len, PF_SONATA_2_4G_TX_CALLBACK cb);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol rx
 * @return API_SUCCESS
 */
uint16_t sonata_2_4g_rx_data(uint16_t channel ,uint32_t access_word, uint16_t timeout_ms, PF_SONATA_2_4G_RX_CALLBACK cb,PF_SONATA_2_4G_TIMEOUT_CALLBACK timeout_cb);
/*!
 * @brief  2.4G private protocol stop rx
 * @return API_SUCCESS
 */
void sonata_2_4g_rx_stop(void);

/*after call this function, wait 1ms for irq clear*/
void sonata_2_4g_txrx_stop(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol switch to ble
 * @return API_SUCCESS
 */
void sonata_switch_to_ble(void);
/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  ble switch to 2.4G private protocol rx
 * @return API_SUCCESS
 */
void sonata_switch_to_2_4g(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol mode, ble or 2.4g
 * @return API_SUCCESS
 */
SONATA_PP_MODE sonata_2_4g_get_mode(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol switch mode, ble to 2.4g or 2.4g to ble
 * @return API_SUCCESS
 */
void sonata_2_4g_switch_mode(void);

/*
 * FUNCTION DECLARATIONS             Developer use function
 ****************************************************************************************
 */
/*!
 * @brief  2.4G private protocol change to mode
 * @return API_SUCCESS
 */
void sonata_2_4g_change_mode(SONATA_PP_MODE mode);


void sonata_2_4g_init_param(void);

/*for rx stop abnormal*/
void sonata_2_4g_rx_time_record(uint32_t rx_ms);
bool sonata_2_4g_rx_time_past(uint32_t *rx_con_ms);

/** @}*/
#endif //_SONATA_PRIVATE_PROTOCOL_API_H_

