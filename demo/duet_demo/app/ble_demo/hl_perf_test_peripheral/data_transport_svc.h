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
 * @file data_transport_svc.h
 *
 * @brief Data transport service (DTS in short)
 *
 ****************************************************************************************
 */
#ifndef _DATA_TRANSPORT_SVC_H_
#define _DATA_TRANSPORT_SVC_H_
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_gatt_api.h"
#include "sonata_ble_api.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */

/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */

enum csvc_att_db_handles
{
    DTS_CHAR,
    DTS_NTF_VAL,
    DTS_CFG,

    DTS_IDX_NB,
};


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief Add DTS service
 * @return
 */
uint16_t dts_add();
/*!
 * @brief DTS read request
 * @param connection_id connection index
 * @param localHandle custom handle id
 * @param handle   handle id in system
 * @return @see CBStatus
 */
uint16_t dts_read_request_handler(uint8_t connection_id, uint16_t handle);
/*!
 * @brief DTS write request
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
uint16_t dts_write_request_handler(uint8_t connection_id, uint16_t handle,
                                   uint16_t offset, uint16_t length, uint8_t *value);

/*!
 * @brief Seav DTS start handle id
 * @param handle
 */
void dts_set_start_handle(uint16_t handle);

/*!
 * @brief Interface for sending data done
 * @param status
 */
void dts_data_send_done(uint8_t status);
/*!
 * @brief DTS should know MTU value, and  the real mtu is "app_mtu = mtu -3"
 * @param mtu
 */
void dts_set_mtu(uint16_t mtu);

/*!
 * @brief Handler for UART IRQ
 * @param ch
 */
void dts_uartirq_handler(char ch);

/*!
 * @brief get prefered MTU
 * @return
 */
uint8_t dts_get_preferred_mtu();

void dts_send_dec_data(uint8_t data);
uint8_t dts_timer_handler(uint16_t id);
#endif //_DATA_TRANSPORT_SVC_H_

