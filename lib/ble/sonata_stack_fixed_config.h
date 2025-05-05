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

#ifndef SONATA_STACK_FIXED_CONFIG_H_
#define SONATA_STACK_FIXED_CONFIG_H_


/*
 * DEFINES
 ****************************************************************************************
 */
//////////////////////////////////////////
/*do not alter*/
#define CFG_BLE

#define CFG_EMB


#if defined(CFG_BLE_NO_CENTRAL) || defined(CFG_DYNA_HL_LL_PERIPHERAL_ONLY)
    #define CFG_BROADCASTER
    #define CFG_PERIPHERAL
#else
    #define CFG_ALLROLES
#endif // #if defined(CFG_BLE_NO_CENTRAL)

#if ( defined(CFG_PLF_SONATA) && defined(__ROM_COMPILE__) )
    #define CFG_ACT            12
#else
    #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
    #define CFG_ACT            11
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M) )
    #define CFG_ACT            8
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M_CUTDOWN) )
    #define CFG_ACT            7
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L2) )
        #define CFG_ACT            5
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L) )
        #define CFG_ACT            2
    #elif defined(CFG_PLF_DUET)
        #define CFG_ACT            12
    #else
        #define CFG_ACT            8
    #endif // #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
#endif



#if ( defined(CFG_PLF_SONATA) && defined(__ROM_COMPILE__) )
    #define CFG_RAL            50
#else
    #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
    #define CFG_RAL            10
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M) )
    #define CFG_RAL            10
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M_CUTDOWN) )
    #define CFG_RAL            5
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L2) )
        #define CFG_RAL            5
    #elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L) )
        #define CFG_RAL            1
    #elif defined(CFG_PLF_DUET)
        #define CFG_RAL            50
    #else
        #define CFG_RAL            10
    #endif // #if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
#endif


#if defined (CFG_BLE_HL_LL_ROM_M_CUTDOWN) || defined (CFG_DYNA_HL_LL_PERIPHERAL_ONLY)
#else
    #define CFG_CON_CTE_REQ
    #define CFG_CON_CTE_RSP
    #define CFG_CONLESS_CTE_TX
    #define CFG_CONLESS_CTE_RX
    #define CFG_AOD
    #define CFG_AOA
    #define CFG_WLAN_COEX
#endif

#if ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_H) )
    #define CFG_NB_PRF         10
#elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M) )
    #define CFG_NB_PRF         10
#elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_M_CUTDOWN) )
    #define CFG_NB_PRF         5
#elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L2) )
    #define CFG_NB_PRF         10
#elif ( defined(CFG_PLF_SONATA) && defined(CFG_BLE_HL_LL_ROM_L) )
    #define CFG_NB_PRF         10
#elif defined(CFG_PLF_DUET)
    #define CFG_NB_PRF         10
#else
    #define CFG_NB_PRF         10
#endif


#endif // #if defined(CFG_PLF_SONATA)



//****************************************************************************************EM Map Start
//#define __ROM_COMPILE__  //For Demo
//#define __APP_COMPILE__  //For Demo
//#define __CORE_COMPILE__  //For Demo
#if defined (__ROM_COMPILE__)
    #define ROM_BLUETOOTH_RESOURCE  1
    #define CORE_BLUETOOTH_RESOURCE 0
    #define APP_BLUETOOTH_RESOURCE  0
#elif defined(__CORE_COMPILE__)
    #define ROM_BLUETOOTH_RESOURCE  0
    #define CORE_BLUETOOTH_RESOURCE 1
    #define APP_BLUETOOTH_RESOURCE  0
#elif defined(__APP_COMPILE__)
    #define ROM_BLUETOOTH_RESOURCE  0
    #define CORE_BLUETOOTH_RESOURCE 0
    #define APP_BLUETOOTH_RESOURCE  1
#elif defined(__ASR_DEBUG__)
    #define ROM_BLUETOOTH_RESOURCE  1
    #define CORE_BLUETOOTH_RESOURCE 1
    #define APP_BLUETOOTH_RESOURCE  1
#else
    #define ROM_BLUETOOTH_RESOURCE  0
    #define CORE_BLUETOOTH_RESOURCE 0
    #define APP_BLUETOOTH_RESOURCE  0
#endif
//****************************************************************************************EM Map End

