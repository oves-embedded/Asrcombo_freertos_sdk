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
 * @file sonata_ble_transport.h
 *
 * @brief header file - ble transport
 *
 * Copyright © 2020 - 2023 ASR
 *
 *
 ****************************************************************************************
 */


#ifndef _SONATA_BLE_TRANS_H_
#define _SONATA_BLE_TRANS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_utils_api.h"



/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */





/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */



/*
 * Type Definition
 ****************************************************************************************
 */
typedef struct sonata_ble_transport_pad_config
{
    void (*pf_init_pad_config)(void);
    void (*pf_wakeup_pad_config)(void);
    void (*pf_flow_on_pad_config)(void);
    void (*pf_flow_off_pad_config)(void);
}sonata_ble_transport_pad_config_t;


/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */



/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern void sonata_ble_transport_init(sonata_ble_transport_pad_config_t *config);


extern void sonata_ble_transport_wkup(void);


#endif //_SONATA_BLE_TRANS_H_

