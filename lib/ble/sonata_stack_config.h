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

#ifndef SONATA_STACK_CONFIG_H_
#define SONATA_STACK_CONFIG_H_
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_stack_user_config.h"
#include "sonata_stack_fixed_config.h"


/*
 * DEFINES
 ****************************************************************************************
 */
//////////////////////////////////////////
/*user configurable*/
#ifdef SONATA_CFG_APP
#define CFG_APP
#endif //SONATA_CFG_APP

#ifdef SONATA_CFG_HOST
#define CFG_HOST

#define CFG_PRF

#if ( defined(CFG_PLF_SONATA) && defined(__ROM_COMPILE__) )
    #define CFG_CON                    11
#else
    #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
    #define CFG_CON                    10
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M) )
    #define CFG_CON                    7
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M_CUTDOWN) )
    #define CFG_CON                    6
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L2) )
        #define CFG_CON                    4
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L) )
        #define CFG_CON                    1
    #elif defined(CFG_PLF_DUET)
        #ifdef _VENDOR_MIDEA_
        #define CFG_CON                    11
        #else
        #define CFG_CON                    7
        #endif
    #else
        #define CFG_CON                    7
    #endif // #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
#endif

#if ( defined(CFG_BLE_NO_CENTRAL) )

#else
#define CFG_ATTC
#endif

#define CFG_ATTS

#define CFG_EXT_DB

#ifdef SONATA_CFG_BLE_TRANSPORT
#define CFG_AHITL
#endif //SONATA_CFG_BLE_TRANSPORT

#endif //SONATA_CFG_HOST

#ifdef SONATA_CFG_BLE_TRANSPORT
#define CFG_HCITL
#endif //SONATA_CFG_BLE_TRANSPORT

#ifdef SONATA_CFG_OTAS
#define CFG_PRF_OTAS
#endif //SONATA_CFG_OTAS

#ifdef SONATA_CFG_DIS
#define CFG_PRF_DISS
#endif //SONATA_CFG_DIS

#ifdef SONATA_CFG_BASS
#define CFG_PRF_BASS
#endif //SONATA_CFG_BASS

///HID Device Role
#ifdef SONATA_CFG_HOGPD
#define CFG_PRF_HOGPD
#endif //SONATA_CFG_HOGPD

///HID Boot Host Role
#ifdef SONATA_CFG_HOGPBH
#define CFG_PRF_HOGPBH
#endif //SONATA_CFG_HOGPBH

///HID Report Host Role
#ifdef SONATA_CFG_HOGPRH
#define CFG_PRF_HOGPRH
#endif //SONATA_CFG_HOGPRH


#ifdef SONATA_CFG_MESH
#define CFG_BLE_MESH

#define CFG_BLE_MESH_MSG_API

#define CFG_BLE_ADV_TEST_MODE

#ifndef CFG_BLE_MESH_L
#if ( (defined SONATA_CFG_MESH_DBG) && (defined SONATA_CFG_DBG) )
#define CFG_BLE_MESH_DBG
#endif //( (defined SONATA_CFG_MESH_DBG) && (defined SONATA_CFG_DBG) )

#define CFG_BLE_MESH_PROVISIONER

#define CFG_BLE_MESH_FRIEND

#define CFG_BLE_MESH_LPN

#define CFG_BLE_MESH_MDL_BATS

#define CFG_BLE_MESH_MDL_LOCS

#define CFG_BLE_MESH_MDL_PROPS

#define CFG_BLE_MESH_MDL_BATC

#define CFG_BLE_MESH_MDL_LOCC

#define CFG_BLE_MESH_MDL_PROPC
#endif

#define CFG_BLE_MESH_RELAY

#define CFG_BLE_MESH_GATT_PROXY

#define CFG_BLE_MESH_GATT_PROV

#define CFG_BLE_MESH_STORAGE_NVDS

#define CFG_BLE_MESH_MDL_SERVER

#define CFG_BLE_MESH_MDL_GENS

#define CFG_BLE_MESH_MDL_CLIENT

#define CFG_BLE_MESH_MDL_GENC

#define CFG_BLE_MESH_MDL_LIGHTS

#endif //SONATA_CFG_MESH

#ifdef SONATA_CFG_NVDS
#define CFG_NVDS
#endif //SONATA_CFG_NVDS

#ifdef SONATA_CFG_EFUSE
#define CFG_EFUSE
#endif //SONATA_CFG_EFUSE

#ifdef SONATA_CFG_CHIP_CHECK
#define CFG_CHIP_CHECK
#endif //SONATA_CFG_CHIP_CHECK

#ifdef SONATA_MESH_CFG_GATEWAY
#define CFG_GATEWAY
#endif //SONATA_MESH_CFG_GATEWAY

#ifdef SONATA_CFG_DISPLAY
#define CFG_DISPLAY
#endif //SONATA_CFG_DISPLAY

#ifdef SONATA_CFG_DBG
#define CFG_DBG

#define CFG_DBG_MEM

#define CFG_DBG_FLASH

#define CFG_DBG_STACK_PROF
#endif // SONATA_CFG_DBG

#ifdef SONATA_CFG_TRC
#define CFG_TRC_EN

#define CFG_TRC_ALL
#endif //SONATA_CFG_TRC


//****************************************************************************************EM Map Start
#if (ROM_BLUETOOTH_RESOURCE || APP_BLUETOOTH_RESOURCE || CORE_BLUETOOTH_RESOURCE)
#include "sonata_mem_config.h"

#define ROM_MAX_CFG_ACT               12
#define ROM_MAX_CFG_CON               11
#define ROM_MAX_CFG_WLIST             30
#define ROM_MAX_CFG_RAL               50
#define ROM_MAX_GAP_MAX_LE_MTU        2048
#define ROM_MAX_CFG_HL_HEAP_DB_SIZE   3072

#define VAR_CFG_ACT                   (pf_mem_config->ble_activity_max)
#define VAR_CFG_CON                   (pf_mem_config->ble_connection_max)
#define VAR_CFG_WLIST                 (pf_mem_config->ble_wlist_max)
#define VAR_CFG_RAL                   (pf_mem_config->ble_ral_max)
#define VAR_CFG_BLE_ACL_BUF_NB_TX     (pf_mem_config->ble_acl_buf_nb_tx)
#define VAR_CFG_BLE_ADV_BUF_NB_TX     (pf_mem_config->ble_adv_buf_nb_tx)
#define VAR_CFG_BLE_TXDESC            (pf_mem_config->ble_tx_desc_nb)
#define VAR_CFG_BLE_RXDESC            (pf_mem_config->ble_rx_desc_nb)
#define VAR_GAP_MAX_LE_MTU            (pf_mem_config->gap_max_le_mtu)
#define VAR_CFG_HL_HEAP_DB_SIZE       (pf_mem_config->hl_heap_db_size)


#define CUSTOMER_CFG_ACT             (SONATA_CFG_ACT)
#define CUSTOMER_CFG_CON             (SONATA_CFG_CON)
#define CUSTOMER_CFG_WLIST           (SONATA_CFG_WLIST)
#define CUSTOMER_CFG_RAL             (SONATA_CFG_RAL)
#define CUSTOMER_CFG_RX_DESC_NB      (SONATA_CFG_RX_DESC_NB)
#define CUSTOMER_GAP_MAX_LE_MTU      (SONATA_GAP_MAX_LE_MTU)
#define CUSTOMER_CFG_HL_HEAP_DB_SIZE (SONATA_CFG_HL_HEAP_DB_SIZE)


#if(ROM_MAX_CFG_ACT < CFG_ACT)
#error "CFG_ACT is too big."
#endif

#if(ROM_MAX_CFG_CON < CFG_CON)
#error "CFG_CON is too big."
#endif

#if(ROM_MAX_CFG_RAL < CFG_RAL)
#error "CFG_RAL is too big."
#endif


#endif
//****************************************************************************************EM Map end

#endif /*SONATA_STACK_CONFIG_H_*/

