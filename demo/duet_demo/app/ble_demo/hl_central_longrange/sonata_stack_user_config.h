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
#ifndef SONATA_STACK_USER_CONFIG_H_
#define SONATA_STACK_USER_CONFIG_H_

/*
 * DEFINES
 ****************************************************************************************
 */
#define SONATA_CFG_CONTROLLER

#define SONATA_CFG_HOST

#define SONATA_CFG_APP

#ifdef CFG_MRFOTA_TEST
#define SONATA_CFG_BLE_TRANSPORT
#endif
//#define SONATA_CFG_BLE_DIRECTION_FINDING

//#define SONATA_CFG_BLE_WLAN_COEX

//#define SONATA_CFG_MESH

#define SONATA_CFG_DIS

//#define SONATA_CFG_BASS

//#define SONATA_CFG_OTAS

#define SONATA_CFG_DBG

//#define SONATA_CFG_MESH_DBG

//#define SONATA_CFG_TRC

//#define SONATA_CFG_DISPLAY

//#define SONATA_CFG_NVDS


#endif /*SONATA_STACK_USER_CONFIG_H_*/

