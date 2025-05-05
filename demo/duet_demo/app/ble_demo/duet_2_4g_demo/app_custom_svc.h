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
 * @file \app_custom_svc.h
 *
 * @brief custom service db
 *
 ****************************************************************************************
 */
#ifndef _APP_CUSTOM_SVC_H_
#define _APP_CUSTOM_SVC_H_
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
#ifdef CLOUD_JOYLINK_SUPPORT
enum csvc_att_db_handles
{
    //CSVC_IDX_SVC,
    CSVC_IDX_1_CHAR ,
    CSVC_IDX_1_VAL,
    CSVC_IDX_2_CHAR,
    CSVC_IDX_2_IND_VAL,
    CSVC_IDX_2_CFG,
    CSVC_IDX_NB,
};
#else
enum csvc_att_db_handles
{
    //CSVC_IDX_SVC,
    CSVC_IDX_1_CHAR ,
    CSVC_IDX_1_VAL,
    CSVC_IDX_2_CHAR,
    CSVC_IDX_2_VAL,
    CSVC_IDX_3_CHAR,
    CSVC_IDX_3_NTF_VAL,
    CSVC_IDX_3_CFG,
    CSVC_IDX_4_CHAR,
    CSVC_IDX_4_IND_VAL,
    CSVC_IDX_4_CFG,
    CSVC_IDX_NB,
};
#endif

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief Add custom service database
 * @return
 */
uint16_t app_custom_svc_add_request();
/*!
 * @brief GATT read request handler
 * @param connection_id connection index
 * @param localHandle custom handle id
 * @param handle   handle id in system
 * @return @see CBStatus
 */
uint16_t app_custom_svc_read_request_handler(uint8_t connection_id,  uint16_t handle);
/*!
 * @brief GATT write request
 * @param connection_id connection index
 * @param localHandle custom handle id
 * @param handle   handle id in system
 * @param offset   read offset
 * @param length  value length
 * @param value  value array
 * @return @see CBStatus
 */
uint16_t app_custom_svc_write_request_handler(uint8_t connection_id, uint16_t handle,
        uint16_t offset, uint16_t length, uint8_t *value);

/*!
 * @brief Set custom service's start att handle
 * @param handle
 */
void app_custom_svc_set_start_handle(uint16_t handle);




#endif //BLE_SOC_APP_CUSTOM_SVC_H

