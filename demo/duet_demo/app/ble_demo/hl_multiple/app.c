/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) ASR 2020-2029
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#include "sonata_ble_hook.h"
#include "sonata_log.h"
#include "data_transport_svc.h"
#include "app_nv.h"
#include "app_command.h"
#include "app_utils.h"
#include "app_uart.h"
#include "app_patch.h"
#include "app_platform.h"

#if BLE_BATT_SERVER
#include "sonata_prf_bass_api.h"
#endif //BLE_BATT_SERVER

/*
 * DEFINES
 ****************************************************************************************
 */

#define SOFTWARE_VERSION "app-1.0.0-20201015.1100"
SYS_APP_VERSION_SEG const char app_version[] = SOFTWARE_VERSION;
/*!
 * @brief save local handle start index
 */
static uint16_t custom_svc_start_handle = 0;
//Mark for profile dis
static bool prf_add_diss = false;
static bool prf_add_bass = false;
static bool prf_add_ota = false;
static bool prf_add_dts = false;
static att_db_item gAttDb[APP_ATT_DB_ITEM_MAX] = {0};

uint32_t gTimeStamp = 0;
static bool gExtentAdv = false;
static bool gPeroidAdv = false;
static bool gDirectAdv = false;
static uint8_t gWorkCounter = 0;
static uint8_t gWorkFlag = 0;
//uint8_t targetAddr1[SONATA_GAP_BD_ADDR_LEN] = {0x13, 0x71, 0xDA, 0x7D, 0x1A, 0x00};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
typedef struct app_env_t{

    uint8_t gAppStatus;
    uint16_t attrHandle;
    uint16_t targetWriteHandle;
    uint16_t targetReadHandle;
    uint16_t targetNtfHandle;
    app_uuids appUuids;
    actives act[APP_ACTIVE_MAX];
    uint8_t advSid;
    uint8_t connection_id;
    uint8_t gSendDataLock;
    bool bAdvWithScanResponse;
    bool bWhiteListEnable;
    bool bPalEnable;
    bool bContinuousScan;
    uint8_t gDtsOn;
    uint8_t targetAddr[6];
    bool bTransEnable;
}app_env;

static app_env gAppEnv;

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * EXTERNAL FUNCTION DECLARATION
 ****************************************************************************************
 */

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static void app_ble_print_info(bool adv);

void app_active_update(uint8_t conIdx, uint8_t* mac)
{
    if (conIdx >= APP_ACTIVE_MAX)
    {
        APP_TRC_HIGH("APP: %s,ERROR:ACTIVE overflow\r\n",__FUNCTION__ );
        return;
    }
    if (gAppEnv.act[conIdx].runing == true)
    {
        APP_TRC_HIGH("APP: %s,ERROR:ACTIVE id error\r\n",__FUNCTION__ );
        return;
    }
    gAppEnv.act[conIdx].runing = true;
    gAppEnv.act[conIdx].assign_id = conIdx;
    addrCopy(gAppEnv.act[conIdx].peer, mac);
}

void app_active_delete(uint8_t conIdx)
{
    if (conIdx >= APP_ACTIVE_MAX)
    {
        APP_TRC_HIGH("APP: %s,ERROR:ACTIVE overflow\r\n",__FUNCTION__ );
        return;
    }
    gAppEnv.act[conIdx].runing = false;

}

actives *app_get_active()
{
    return gAppEnv.act;
}

void app_ble_config_legacy_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t own_addr_type = SONATA_GAP_STATIC_ADDR;
    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0;
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    //    msg->adv_param.adv_param.peer_addr.addr.addr:00
    param.addr_type = SONATA_ADDR_PUBLIC;
    param.adv_intv_min = 100;
    param.adv_intv_max = 500;
    param.chnl_map = 0x07;
    param.phy = SONATA_GAP_PHY_LE_1MBPS;
    #ifdef DEMO_RANDOM_ADDRESS
        param.addr_type = SONATA_ADDR_RAND;
        own_addr_type = SONATA_GAP_STATIC_ADDR;
        #ifdef DEMO_RANDOM_RESLOVE_ADDRESS
            own_addr_type = SONATA_GAP_GEN_RSLV_ADDR;
        #endif
    #endif
    uint16_t ret = sonata_ble_config_legacy_advertising(own_addr_type, &param);//Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_ADV;
    }
    gExtentAdv = false;

    //Demo for adv update --Start
    //sonata_api_app_timer_set(APP_TIMER_ADV_DATA, 5000);
    //Demo for adv update --End

}
void app_ble_config_direct_legacy_advertising(bool highDuty, bool addrType, uint8_t *address)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    #if (USE_APP_SEC)
    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_NON_DISC;
    param.prop = highDuty ? SONATA_GAP_ADV_PROP_DIR_CONN_HDC_MASK : SONATA_GAP_ADV_PROP_DIR_CONN_LDC_MASK;
    param.adv_intv_min = 32; //(in unit of 625us)
    param.adv_intv_max = 32; //(in unit of 625us)
    param.chnl_map = 0x07;
    param.phy = SONATA_GAP_PHY_LE_1MBPS;
    if (address != NULL)
    {
        addrCopy(param.addr.addr, address);
    }
    else if (app_nv_get_bonded() == true)
    {
        bonded_dev_info_list_t bondList = {0};
        app_nv_get_bonded_device_info(&bondList);
        bonded_dev_info_t *bondedDevInfo = &bondList.devInfo[0];
        addrCopy(param.addr.addr, bondedDevInfo->peer_addr);
    }
    else
    {
        param.addr.addr[0] = 0xA1;
        param.addr.addr[1] = 0xB1;
        param.addr.addr[2] = 0xC1;
        param.addr.addr[3] = 0xD1;
        param.addr.addr[4] = 0xE1;
        param.addr.addr[5] = 0xF1;
    }
    param.addr_type = addrType; //SONATA_ADDR_RAND : SONATA_ADDR_PUBLIC
    APP_TRC("APP: Direct ADV addr[type=%d]:%02X %02X %02X %02X %02X %02X\r\n", param.addr_type,
            param.addr.addr[0], param.addr.addr[1], param.addr.addr[2], param.addr.addr[3], param.addr.addr[4], param.addr.addr[5]);
    if (addrValid(param.addr.addr) == false)
    {
        APP_TRC_ERROR("APP: Addr Error\r\n");
    }
    uint16_t ret = sonata_ble_config_legacy_advertising(SONATA_GAP_STATIC_ADDR, &param);//Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_ADV;
    }
    gDirectAdv = true;
    #endif
}


void app_ble_config_extended_advertising()
{
    APP_TRC("APP: %s  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n", __FUNCTION__);
    sonata_gap_extended_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_EXT_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0xE2;//Todo
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    param.prim_adv_intv_min = 32;
    param.prim_adv_intv_max = 32;
    param.prim_chnl_map = 0x07;
    param.prim_phy = SONATA_GAP_PHY_1MBPS;
    param.second_max_skip = 0;
    param.second_phy = SONATA_GAP_PHY_CODED;
    param.second_adv_sid = 0; //Todo

    uint16_t ret = sonata_ble_config_extended_advertising(SONATA_GAP_STATIC_ADDR, &param);//Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_ADV;
    }
    gExtentAdv = true;
}

void app_ble_config_periodic_advertising()
{
    APP_TRC("APP: %s  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n", __FUNCTION__);
    sonata_gap_periodic_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_EXT_ADV_PROP_NON_CONN_NON_SCAN_MASK; //Not Connectable, anonymous, scannable
    param.max_tx_pwr = 0xE2;//Todo
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    param.prim_adv_intv_min = 0x00A0;
    param.prim_adv_intv_max = 0x0140;
    param.prim_chnl_map = 0x07;
    param.prim_phy = SONATA_GAP_PHY_1MBPS;
    param.second_max_skip = 0;
    param.second_phy = SONATA_GAP_PHY_1MBPS;
    param.second_adv_sid = 0; //Todo
    param.period_adv_intv_min = 32;
    param.period_adv_intv_max = 32;
    param.period_switching_pattern_len = 0;
    param.period_antenna_id[0] = 0;
    param.period_cte_count = 0;
    param.period_cte_type = 0;
    param.period_cte_len = 0;

    uint16_t ret = sonata_ble_config_periodic_advertising(SONATA_GAP_STATIC_ADDR, &param);
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_ADV;
    }
    gPeroidAdv = true;

}

void app_ble_config_period_sync(uint8_t type)
{
    APP_TRC("APP: %s \r\n", __FUNCTION__);
    if (type == SONATA_GAP_PER_SYNC_TYPE_SELECTIVE)
    {
        gAppEnv.bPalEnable = true;
    }
    else
    {
        gAppEnv.bPalEnable = false;
    }
    uint16_t ret = sonata_ble_config_period_sync(SONATA_GAP_STATIC_ADDR);
    //Next event:SONATA_GAP_CMP_SCANNING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_PERIODSYNC;
    }

}

void app_ble_config_scanning(uint8_t type, uint8_t *target)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    if (type == SONATA_GAP_SCAN_TYPE_SEL_OBSERVER)
    {
        gAppEnv.bWhiteListEnable = true;
    }
    else if (type == SONATA_GAP_SCAN_TYPE_OBSERVER)
    {
        gAppEnv.bContinuousScan = true;
    }
    else
    {
        gAppEnv.bWhiteListEnable = false;
        gAppEnv.bContinuousScan = false;
    }
    if (target != NULL)
    {
        addrCopy(gAppEnv.targetAddr, target);
    }
    else
    {
        memset(gAppEnv.targetAddr, 0, SONATA_GAP_BD_ADDR_LEN);
    }
    uint16_t ret = sonata_ble_config_scanning(SONATA_GAP_STATIC_ADDR);
    //Next event:SONATA_GAP_CMP_SCANNING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_SCAN;
    }

}


void app_ble_config_initiating(uint8_t type, uint8_t *address)
{
    if (address != NULL)
    {
        addrCopy(gAppEnv.targetAddr, address);
    }
    else
    {
        memset(gAppEnv.targetAddr, 0, SONATA_GAP_BD_ADDR_LEN);
    }
    APP_TRC_HIGH("APP: %s, addr:%02X %02X %02X %02X %02X %02X\r\n", __FUNCTION__,
           gAppEnv.targetAddr[0], gAppEnv.targetAddr[1], gAppEnv.targetAddr[2], gAppEnv.targetAddr[3], gAppEnv.targetAddr[4], gAppEnv.targetAddr[5]);

    if (type != 0)
    {
        gAppEnv.bWhiteListEnable = true;
    }
    //Call api to config init
    uint8_t own_addr_type = SONATA_GAP_STATIC_ADDR;
    #ifdef DEMO_RANDOM_RESLOVE_ADDRESS
    own_addr_type = SONATA_GAP_GEN_RSLV_ADDR;
    #endif
    uint16_t ret = sonata_ble_config_initiating(own_addr_type);
    //Next event:SONATA_GAP_CMP_INITIATING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    else
    {
        gAppEnv.gAppStatus = STATUS_INITIATING;
    }
}

static void app_ble_set_scan_response_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    uint8_t advData[] = { //Advertising data format
            3, SONATA_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID, 0x12, 0x18,
            3, SONATA_GAP_AD_TYPE_APPEARANCE, 0xC1, 0x03        //0x03C1: HID Keyboard
    };
    uint16_t ret = sonata_ble_set_scan_response_data(sizeof(advData), advData);
    gAppEnv.bAdvWithScanResponse = 0;
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief set advertising data
 */
static void app_ble_set_adv_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t dev_name_saved[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
    app_nv_get_local_device_name(dev_name_saved, APPNV_LEN_LOCAL_DEVICE_NAME, true);
    uint8_t nameLen = dev_name_saved[0];
    uint8_t advData[30] = {0};
    memset(advData, ' ', 30);
    advData[0] = nameLen + 1;
    advData[1] = SONATA_GAP_AD_TYPE_COMPLETE_NAME;
    sprintf((char *) (advData + 2), "%s", &dev_name_saved[1]);

    advData[nameLen + 2] = 3;
    if (app_nv_get_bond_type_value() == 0)
    {
        advData[nameLen + 3] = SONATA_GAP_AD_TYPE_APPEARANCE;
        advData[nameLen + 4] = 0xC1;
        advData[nameLen + 5] = 0x03;
    }
    else
    {
        advData[nameLen + 3] = SONATA_GAP_AD_TYPE_MANU_SPECIFIC_DATA;
        advData[nameLen + 4] = 0xFF;
        advData[nameLen + 5] = 0xAA;
    }

    APP_TRC("APP: %s ,adv[%d]:%s:[", __FUNCTION__, nameLen + 6, advData);
    for (int i = 0; i < nameLen + 6; ++i)
    {
        printf("%02X ", advData[i]);
    }
    printf("]\r\n");

    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(nameLen + 6, advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

void app_ble_update_adv_data(uint16_t counter)
{
    gWorkFlag = 1;

    uint8_t totalLen =  11;
    uint8_t advData[20] = { //Advertising data format
            10, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A','S','R','-','0','0','0','0','0'
    };

    sprintf((char *)&advData[6], "%05d", counter);
    printf("APP: %s ,adv[%d]:%s:[", __FUNCTION__, totalLen, advData);
    for (int i = 0; i < totalLen; ++i)
    {
        printf("%02X ", advData[i]);
    }
    printf("]\r\n");
    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(totalLen, advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }

}

static void app_ble_set_extended_adv_data()
{
    #define FIXED_LEN1 (227)
    uint8_t advData[FIXED_LEN1 + 2 + APPNV_LEN_LOCAL_DEVICE_NAME + 2] = { //Advertising data format,Demo for long adv data
            APPNV_LEN_LOCAL_DEVICE_NAME + 1, SONATA_GAP_AD_TYPE_COMPLETE_NAME, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',
            FIXED_LEN1 + 1, SONATA_GAP_AD_TYPE_MANU_SPECIFIC_DATA,
            '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '2', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '3', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '4', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '5', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '6', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '7', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '8', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '9', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '0', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '2', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '3', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '4', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '5', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '6', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '7', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '8', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',            '9', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '0', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '2', '2', '3', '4', '5', '6', '7', '8', '9', '0',            '3', 'B', 'C', 'D', 'E', 'F', 'G',
    };
    APP_TRC("APP: %s , length=%d \r\n", __FUNCTION__, sizeof(advData));
    if (sizeof(advData) > (254-9-3))
    {
        //Length should less than  242
        return;
    }
    uint8_t dev_name_saved[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
    app_nv_get_local_device_name(dev_name_saved, APPNV_LEN_LOCAL_DEVICE_NAME, true);
    memcpy(advData + 2, dev_name_saved + 1, dev_name_saved[0]);

    //Call API
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    //sonata_ble_set_scan_response_data(dev_name_saved[0], dev_name_saved + 1);
}


static void app_ble_set_periodic_adv_data(uint8_t demoData)
{
    #define FIXED_LEN (100)
    uint8_t advData[FIXED_LEN + 2 + APPNV_LEN_LOCAL_DEVICE_NAME + 2] = {
            APPNV_LEN_LOCAL_DEVICE_NAME + 1, SONATA_GAP_AD_TYPE_COMPLETE_NAME, '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            FIXED_LEN + 1, SONATA_GAP_AD_TYPE_MANU_SPECIFIC_DATA,
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
            '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    } ;
    APP_TRC("APP: %s  advdata len=%d, demoData=%d\r\n", __FUNCTION__, sizeof(advData), demoData);
    uint8_t dev_name_saved[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
    app_nv_get_local_device_name(dev_name_saved, APPNV_LEN_LOCAL_DEVICE_NAME, true);
    memcpy(advData + 2, dev_name_saved + 1, dev_name_saved[0]);
    for (int i = 0; i < 20; ++i)
    {
        advData[15 + i] = demoData;
    }
    APP_TRC("APP:  ------PeroidAdv Name:%s; Data:%02X %02X %02X %02X %02X------\r\n",
            dev_name_saved+1, advData[16],advData[17],advData[18],advData[19],advData[20]);

    //Call API
    uint16_t ret = sonata_ble_set_period_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}



static void app_ble_start_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //Call api
    uint16_t ret = sonata_ble_start_advertising(0, 0);
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

void app_ble_stop_advertising()
{
    APP_TRC("APP: %s  . ADV Stop <\r\n", __FUNCTION__);
    sonata_ble_stop_advertising();
}

void app_ble_stop_initiating()
{
    APP_TRC("APP: %s  . Init Stop <\r\n", __FUNCTION__);
    sonata_ble_stop_initiating();
}

void app_ble_stop_scanning()
{
    APP_TRC("APP: %s  . Scan Stop <\r\n", __FUNCTION__);
    sonata_ble_stop_scanning();
}

void app_ble_stop_period_sync()
{
    APP_TRC("APP: %s  . ADV Stop <\r\n", __FUNCTION__);
    sonata_ble_stop_period_sync();
}

static void app_ble_start_scanning()
{
    APP_TRC("APP: %s  gAppEnv.bWhiteListEnable=%d\r\n", __FUNCTION__,gAppEnv.bWhiteListEnable);
    //bFound = false;
    sonata_gap_scan_param_t param = {0};
    param.type = SONATA_GAP_SCAN_TYPE_GEN_DISC;
    // For continuous scan, use OBSERVER type, use duration to control scan timeer.
    // if duration=0, will scan for ever until sonata_ble_stop_scanning() called
    if (gAppEnv.bContinuousScan == true)
    {
        param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;
    }
    else if (gAppEnv.bWhiteListEnable == true)
    {
        param.type = SONATA_GAP_SCAN_TYPE_SEL_OBSERVER;
    }
    param.prop = SONATA_GAP_SCAN_PROP_ACTIVE_1M_BIT | SONATA_GAP_SCAN_PROP_PHY_1M_BIT;//0x05
    if (DEMO_LONG_RANGE)
    {
        APP_TRC("APP: %s  >>>>>>>>>>>>>>>>>>>>>>>>>>>>Long Range\r\n", __FUNCTION__);
        param.prop = SONATA_GAP_SCAN_PROP_PHY_CODED_BIT | SONATA_GAP_SCAN_PROP_ACTIVE_CODED_BIT;
    }
    param.dup_filt_pol = SONATA_GAP_DUP_FILT_EN;
//    param.scan_param_1m.scan_intv = 0x0140;
//    param.scan_param_1m.scan_wd = 0x00A0;
//    param.scan_param_coded.scan_intv = 0x0140;
//    param.scan_param_coded.scan_wd = 0x00A0;
    param.scan_param_1m.scan_intv = 64;
    param.scan_param_1m.scan_wd = 64;
    param.scan_param_coded.scan_intv = 64;
    param.scan_param_coded.scan_wd = 64;
    param.duration = 0;
    param.period = 0;
    app_cmd_update_scan_param(&param);
    uint16_t ret = sonata_ble_start_scanning(&param);
    //Scan result will show in app_gap_scan_result_callback()
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }

}

static void app_ble_start_initiating(uint8_t *target)
{
    APP_TRC("APP: %s  target:%02X %02X %02X %02X %02X %02X\r\n", __FUNCTION__,target[0],target[1],target[2],target[3],target[4],target[5]);
//    if (app_ble_check_address(target) == false)
//    {
//        APP_TRC("APP: %s, Target address is not right. Stop\r\n", __FUNCTION__);
//        return;
//    }
    sonata_gap_init_param_t param = {0};
    param.type = SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST;
    if (gAppEnv.bWhiteListEnable == true)
    {
        param.type = SONATA_GAP_INIT_TYPE_AUTO_CONN_EST;
    }
    param.prop = SONATA_GAP_INIT_PROP_1M_BIT | SONATA_GAP_INIT_PROP_2M_BIT | SONATA_GAP_INIT_PROP_CODED_BIT;
    param.prop = SONATA_GAP_INIT_PROP_1M_BIT;
    param.conn_to = 0;
    param.peer_addr.addr_type = SONATA_GAP_STATIC_ADDR; //  Addr
    addrCopy(param.peer_addr.addr.addr, target);

    if (param.prop & SONATA_GAP_INIT_PROP_1M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_1M_BIT \r\n", __FUNCTION__, param.prop);
        param.scan_param_1m.scan_intv = 64;
        param.scan_param_1m.scan_wd = 64;
        param.conn_param_1m.conn_intv_min = 100;
        param.conn_param_1m.conn_intv_max = 500;
        param.conn_param_1m.conn_latency = 6;
        param.conn_param_1m.supervision_to = 1000;
        param.conn_param_1m.ce_len_min = 0x0008;
        param.conn_param_1m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_2M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_2M_BIT \r\n", __FUNCTION__, param.prop);
        param.conn_param_2m.conn_intv_min = 0x0028;
        param.conn_param_2m.conn_intv_max = 0x0028;
        param.conn_param_2m.conn_latency = 0;
        param.conn_param_2m.supervision_to = 0x02BC;
        param.conn_param_2m.ce_len_min = 0x0008;
        param.conn_param_2m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_CODED_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_CODED_BIT \r\n", __FUNCTION__, param.prop);
        param.scan_param_coded.scan_intv = 0x0200;
        param.scan_param_coded.scan_wd = 0x0100;
        param.conn_param_coded.conn_intv_min = 0x0028;
        param.conn_param_coded.conn_intv_max = 0x0028;
        param.conn_param_coded.conn_latency = 0;
        param.conn_param_coded.supervision_to = 0x02BC;
        param.conn_param_coded.ce_len_min = 0x0008;
        param.conn_param_coded.ce_len_max = 0x0008;
    }

    uint16_t ret = sonata_ble_start_initiating(&param);
    //Next event:If connected, SONATA_GAP_CMP_INITIATING_DELETE event will be received
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


static void app_ble_start_period_sync(uint8_t * addr)
{
    APP_TRC("APP: %s, addr:%02X %02X %02X %02X %02X %02X\r\n", __FUNCTION__, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    sonata_gap_per_sync_param_t param = {0};
    param.type =  SONATA_GAP_PER_SYNC_TYPE_GENERAL;
    if (gAppEnv.bPalEnable == true)
    {
        param.type = SONATA_GAP_PER_SYNC_TYPE_SELECTIVE;
    }
    param.skip = 0;
    param.sync_to = 100;
    param.report_disable = false;
    param.cte_type = 0;

    param.adv_addr.adv_sid = 0;
    param.adv_addr.addr.addr_type = 0;
    addrCopy(param.adv_addr.addr.addr.addr, addr);

    uint16_t ret = sonata_ble_start_period_sync(&param);
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }

}

void app_ble_discovery_svc(uint8_t idx, uint8_t *uuid)
{
    app_nv_get_uuid_value(&gAppEnv.appUuids);
    if (uuid == NULL)
    {
        APP_TRC("APP: %s  \r\n", __FUNCTION__);
        sonata_ble_gatt_disc_all_svc(idx);//--->app_gatt_disc_svc_callback()
    }
    else
    {
        APP_TRC("APP: %s  uuid:%02X %02X \r\n", __FUNCTION__,uuid[0],uuid[1]);
        sonata_ble_gatt_disc_svc_by_uuid(idx, 0, 0xFFFF, SONATA_ATT_UUID_16_LEN, uuid);
    }
}

void app_ble_discovery_char(uint8_t idx, uint8_t *uuid)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    app_nv_get_uuid_value(&gAppEnv.appUuids);//
    if (uuid == NULL)
    {
        sonata_ble_gatt_disc_all_characteristic(idx, 1, 0xFFFF);
    }
    else
    {
        sonata_ble_gatt_disc_characteristic_by_uuid(idx, 1, 0xFFFF, SONATA_ATT_UUID_16_LEN, uuid);
    }
}

void app_ble_discovery_desc(uint8_t idx)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    app_nv_get_uuid_value(&gAppEnv.appUuids);//
    sonata_ble_gatt_disc_all_descriptor(idx, 1, 0xFFFF);
}

static void app_ble_print_info(bool adv)
{
    //Print device info
    uint8_t dev_name_saved[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
    app_nv_get_local_device_name(dev_name_saved, APPNV_LEN_LOCAL_DEVICE_NAME, true);
    printf("\r\n----BLE NAME[%s] ----\r\n", dev_name_saved + 1);
    uint8_t *addr = sonata_get_bt_address();
    printf("----BLE MAC[%02X:%02X:%02X:%02X:%02X:%02X] ----\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    printf("----DTS UUID[Sev:FF50 Read:FF51 Write:FF51 NTF:FF51]  ----\r\n");
    printf("----AUTONTF:%d\r\n",app_nv_get_autontf());

    if (adv)
    {
        printf("--------------------Advertising--------------------\r\n");
    }
    printf("\r\n");

}

static void app_ble_print_peer_info(uint8_t * name, uint16_t nameLen, uint8_t*mac)
{
    printf("\r\n----PEER BLE Connected ----");
    printf("\r\n----PEER BLE NAME[%s] ----\r\n", name);
    printf("----PEER BLE MAC[%02X:%02X:%02X:%02X:%02X:%02X] ----\r\n",
           mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    printf("\r\n");

}

#ifdef DEMO_RANDOM_RESLOVE_ADDRESS
static void app_ble_gen_rand_addr()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t prand[SONATA_GAP_ADDR_PRAND_LEN] = {0x01,0x01,0xDF};
    sonata_ble_gap_generate_random_address(SONATA_BD_ADDR_STATIC, prand);
}
#endif
/*!
 * @brief
 */
static void app_ble_on()
{

    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    gAppEnv.connection_id = 0XFF;

    sonata_gap_set_dev_config_cmd cmd = {0};
    cmd.role = APP_BLE_ROLE;
    cmd.gap_start_hdl = 0;
    cmd.gatt_start_hdl = 0;
    cmd.renew_dur = 0x0096;
    cmd.privacy_cfg = 0;
    #ifdef DEMO_RANDOM_ADDRESS
    {
        uint8_t customAddress[SONATA_GAP_BD_ADDR_LEN] = {0x44, 0x55, 0x44, 0x55, 0x44, 0x55};
        memcpy(cmd.addr.addr, &customAddress[0], SONATA_GAP_BD_ADDR_LEN);
        cmd.privacy_cfg = SONATA_GAP_PRIV_CFG_PRIV_ADDR_BIT;
        #ifdef DEMO_RANDOM_RESLOVE_ADDRESS
        cmd.privacy_cfg = SONATA_GAP_PRIV_CFG_PRIV_ADDR_BIT;
        #endif
    }
    #endif
    cmd.pairing_mode = SONATA_GAP_PAIRING_SEC_CON|SONATA_GAP_PAIRING_LEGACY;
    cmd.att_cfg = 0x0080 | SONATA_GAP_ATT_NAME_PERM_MASK;
    cmd.max_mtu = APP_MTU;
    cmd.max_mps = 0x02A0;
    cmd.max_nb_lecb = 0x0A;
    cmd.hl_trans_dbg = false;
    gAppEnv.bTransEnable = app_nv_get_hl_trans_value();
    if(gAppEnv.bTransEnable == true)
    {
        cmd.hl_trans_dbg = true;
        printf("---------------------------------------------\r\n");
        printf("          BLE_TRANSPORT ON \r\n");
        printf("---------------------------------------------\r\n");
    }
    //cmd.pa_en = SONATA_PA_622;
    uint16_t ret = sonata_ble_on(&cmd);//Next event:SONATA_GAP_CMP_BLE_ON
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
    app_ble_print_info(0);

    #if (DEMO_ROM_PATCH)
        app_patch_test();
        assert_err(888, 1);
        assert_err(888, 0);
    #endif
}

/*!
 * @brief add profiles
 */
static void app_add_profiles()
{
    //Add dis profile for example, SONATA_GAP_CMP_PROFILE_TASK_ADD will received when added
    #if DEMO_PRF_DISS
    if (prf_add_diss == false)
    {
        sonata_prf_diss_add_dis();
    }
    #endif
    #if DEMO_PRF_BASS
    if (prf_add_bass == false)
    {
        sonata_prf_bas_add_bass(1,false);
    }
    #endif
    #if DEMO_PRF_OTA
    if (prf_add_ota == false)
    {
        sonata_prf_otas_add_otas();
    }
    #endif
    if (prf_add_dts == false)
    {
        dts_add();
    }


}

/*
 * LOCAL FUNCTION DEFINITIONS    Callback functions
 ****************************************************************************************
 */

void app_ble_auto_conn()
{
    uint8_t value[APPNV_LEN_AUTOCONN] = {0};
    if (app_nv_get_autoconn(value, true) == true)
    {
        if (value[0] == true)
        {
            printf("APP: %s ,Start Auto Conn, \r\n", __FUNCTION__);
            memcpy(gAppEnv.targetAddr, &value[1], 6);
            app_ble_config_initiating(0, gAppEnv.targetAddr);
        }
    }
}


/*!
 * @brief BLE complete event handler
 * @param opt_id @see sonata_ble_complete_type
 * @param status complete status
 * @param param param and deparam will be difference according to difference complete event.
 * @param dwparam param and deparam will be difference according to difference complete event.
 * @return
 */
static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    if (app_command_get_cmdid() != ID_THROUGHPUT)
    {
        APP_TRC_HIGH("ACP: %s  opt_id=%04X,status=%02X,param=%04X,dwparam=%lu\r\n", __FUNCTION__, opt_id, status, param, dwparam);
    }
    if(gAppEnv.bTransEnable == true)
    {
        return CB_DONE;
    }

   if (status != 0)
    {
        APP_TRC_ERROR("ERR: %s  **********Operation[%04X] Fail. status:%02X(X)\r\n", __FUNCTION__, opt_id, status);
        sonata_api_app_timer_clear(APP_TIMER_ADV_DATA);
        return CB_DONE;
    }
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
        {
            app_add_profiles();
            uint8_t autoAdv = app_nv_get_autoadv();
            if (autoAdv == 1)
            {
                app_ble_config_legacy_advertising();

            }
            else if (autoAdv == 2)
            {
                app_ble_config_extended_advertising();
            }
            #if (USE_APP_SEC)
            app_sec_set_local_irk(app_sec_get_local_irk());
            #endif
//            {
//                extern void app_cmd_data_adapter(uint16_t length, const uint8_t *value);
//                char *cmd = "AT+MCONN=111100001194,111100001195,111100001196,111100001197,111100001198,111100001199,111100001200,111100001157,111100001115";
//                app_cmd_data_adapter(strlen(cmd),(uint8_t *)cmd);
//            }
            app_ble_auto_conn();
        }
            break;
        //Adv---------------------------------------------------
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            if (gExtentAdv == true)
            {
                app_ble_set_extended_adv_data();
            }
            else if (gDirectAdv == true)
            {
                app_ble_start_advertising();
            }
            else
            {
                app_ble_set_adv_data();
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_START ://0x0F06
            APP_TRC("ACP: %s  SONATA_GAP_CMP_ADVERTISING_START activity=%d gPeroidAdv=%d\r\n", __FUNCTION__, param, gPeroidAdv);
            app_ble_print_info(true);
            if (gPeroidAdv == true)
            {
                sonata_api_app_timer_set(APP_TIMER_ADV_DATA, 1000);
            }
            break;
        case SONATA_GAP_CMP_ADVERTISING_STOP:
            sonata_api_app_timer_clear(APP_TIMER_ADV_DATA);
            break;
        case SONATA_GAP_CMP_ADVERTISING_DELETE:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_ADVERTISING_DELETE \r\n", __FUNCTION__);
            if (app_get_connection_id() != 0XFF)
            {
                sonata_ble_gatt_exchange_mtu(app_get_connection_id());
            }
            gAppEnv.gAppStatus = STATUS_NONE;
            app_cmd_adv_done();

            break;
        //Adv Data --------------------------------------------
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
            if (gPeroidAdv == true)
            {
                app_ble_set_periodic_adv_data(gWorkCounter);  //SONATA_GAP_CMP_SET_PERIOD_ADV_DATA
                break;
            }
            if (gAppEnv.bAdvWithScanResponse == true)
            {
                app_ble_set_scan_response_data();
            }
            else if(gWorkFlag == 0)
            {
                app_ble_start_advertising();
            }
            break;
        case SONATA_GAP_CMP_SET_PERIOD_ADV_DATA:
            APP_TRC("APP: %s ,gWorkCounter=%d, \r\n", __FUNCTION__,gWorkCounter);
            if (gWorkCounter == 0)
            {
                app_ble_start_advertising();//SONATA_GAP_CMP_ADVERTISING_START
            }
            break;
        case SONATA_GAP_CMP_PER_ADV_CTE_TX_CTL:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_PER_ADV_CTE_TX_CTL, index=%d, seq:%d \r\n", __FUNCTION__, (param), (uint16_t) dwparam);
            break;
        case SONATA_GAP_CMP_SET_SCAN_RSP_DATA:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SET_SCAN_RSP_DATA gWorkFlag=%d\r\n", __FUNCTION__,gWorkFlag);
            if(gWorkFlag == 0)
            {
                app_ble_start_advertising();
            }
            break;
        //Scan  --------------------------------------------
        case SONATA_GAP_CMP_SCANNING_CONFIG ://0x0F03
            app_ble_start_scanning();
            break;
        case SONATA_GAP_CMP_SCANNING_START ://0x0F07
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SCANNING_START \r\n", __FUNCTION__);
            //app_add_profiles();
            app_cmd_scan_start();
            if (app_command_get_cmdid() == ID_PERIODSYNC)
            {
                if (app_command_get_cmdtype() == TYPE_PEROIDSYNC_START_WITH_PAL)
                {
                    app_ble_config_period_sync(SONATA_GAP_PER_SYNC_TYPE_SELECTIVE);
                }
                else
                {
                    app_ble_config_period_sync(SONATA_GAP_PER_SYNC_TYPE_GENERAL);
                }
            }
            break;
        case SONATA_GAP_CMP_SCANNING_STOP:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SCANNING_STOP, index=%d, seq:%d \r\n", __FUNCTION__, (param), (uint16_t) dwparam);
            break;
        case SONATA_GAP_CMP_SCANNING_DELETE:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SCANNING_DELETE \r\n", __FUNCTION__);
            app_cmd_scan_end();
            gAppEnv.gAppStatus = STATUS_NONE;
            break;
        //Init  --------------------------------------------
        case SONATA_GAP_CMP_INITIATING_CONFIG ://0x0F04
            app_ble_start_initiating(gAppEnv.targetAddr);
            app_cmd_conn_start(app_get_connection_id());
            break;

        case SONATA_GAP_CMP_INITIATING_START: //0x0F08
            APP_TRC("ACP: %s  SONATA_GAP_CMP_INITIATING_START \r\n", __FUNCTION__);
            //app_add_profiles();
            app_cmd_mconn_update_status(param, 0XFF, gAppEnv.targetAddr);
            break;
        case SONATA_GAP_CMP_INITIATING_DELETE:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_INITIATING_DELETE \r\n", __FUNCTION__);
            gAppEnv.gAppStatus = STATUS_NONE;
            break;
        //Period Sync --------------------------------------------
        case SONATA_GAP_CMP_PERIOD_SYNC_CONFIG:
            app_ble_start_period_sync(gAppEnv.targetAddr);
            break;
        case SONATA_GAP_CMP_PERIOD_SYNC_START:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_PERIOD_SYNC_START \r\n", __FUNCTION__);
            break;
        //GAP --------------------------------------------
        case SONATA_GAP_CMP_PROFILE_TASK_ADD://0x011B
            APP_TRC("ACP: Profile added,statue=%d,dwparam=%lu\r\n", status, dwparam);
            if (dwparam == SONATA_PRF_ID_DISS)
            {
                prf_add_diss = true;
            }
            else if (dwparam == SONATA_PRF_ID_BASS)
            {
                prf_add_bass = true;
            }
            else if (dwparam == SONATA_PRF_ID_OTAS)
            {
                prf_add_ota = true;
            }
            if (prf_add_diss  && prf_add_bass && prf_add_ota)
            {
                #if (USE_APP_SEC)
                app_sec_complete_event_handler(opt_id, status, param, dwparam);
                #endif
            }
            break;
        case SONATA_GAP_CMP_GEN_RAND_NB:
        case SONATA_GAP_CMP_SET_IRK:
            #if (USE_APP_SEC)
            app_sec_complete_event_handler(opt_id, status, param, dwparam);
            #endif
            break;
        case SONATA_GAP_CMP_STOP_ALL_ACTIVITIES:
            app_cmd_active_clear_handler(opt_id);
            break;
        case SONATA_GAP_CMP_DELETE_ALL_ACTIVITIES:
            app_cmd_active_clear_handler(opt_id);
            break;
        case SONATA_GATT_CMP_NOTIFY:
            if (app_command_get_cmdid() != ID_THROUGHPUT)
            {
                //APP_TRC_HIGH("ACP: %s  SONATA_GATT_CMP_NOTIFY, seq:%d \r\n", __FUNCTION__, (uint16_t) dwparam);
                printf("ACP:seq:%d \r\n",  (uint16_t) dwparam);
                gAppEnv.gSendDataLock = false;
            }
//            dts_data_send_done(status);
            app_command_handle_ntf_complete_ind();
            break;
        case SONATA_GAP_CMP_SET_LE_PKT_SIZE:
            app_command_handle_le_pkt_size();
            break;
        case SONATA_GAP_CMP_UPDATE_PARAMS:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_UPDATE_PARAMS, status:%d \r\n", __FUNCTION__, status);
            break;
        case SONATA_GAP_CMP_ENCRYPT:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_ENCRYPT, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_GET_PEER_NAME:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_GET_PEER_NAME, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            if (app_nv_get_autontf() !=0)
            {
                uint8_t role = sonata_ble_get_role(param);
                if (role == SONATA_ROLE_MASTER)
                {
                    //Find DTS and trun on NTF
                    app_nv_get_uuid_value(&gAppEnv.appUuids);
                    sonata_ble_gatt_disc_all_characteristic(param, 1, 0xFFFF);
                    //Go to SONATA_GATT_CMP_DISC_ALL_CHAR
                    //NTF will turn on @app_gatt_disc_char_callback()
                    //sonata_ble_gatt_disc_all_descriptor(conidx, 1, 0xFFFF);
                }
            }
//            app_ble_config_legacy_advertising();
            break;
        case SONATA_GAP_CMP_GEN_RAND_ADDR:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_GEN_RAND_ADDR, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            #ifdef DEMO_RANDOM_RESLOVE_ADDRESS
                app_ble_on();
            #endif
            break;
        case SONATA_GAP_CMP_GET_RAL_SIZE:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_GET_RAL_SIZE, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_GET_RAL_LOC_ADDR:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_GET_RAL_LOC_ADDR, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_GET_RAL_PEER_ADDR:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_GET_RAL_PEER_ADDR, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_BOND:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_BOND, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_SECURITY_REQ:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SECURITY_REQ, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_SET_RAL:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SET_RAL, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_SET_WL:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_SET_WL, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GAP_CMP_RESOLV_ADDR:
            APP_TRC("ACP: %s  SONATA_GAP_CMP_RESOLV_ADDR, index = %d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        //Gatt --------------------------------------------
        case SONATA_GATT_CMP_MTU_EXCH:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_MTU_EXCH, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GATT_CMP_DISC_ALL_SVC:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_ALL_SVC, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GATT_CMP_DISC_BY_UUID_SVC:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_BY_UUID_SVC, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GATT_CMP_DISC_INCLUDED_SVC:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_INCLUDED_SVC, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            break;
        case SONATA_GATT_CMP_DISC_ALL_CHAR:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_ALL_CHAR, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            if (app_nv_get_autontf() !=0)
            {
                if (gAppEnv.targetNtfHandle == 44)
                {
                    gAppEnv.gDtsOn = dwparam;
                }
                app_ble_master_turn_ntf(param, gAppEnv.targetNtfHandle, true);
            }
            break;
        case SONATA_GATT_CMP_DISC_BY_UUID_CHAR:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_BY_UUID_CHAR, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            app_cmd_discovery_handler(status);
            break;
        case SONATA_GATT_CMP_DISC_DESC_CHAR:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_DISC_DESC_CHAR, index=%d, status:%d \r\n", __FUNCTION__, param, status);
            app_cmd_discovery_handler(status);
            break;
        case SONATA_GATT_CMP_READ:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_READ, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_READ_LONG:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_READ_LONG, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_READ_BY_UUID:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_READ_BY_UUID, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_READ_MULTIPLE:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_READ_MULTIPLE, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_WRITE:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_WRITE, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            gAppEnv.gSendDataLock = 0;
            if (gAppEnv.targetNtfHandle == 44)
            {
                gAppEnv.gDtsOn = 1;
            }
            break;
        case SONATA_GATT_CMP_WRITE_NO_RESPONSE:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_WRITE_NO_RESPONSE, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_WRITE_SIGNED:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_WRITE_SIGNED, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_EXEC_WRITE:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_EXEC_WRITE, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_REGISTER:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_REGISTER, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_UNREGISTER:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_UNREGISTER, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_INDICATE:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_INDICATE, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_SDP_DISC_SVC:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_SDP_DISC_SVC, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_SDP_DISC_SVC_ALL:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_SDP_DISC_SVC_ALL, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_SDP_DISC_CANCEL:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_SDP_DISC_CANCEL, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_READ_DB_HASH:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_READ_DB_HASH, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_ROBUST_DB_CACHE_EN:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_ROBUST_DB_CACHE_EN, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GATT_CMP_SVC_CHANGED:
            APP_TRC("ACP: %s  SONATA_GATT_CMP_SVC_CHANGED, index=%d, seq:%d \r\n", __FUNCTION__,(param), (uint16_t) dwparam);
            break;
        case SONATA_GAP_CMP_RESET:
            app_platform_reset_complete_handler();
            break;
        default:
            APP_TRC("ACP: %s  App get uncased complete event, please check. id:%04X, status=%d \r\n", __FUNCTION__, opt_id, status);
            break;
    }

    return CB_DONE;
}

#define LEN_ADV_DATA (64)
#define LEN_ADV_BUFF_ITEM (40)
typedef struct scan_info{
    struct sonata_gap_bdaddr addr;
    uint8_t length;
    uint8_t advData[LEN_ADV_DATA];
    uint8_t advLength;
    uint8_t rspLength;
}scan_info_t;

scan_info_t scanInfo[LEN_ADV_BUFF_ITEM] = {0};

void app_gap_combine_scan_result(uint8_t type, uint8_t *addr, uint8_t addr_type, uint8_t length, uint8_t *data)
{
    //APP_TRC("APP: app_gap_combine_scan_result ,type=%d,length=%d\r\n", type, length);
    if (length == 0)
    {
        return;
    }
    uint8_t temp[SONATA_GAP_BD_ADDR_LEN] = {0};
    uint8_t useLen = 0;
    for (int i = 0; i < LEN_ADV_BUFF_ITEM; ++i)
    {
        scan_info_t *info = &scanInfo[i];
        if (memcmp(addr, info->addr.addr.addr, SONATA_GAP_BD_ADDR_LEN) == 0 || memcmp(info->addr.addr.addr, temp, SONATA_GAP_BD_ADDR_LEN) == 0)
        {
            switch (type)
            {
                case SONATA_GAP_REPORT_TYPE_ADV_EXT :
                    info->length = length;
                    info->advLength = length;
                    info->rspLength = 0;
                    useLen = length > LEN_ADV_DATA ? LEN_ADV_DATA : length;
                    memcpy(info->advData, data, useLen);
                    addrCopy(info->addr.addr.addr, addr);
                    info->addr.addr_type = addr_type;
                    break;
                case SONATA_GAP_REPORT_TYPE_ADV_LEG:
                    info->length = length;
                    info->advLength = length;
                    info->rspLength = 0;
                    useLen = length > LEN_ADV_DATA ? LEN_ADV_DATA : length;
                    memcpy(info->advData, data, useLen);
                    addrCopy(info->addr.addr.addr, addr);
                    info->addr.addr_type = addr_type;
                    break;
                case SONATA_GAP_REPORT_TYPE_SCAN_RSP_EXT:
                case SONATA_GAP_REPORT_TYPE_SCAN_RSP_LEG:
                    info->length += length;
                    info->rspLength = length;
                    useLen = (length + info->advLength) > LEN_ADV_DATA ? (LEN_ADV_DATA - info->advLength) : length;
                    memcpy(info->advData + info->advLength, data, useLen);
                    addrCopy(info->addr.addr.addr, addr);
                    info->addr.addr_type = addr_type;
                    break;
            }
            break;
        }
    }

    {
        for (int loop = 0; loop < LEN_ADV_BUFF_ITEM; ++loop)
        {
            scan_info_t *info = &scanInfo[loop];
            uint8_t k =0;
            APP_TRC("INFO[%02d]: addr:", loop);
            for (k = 0; k < SONATA_GAP_BD_ADDR_LEN; ++k)
            {
                APP_TRC("%02X ", info->addr.addr.addr[k]);
            }
            APP_TRC(" adv[%02d]:", info->length);
            for (k = 0; k < info->length; ++k)
            {
                APP_TRC("%02X ", info->advData[k]);
            }
            APP_TRC("\r\n");
        }
    }

}


static uint8_t gScanResultNameBuffer[40];
static uint16_t  app_gap_scan_result_callback(sonata_gap_ext_adv_report_ind_t *result)
{
    if (app_command_get_cmdid() == ID_PERIODSYNC)
    {
        if (result->adv_sid != gAppEnv.advSid ) //skip adv that sid not right
        {
            return CB_DONE;
        }
    }
    uint8_t type = result->info & 0x7;
    switch (type)
    {
        case SONATA_GAP_REPORT_TYPE_ADV_EXT :
            APP_TRC_HIGH("SCAN: type[%02X]=ADV_EXT      ",result->info);
            break;
        case SONATA_GAP_REPORT_TYPE_ADV_LEG:
            APP_TRC_HIGH("SCAN: type[%02X]=ADV_LEG      ",result->info);
            break;
        case SONATA_GAP_REPORT_TYPE_SCAN_RSP_EXT:
            APP_TRC_HIGH("SCAN: type[%02X]=SCAN_RSP_EXT ",result->info);
            break;
        case SONATA_GAP_REPORT_TYPE_SCAN_RSP_LEG:
            APP_TRC_HIGH("SCAN: type[%02X]=SCAN_RSP_LEG ",result->info);
            break;
        case SONATA_GAP_REPORT_TYPE_PER_ADV:
            APP_TRC_HIGH("SCAN: type[%02X]=PER_ADV      ",result->info);
            break;
    }
    APP_TRC_HIGH(" Sid:%03d",result->adv_sid);

    APP_TRC_HIGH(" addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)    {APP_TRC_HIGH("%02X ", result->trans_addr.addr.addr[i]);}

    APP_TRC(" adv[%02d]:", result->length);
    for (int i = 0; i < result->length; ++i)    {APP_TRC("%02X ", result->data[i]);}

    uint16_t rLen = 0;
    sonata_get_adv_report_info(0x09, gScanResultNameBuffer, 31, result->data, result->length, &rLen);
    APP_TRC(" Name:");
    for (int i = 0; i < rLen; ++i)    {APP_TRC("%c", gScanResultNameBuffer[i]);}
    APP_TRC_HIGH("\r\n");

    //app_gap_combine_scan_result(type, result->trans_addr.addr.addr, result->trans_addr.addr_type, result->length, result->data);
    //app_cmd_scan_result_handler(result->trans_addr.addr.addr, result->trans_addr.addr_type, result->data, result->length);
    #if DEMO_SCAN_AND_CONNECT
    if (bFound == false)
    {
        if (memcmp(result->trans_addr.addr.addr, targetAddr1, SONATA_GAP_BD_ADDR_LEN) == 0)
        {
            bFound = true;
            sonata_ble_stop_scanning();
            //If app adds gap_active_stopped() callback, SDK will callback when active stopped.App can restart or delete it.
            //If app not adds gap_active_stopped() callback,SDK will stop active and then delete it.
        }
    }

    #endif


//    {
//        if ((result->info  & SONATA_GAP_REPORT_INFO_COMPLETE_BIT) == SONATA_GAP_REPORT_INFO_COMPLETE_BIT)
//        {
//            APP_TRC("  addr:");
//            for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
//            {
//                APP_TRC("%02X ", result->trans_addr.addr.addr[i]);
//            }
//
//            APP_TRC(" adv[%02d]:", result->length);
//            for (int i = 0; i < result->length; ++i)
//            {
//                APP_TRC("%02X ", result->data[i]);
//            }
//            APP_TRC("\r\n");
//        }
//    }
    return CB_DONE;
}

void app_ble_get_peer_rssi()
{
    sonata_ble_gap_get_peer_info(app_get_connection_id(), SONATA_GET_PEER_CON_RSSI);

}

static uint16_t app_ble_rsp_event_handler(uint16_t opt_id, uint8_t status, uint16_t handle, uint16_t perm, uint16_t ext_perm, uint16_t length, void *param)
{
    APP_TRC("APP: %s  opt_id=%04X,\r\n", __FUNCTION__, opt_id);
    if (status != 0)
    {
        APP_TRC("APP: %s  ERROR=%04X,\r\n", __FUNCTION__, status);
    }
    switch (opt_id)
    {
        case SONATA_GATT_ADD_SVC_RSP:
        {
            APP_TRC("APP: %s  handle=%d,\r\n", __FUNCTION__, handle);
            //Should save the start handle id for future use
            prf_add_dts = true;
            custom_svc_start_handle = handle;
            dts_set_start_handle(custom_svc_start_handle);
            break;
        }
        case SONATA_GATT_ATT_GET_VALUE_RSP:
        {
            uint8_t *value = param;
            APP_TRC("APP  %s, GatValue:%s", __FUNCTION__,value);

        }
            break;

        default:
            break;
    }

    return CB_DONE;
}


/*!
 * @brief get devcie informations
 * @param info_type @see sonata_gap_local_dev_info
 * @param info
 */
static uint16_t app_get_dev_info_callback(sonata_gap_local_dev_info info_type, void *info)
{
    APP_TRC("\r\n");
    switch (info_type)
    {
        case SONATA_GET_DEV_VERSION:
        {
            #if APP_DBG
            sonata_gap_dev_version_ind_t *dev_info = (sonata_gap_dev_version_ind_t *) info;
            APP_TRC(">>> hci_ver    =%d\r\n"       , dev_info->hci_ver);
            APP_TRC(">>> lmp_ver    =%d\r\n"       , dev_info->lmp_ver);
            APP_TRC(">>> host_ver   =%d\r\n"     , dev_info->host_ver);
            APP_TRC(">>> hci_subver =%d\r\n"   , dev_info->hci_subver);
            APP_TRC(">>> lmp_subver =%d\r\n"   , dev_info->lmp_subver);
            APP_TRC(">>> host_subver=%d\r\n"  , dev_info->host_subver);
            APP_TRC(">>> manuf_name =0x%04X\r\n", dev_info->manuf_name);
            #endif //SONATA_API_TASK_DBG
        }
            break;
        case SONATA_GET_DEV_BDADDR:
        {
            #if APP_DBG
            sonata_gap_dev_bdaddr_ind_t *param = (sonata_gap_dev_bdaddr_ind_t *) info;
            APP_TRC(">>> SONATA_GET_DEV_BDADDR:");
            for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
            { APP_TRC("%02X ", param->addr.addr.addr[i]); }
            APP_TRC("\r\n");
            APP_TRC(">>> Normal addr:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    param->addr.addr.addr[5],param->addr.addr.addr[4],param->addr.addr.addr[3],
                    param->addr.addr.addr[2],param->addr.addr.addr[1],param->addr.addr.addr[0]);

            #endif //SONATA_API_TASK_DBG
        }
            break;

        case SONATA_GET_DEV_ADV_TX_POWER:
        {
            sonata_gap_dev_adv_tx_power_ind_t *param = (sonata_gap_dev_adv_tx_power_ind_t *) info;
            APP_TRC(">>>: SONATA_GET_DEV_ADV_TX_POWER power_lvl =%dX\r\n",  param->power_lvl);
        }
            break;
        case SONATA_GET_WLIST_SIZE:
        {
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC(">>>: SONATA_GET_WLIST_SIZE size =%d\r\n",  param->size);
            break;
        }
        case SONATA_GET_ANTENNA_INFO:
        {
            #if APP_DBG
            sonata_gap_antenna_inf_ind_t *param = (sonata_gap_antenna_inf_ind_t *) info;
            APP_TRC(">>> SONATA_GET_ANTENNA_INFO supp_switching_sampl_rates =%d, antennae_num =%d, max_switching_pattern_len =%d, max_cte_len =%d\r\n",
                    param->supp_switching_sampl_rates, param->antennae_num, param->max_switching_pattern_len, param->max_cte_len);
            #endif //SONATA_API_TASK_DBG
        }
            break;

        case SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_sugg_dflt_data_len_ind_t *param = (sonata_gap_sugg_dflt_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN suggted_max_tx_octets =%d, suggted_max_tx_time =%d\r\n",
                    param->suggted_max_tx_octets, param->suggted_max_tx_time);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_MAX_LE_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_max_data_len_ind_t *param = (sonata_gap_max_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_MAX_LE_DATA_LEN suppted_max_tx_octets =%d, suppted_max_tx_time =%d, suppted_max_rx_octets =%d, suppted_max_rx_time =%d\r\n",
                    param->suppted_max_tx_octets, param->suppted_max_tx_time, param->suppted_max_rx_octets, param->suppted_max_rx_time);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_PAL_SIZE:
        {
            #if APP_DBG
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC(">>>: SONATA_GET_PAL_SIZE size =%d\r\n",  param->size);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_RAL_SIZE:
        {
            #if APP_DBG
            sonata_gap_list_size_ind_t *param = (sonata_gap_list_size_ind_t *) info;
            APP_TRC(">>>: SONATA_GET_RAL_SIZE size =%d\r\n",  param->size);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_NB_ADV_SETS:
        {
            #if APP_DBG
            sonata_gap_nb_adv_sets_ind_t *param = (sonata_gap_nb_adv_sets_ind_t *) info;
            APP_TRC(">>>: SONATA_GET_NB_ADV_SETS nb_adv_sets =%d\r\n",  param->nb_adv_sets);
            #endif //SONATA_API_TASK_DBG

            break;
        }
        case SONATA_GET_MAX_LE_ADV_DATA_LEN:
        {
            #if APP_DBG
            sonata_gap_max_adv_data_len_ind_t *param = (sonata_gap_max_adv_data_len_ind_t *) info;
            APP_TRC(">>> SONATA_GET_MAX_LE_ADV_DATA_LEN param->length=%d\r\n", param->length);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_DEV_TX_PWR:
        {
            #if APP_DBG
            sonata_gap_dev_tx_pwr_ind_t *param = (sonata_gap_dev_tx_pwr_ind_t *) info;
            APP_TRC(">>> SONATA_GET_DEV_TX_PWR min_tx_pwr =%d, max_tx_pwr =%d\r\n",
                    param->min_tx_pwr, param->max_tx_pwr);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        case SONATA_GET_DEV_RF_PATH_COMP:
        {
            #if APP_DBG
            sonata_gap_dev_rf_path_comp_ind_t *param = (sonata_gap_dev_rf_path_comp_ind_t *) info;
            APP_TRC(">>> SONATA_GET_DEV_RF_PATH_COMP tx_path_comp =0x%04X, rx_path_comp =0x%04X\r\n",
                    param->tx_path_comp, param->rx_path_comp);
            #endif //SONATA_API_TASK_DBG
            break;
        }
        default:
            APP_TRC(">>>: %s  No progress for info_type=%02X\r\n", __FUNCTION__, info_type);
            break;

    }
    APP_TRC("\r\n");
    app_cmd_devinfo_done_handler(info_type);
    return CB_DONE;
}


/*!
 * @brief Deal with peer device get local information request.
 * @param opt @see asr_gap_dev_info
 */
static uint16_t app_gap_peer_get_local_info_callback(uint8_t conidx, sonata_gap_dev_info opt)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    switch (opt)
    {
        case SONATA_GAP_DEV_NAME:
        {
            uint8_t dev_name_saved[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
            if(app_nv_get_local_device_name(dev_name_saved, APPNV_LEN_LOCAL_DEVICE_NAME, true))
            {
                sonata_ble_gap_send_get_dev_info_cfm_for_dev_name(conidx, dev_name_saved[0], dev_name_saved + 1);
            }
        }
            break;
        case SONATA_GAP_DEV_APPEARANCE:
        {
            uint16_t appearance = 0x03C1;
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_appearance(conidx, appearance);
        }
            break;
        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
            sonata_ble_gap_send_get_dev_info_cfm_for_slv_pref_params(conidx, 8, 10, 0, 200);
            break;
        case SONATA_GAPC_DEV_RECONNECTION_ADDR:
            gAppEnv.targetAddr[0] = 0x11;
            gAppEnv.targetAddr[5] = 0x66;
            sonata_ble_gap_send_get_dev_info_cfm_for_reconnection_address(conidx, gAppEnv.targetAddr);
            break;
        default:
            break;
    }

    return CB_DONE;
}

/*!
 * @brief
 * @param conidx
 * @param conhdl
 * @param reason
 * @return
 */
static uint16_t app_gap_disconnect_ind_callback(uint8_t conidx, uint16_t conhdl, uint8_t reason)
{
    APP_TRC_HIGH("APP_CB: %s  conidx=%02X, reason=%02X(X)************************\r\n", __FUNCTION__, conidx, reason);
    sonata_api_app_timer_clear(APP_TIMER_NTF_DATA);
    sonata_api_app_timer_clear(APP_TIMER_READ_RSSI);

    app_active_delete(conidx);
    //app_uart_irq_timer_stop();
    gAppEnv.connection_id = 0XFF;
    gAppEnv.targetWriteHandle = 0;
    gAppEnv.appUuids.service = 0;
    gAppEnv.appUuids.read = 0;
    gAppEnv.appUuids.write = 0;
    gAppEnv.appUuids.ntf = 0;
    gAppEnv.gAppStatus = STATUS_NONE;
    gAppEnv.gSendDataLock = 0;
    app_cmd_ledisc_done(conidx);
    app_att_db_clear();
    if (app_nv_get_autoadv() == true)
    {
        app_ble_config_legacy_advertising();
    }
    else if (app_command_get_cmdid() == ID_MCONN)
    {
        app_cmd_mconn_continue();
    }
    {
        app_ble_auto_conn();
    }
    return CB_DONE;
}


/*!
 * @brief
 * @param info
 */
static uint16_t app_gap_peer_att_info_callback(uint8_t conidx, sonata_gap_peer_att_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    sonata_gap_peer_att_info_ind_t *att_info = (sonata_gap_peer_att_info_ind_t *) info;
    APP_TRC("APP_CB: %s, req =0x%02X\r\n", __FUNCTION__, att_info->req);
    APP_TRC("APP_CB: %s, handle =0x%02X\r\n", __FUNCTION__, att_info->handle);
    switch (att_info->req)
    {
        case SONATA_GAP_DEV_NAME:
        {
            APP_TRC("APP  %s, Name:", __FUNCTION__);
            for (int i = 0; i < att_info->info.name.length; ++i)
            {
                APP_TRC("%c", att_info->info.name.value[i]);
            }
            APP_TRC("\r\n");
            uint8_t len = att_info->info.name.length > 20 ? 20 : att_info->info.name.length;
            memcpy(gAppEnv.act[conidx].name, att_info->info.name.value, len);
            app_ble_print_peer_info(att_info->info.name.value, len, gAppEnv.act[conidx].peer);
            //sonata_ble_gap_set_le_pkt_size(0, 251, 2120);
        }
            break;
        case SONATA_GAP_DEV_APPEARANCE:
        {
            APP_TRC("APP_CB: %s, appearance =0x%04X\r\n", __FUNCTION__, att_info->info.appearance);
        }
            break;
        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
        {
            APP_TRC("APP_CB: %s, con_intv_min =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_min);
            APP_TRC("APP_CB: %s, con_intv_max =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_max);
            APP_TRC("APP_CB: %s, slave_latency =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.slave_latency);
            APP_TRC("APP_CB: %s, conn_timeout =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.conn_timeout);
        }
            break;
        case SONATA_GAP_DEV_CTL_ADDR_RESOL:
        {
            APP_TRC("APP_CB: %s, ctl_addr_resol =0x%02X\r\n", __FUNCTION__, att_info->info.ctl_addr_resol);
        }
            break;

        default:
            break;

    }
    return CB_DONE;
}


static uint16_t app_gap_peer_info_callback(uint8_t conidx, sonata_gap_peer_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X, info_type=%02X\r\n", __FUNCTION__, conidx, info->req);
    switch (info->req)
    {
        case SONATA_GET_PEER_VERSION:
            APP_TRC("APP_CB: SONATA_GET_PEER_VERSION, compid:%04X,lmp_subvers:%04X,lmp_vers:%02X,\r\n",
                    info->info.version.compid, info->info.version.lmp_subvers, info->info.version.lmp_vers);
            break;
        case SONATA_GET_PEER_FEATURES:
            APP_TRC("APP_CB: SONATA_GET_PEER_FEATURES, features:");
            for (int i = 0; i < SONATA_GAP_LE_FEATS_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.features.features[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_PEER_CON_RSSI:
        {
            uint8_t rssi = 0 - info->info.rssi.rssi;
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_RSSI, rssi:%d, convert rssi=%d\r\n", info->info.rssi.rssi, rssi);
            dts_send_dec_data(rssi);
        }
            break;
        case SONATA_GET_PEER_CON_CHANNEL_MAP:
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_CHANNEL_MAP, map:");
            for (int i = 0; i < SONATA_GAP_LE_CHNL_MAP_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.channel_map.ch_map.map[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_LE_PING_TO:
            APP_TRC("APP_CB: SONATA_GET_LE_PING_TO, timeout:%04X,\r\n", info->info.ping_to_value.timeout);
            break;
        case SONATA_GET_PHY:
            APP_TRC("APP_CB: SONATA_GET_PHY, tx_phy:%02X, rx_phy:%02X\r\n", info->info.le_phy.tx_phy, info->info.le_phy.rx_phy);
            break;
        case SONATA_GET_CHAN_SEL_ALGO:
            APP_TRC("APP_CB: SONATA_GET_CHAN_SEL_ALGO, chan_sel_algo:%04X,\r\n", info->info.sel_algo.chan_sel_algo);
            break;
        default:
            break;
    }

    return CB_DONE;
}

/*!
 * @brief
 * @param conidx
 * @param intv_min
 * @param intv_max
 * @param latency
 * @param time_out
 */
static uint16_t app_gap_param_update_req_callback(uint8_t conidx, uint16_t intv_min, uint16_t intv_max, uint16_t latency, uint16_t time_out)
{
    APP_TRC("APP_CB: %s  conidx=%d,intv_min=%d,intv_max=%d,latency=%d,time_out=%d\r\n", __FUNCTION__, conidx, intv_min, intv_max, latency, time_out);
    sonata_ble_gap_send_param_update_cfm(conidx, true, 2, 4);
    return CB_DONE;
}

/*!
 * @brief
 * @param conidx
 * @param con_interval
 * @param con_latency
 * @param sup_to
 * @return
 */
static uint16_t app_gap_param_updated_callback(uint8_t conidx, uint16_t con_interval, uint16_t con_latency, uint16_t sup_to)
{
    APP_TRC("APP_CB: %s  conidx=%d,con_interval=%d,con_latency=%d,sup_to=%d\r\n", __FUNCTION__, conidx, con_interval, con_latency, sup_to);
    app_command_handle_param_updated();
    return CB_DONE;

}

static uint16_t app_gap_le_pke_size_callback(uint8_t conidx, uint16_t max_tx_octets, uint16_t max_tx_time, uint16_t max_rx_octets, uint16_t max_rx_time)
{
    APP_TRC("APP_CB: %s  max_tx_octets=%d(d), max_rx_octets=%d(d)\r\n", __FUNCTION__, max_tx_octets, max_rx_octets);
    app_command_handle_le_pkt_size();
    return CB_DONE;
}

static uint16_t  app_gap_period_sync_callback(uint8_t actv_idx, uint8_t phy, uint16_t intv, uint8_t adv_sid,
                                              uint8_t clk_acc, uint16_t serv_data, uint8_t addr_type, uint8_t *addr)
{
    uint8_t antenna_id[] = { 0, 1} ;
    APP_TRC("APP_CB: %s,actv_idx=%d, phy=%d, intv=%d, adv_sid=%d, clk_acc=%d, serv_data=%d", __FUNCTION__, actv_idx, phy, intv, adv_sid, clk_acc, serv_data);
    APP_TRC(",addr[type=%d]:%02X %02X %02X %02X %02X %02X\r\n", addr_type, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    gAppEnv.advSid = adv_sid;
    app_ble_stop_scanning();
    sonata_ble_gap_periodic_advertising_iq_samples_control(actv_idx, true, 2, 0x08, 2, antenna_id);
    return CB_DONE;
}


/*!
 * @brief
 * @param conidx
 * @param length
 * @param name
 * @return
 */
static uint16_t app_gap_peer_set_local_device_name_callback(uint8_t conidx, uint16_t length, uint8_t *name)
{
    APP_TRC_HIGH("APP_CB: %s \r\n", __FUNCTION__);
    if (length >= APPNV_LEN_LOCAL_DEVICE_NAME)
    {
        sonata_ble_gap_send_set_dev_name_cfm(conidx, SONATA_HL_GAP_ERR_REJECTED);
    }
    else
    {
        if (app_nv_set_local_device_name(name, length))
        {
            sonata_ble_gap_send_set_dev_name_cfm(conidx, 0x0);
            APP_TRC_HIGH("\r\n Set Name Done:");
            for (int i = 0; i < length; ++i)
            { APP_TRC_HIGH("%c ", ((uint8_t *) name)[i]); }
            APP_TRC_HIGH("\r\n");
        }
        else
        {
            sonata_ble_gap_send_set_dev_name_cfm(conidx, SONATA_HL_GAP_ERR_REJECTED);
        }
    }
    return CB_DONE;
}


static uint16_t app_gap_connection_req_callback(uint8_t conidx,sonata_gap_connection_req_ind_t *req) {
    APP_TRC_HIGH("APP_CB: %s conidx=%d \r\n", __FUNCTION__, conidx);
    APP_TRC_HIGH("      : app_bond       =%d  \r\n"        , app_nv_get_bonded());
    APP_TRC_HIGH("      : conhdl         =%d  \r\n"        , req->conhdl);
    APP_TRC_HIGH("      : con_interval   =%d  \r\n"  , req->con_interval);
    APP_TRC_HIGH("      : con_latency    =%d  \r\n"   , req->con_latency);
    APP_TRC_HIGH("      : sup_to(timeout)=%d  \r\n"        , req->sup_to);
    APP_TRC_HIGH("      : clk_accuracy   =%d  \r\n"  , req->clk_accuracy);
    APP_TRC_HIGH("      : addr_type      =%d  \r\n", req->peer_addr_type);
    APP_TRC_HIGH("      : role           =%d  \r\n"          , req->role);
    APP_TRC_HIGH("      : addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i) { APP_TRC("%02X ", req->peer_addr.addr[i]); }
    APP_TRC_HIGH(" \r\n");

    gAppEnv.connection_id = conidx;
    app_active_update(conidx, req->peer_addr.addr);
    app_cmd_mconn_update_status(0xFF, conidx, req->peer_addr.addr);
    app_cmd_conn_result_handler(conidx);
    #if (USE_APP_SEC)
    app_sec_set_peer_addr(req->peer_addr.addr);
    #endif
    #if DEMO_PRF_BASS
    sonata_prf_batt_enable_prf(conidx);
    #endif

    //Send connection confirm with auth type
    #if (USE_APP_SEC)
    sonata_gap_connection_cfm_t cfm = {0};
    app_sec_make_connection_confirm_parameter(req, &cfm);
    #else
    sonata_gap_connection_cfm_t cfm = {0};
    cfm.auth = SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND;
    #endif
    sonata_ble_gap_send_connection_cfm(conidx, &cfm);
    //If need to start encrypt, then start it
    #if (USE_APP_SEC)
    app_sec_start_encrypt(conidx, req);
    #endif

    app_cmd_conn_done(app_get_connection_id());
    if (app_command_get_cmdid() == ID_MCONN)
    {
        sonata_api_app_timer_set(APP_TIMER_MCONN,3000);
    }
    else
    {
        sonata_ble_gap_get_peer_info(app_get_connection_id(), SONATA_GET_PEER_NAME); //Goto SONATA_GAP_CMP_GET_PEER_NAME
    }
    return CB_DONE; //SDK will send connection confirm message

}

/*!
 * @brief
 * @param connection_id
 * @param handle
 * @return
 */
static uint16_t app_gatt_read_request_callback(uint8_t connection_id, uint16_t handle)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=%d,\r\n", __FUNCTION__, handle);
    if (handle > custom_svc_start_handle)
    {
        return dts_read_request_handler(connection_id, handle);
    }

    return CB_REJECT;
}

static uint16_t app_gatt_read_callback(uint8_t conidx, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=0x%04X,\r\n", __FUNCTION__, handle);
    printf("APP_CB  %s, Read data form peer:", __FUNCTION__);
    for (int i = 0; i < length; ++i)
    {
        printf("%02X[%c] ", value[i],value[i]);
    }
    printf("\r\n");

    return CB_DONE;
}
/*!
 * @brief
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
static uint16_t app_gatt_write_request_callback(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    //APP_TRC("APP_CB: %s, handle=0x%04X,custom_svc_start_handle=0x%04X", __FUNCTION__,handle,custom_svc_start_handle);
    APP_TRC("APP_CB: %s, handle=%0d\r\n", __FUNCTION__, handle);
    if (handle > custom_svc_start_handle)
    {
        return dts_write_request_handler(connection_id, handle, offset, length, value);
    }

    return CB_DONE;
}



/*!
 * @brief
 * @param connection_id
 * @param mtu
 * @return
 */
static uint16_t app_gatt_mtu_changed_callback(uint8_t connection_id, uint16_t mtu)
{
    APP_TRC("APP_CB: %s, mtu=%d\r\n", __FUNCTION__, mtu);
    dts_set_mtu(APP_MTU);
    return CB_DONE;
}

static uint16_t app_gatt_disc_svc_callback(uint8_t connection_id, uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    uint16_t service_uuid = 0;
    //APP_TRC("APP_CB: %s, start_hdl=0x%04X, end_hdl =0x%04X, uuid=", __FUNCTION__, start_hdl, end_hdl);

    for (int i = 0; i < uuid_len; ++i)
    {
        // APP_TRC("%02X", uuid[i]);
        if (i == 0)
        {
            service_uuid = uuid[i];
        }
        if (i == 1)
        {
            service_uuid = uuid[i] << 8 | service_uuid;
        }
    }
    APP_TRC("\r\n");
    APP_TRC("APP_CB: %s,service_uuid = %04X[%02X %02X], start=%02d, end=%02d,targetUUID= %04X\r\n", __FUNCTION__,
            service_uuid,uuid[0],uuid[1], start_hdl, end_hdl, gAppEnv.appUuids.service);

    if (service_uuid == gAppEnv.appUuids.service)
    {
        sonata_ble_gatt_disc_all_characteristic(connection_id, start_hdl, end_hdl);
    }

    app_att_db_update_range(start_hdl,service_uuid,start_hdl,end_hdl);
    return CB_DONE;
}

static uint16_t app_gatt_disc_char_callback(uint8_t conidx, uint16_t attr_hdl, uint16_t pointer_hdl, uint8_t prop, uint8_t uuid_len, uint8_t *uuid)
{
    uint16_t char_uuid = 0;
    //APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        //APP_TRC("%02X", uuid[i]);
        if (i == 0)
        {
            char_uuid = uuid[i];
        }
        if (i == 1)
        {
            char_uuid = uuid[i] << 8 | char_uuid;
        }
    }

    APP_TRC("APP_CB: %s,char_uuid = %04X,attr_hdl=%02d,prop=%02X, target Read:%04X, Write=%04X, Ntf=%04X\r\n", __FUNCTION__, char_uuid, attr_hdl, prop,
            gAppEnv.appUuids.read, gAppEnv.appUuids.write,gAppEnv.appUuids.ntf);

    if (char_uuid == gAppEnv.appUuids.read)
    {
        gAppEnv.attrHandle = attr_hdl;
        gAppEnv.targetReadHandle = attr_hdl + 1;
        APP_TRC_HIGH("APP_CB: %s, ***Find targetReadHandle =%d, UUID=%04X\r\n", __FUNCTION__, gAppEnv.targetReadHandle, char_uuid);
    }
    if (char_uuid == gAppEnv.appUuids.write)
    {
        gAppEnv.targetWriteHandle = attr_hdl + 1;
        APP_TRC_HIGH("APP_CB: %s, ***Find targetWriteHandle=%d, UUID=%04X\r\n", __FUNCTION__, gAppEnv.targetWriteHandle,char_uuid);

    }
    if (char_uuid == gAppEnv.appUuids.ntf)
    {
        gAppEnv.targetNtfHandle = attr_hdl + 2;
        APP_TRC_HIGH("APP_CB: %s, ***Find targetNtfHandle  =%d, UUID=%04X\r\n", __FUNCTION__, gAppEnv.targetNtfHandle,char_uuid);

    }

    app_att_db_update_prop(attr_hdl, prop);
    return CB_DONE;
}

static uint16_t app_gatt_disc_char_desc_callback(uint8_t conidx, uint16_t attr_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    uint16_t service_uuid = 0;

    //APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        //APP_TRC("%02X", uuid[i]);
        if (i == 0)
        {
            service_uuid = uuid[i];
        }
        if (i == 1)
        {
            service_uuid = uuid[i] << 8 | service_uuid;
        }
    }
    APP_TRC("APP_CB: %s,uuid = %04X,attr_hdl=%02d\r\n", __FUNCTION__, service_uuid, attr_hdl);
    if (service_uuid == SONATA_ATT_DESC_CLIENT_CHAR_CFG && attr_hdl > gAppEnv.attrHandle && gAppEnv.targetNtfHandle == 0)
    {
        gAppEnv.targetNtfHandle = attr_hdl;
        APP_TRC("\r\n");
        APP_TRC("APP_CB: %s,service_uuid = %04X, Find targetNtfHandle = %d\r\n", __FUNCTION__, service_uuid, gAppEnv.targetNtfHandle);
        if (app_nv_get_autontf() !=0)
        {
            app_ble_master_turn_ntf(conidx, gAppEnv.targetNtfHandle, true);
        }
    }
    app_att_db_push(attr_hdl, service_uuid);
    return CB_DONE;
}


static uint16_t app_gatt_event_callback(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value)
{
    APP_TRC_HIGH("APP_CB: %s,conidx=%d handle = %04X, type = %04X,length = %d, data[0]=%02X\r\n", __FUNCTION__, conidx, handle, type, length,value[0]);
    APP_TRC("APP_CB: %s, Master get Ntf data form Slave. Data[%d]:", __FUNCTION__, length);
    for (int i = 0; i < length; ++i)
    {
        APP_TRC("%02X[%c] ", value[i], value[i]);
    }
    APP_TRC("\r\n");
    if (gAppEnv.gSendDataLock == 0)
    {
        gAppEnv.gSendDataLock = 1;
        app_command_handle_gatt_event(conidx, handle, type, length, value);
    }
    else
    {
        printf("\r\n");
    }
    app_command_handle_peer_command(value, length);
    return CB_DONE;
}

static uint16_t app_gatt_event_req_callback(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value)
{
    APP_TRC("APP_CB: %s,handle = %04X, type = %04X,length = %04X\r\n", __FUNCTION__,
            handle, type, length);

    APP_TRC("APP_CB: %s, Master get Ind data form Slave. Data:", __FUNCTION__);
    for (int i = 0; i < length; ++i)
    {
        APP_TRC("%02X[%c] ", value[i],value[i]);
    }
    APP_TRC("\r\n");
    sonata_ble_gatt_send_event_confirm(conidx, handle);


    return CB_DONE;
}

static uint16_t app_gatt_connection_info_callback(uint8_t conidx, uint16_t gatt_start_handle, uint16_t gatt_end_handle, uint16_t svc_chg_handle,
                                                  uint8_t cli_info, uint8_t cli_feat)
{
    APP_TRC("APP_CB: %s,conidx=%d,gatt_start_handle=%d,gatt_end_handle=%d,svc_chg_handle=%d,cli_info=%d,cli_feat=%d\r\n",
            __FUNCTION__ ,conidx, gatt_start_handle, gatt_end_handle, svc_chg_handle, cli_info, cli_feat);

    return CB_DONE;
}

static uint16_t app_gatt_att_info_req_callback(uint8_t conidx, uint16_t handle)
{
    APP_TRC("APP_CB: %s,conidx = %d, handle = %d\r\n", __FUNCTION__,  conidx, handle);
    sonata_ble_gatt_send_att_info_confirm(conidx, handle, 100, 0);
    return CB_DONE;
}
uint8_t app_get_connection_id()
{
    return gAppEnv.connection_id;
}


app_status app_get_status()
{
    return gAppEnv.gAppStatus;
}

bool app_ble_send_data(uint8_t conidx, uint16_t length, uint8_t*data)
{
    if (gAppEnv.targetWriteHandle != 0)
    {
        sonata_ble_gatt_write(conidx, gAppEnv.targetWriteHandle, 0, 0, length, data);
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetWriteHandle [%d] is wrong, can't send data\r\n", __FUNCTION__, gAppEnv.targetWriteHandle);

    }
    return false;
}


bool app_ble_master_write_data(uint8_t conidx, uint16_t length, uint8_t *data)
{
    if (gAppEnv.targetWriteHandle != 0)
    {
        APP_TRC("APP: %s,targetWriteHandle=%d, dataLen=%d\r\n", __FUNCTION__, gAppEnv.targetWriteHandle,length);
        sonata_ble_gatt_write(conidx, gAppEnv.targetWriteHandle, 0, 0, length, data);
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetWriteHandle [%d] is wrong, can't write data\r\n", __FUNCTION__, gAppEnv.targetWriteHandle);

    }
    return false;
}

bool app_ble_master_write_data_no_response(uint8_t conidx, uint16_t length, uint8_t *data)
{
    if (gAppEnv.targetWriteHandle != 0)
    {
        sonata_ble_gatt_write_no_response(conidx, gAppEnv.targetWriteHandle, 0, 0, length, data);
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetWriteHandle [%d] is wrong, can't write data\r\n", __FUNCTION__, gAppEnv.targetWriteHandle);
    }
    return false;
}

bool app_ble_master_read_data(uint8_t conidx, uint16_t hdl)
{
    if (hdl != 0)
    {
        sonata_ble_gatt_read_by_handle(conidx, hdl);
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetReadHandle [%d] is wrong, can't read data\r\n", __FUNCTION__, hdl);
    }
    return false;
}

bool app_ble_master_turn_ntf(uint8_t conidx, uint8_t hdl, bool on)
{
    if (hdl != 0)
    {
        APP_TRC("APP: %s,targetNtfHandle=%d, on=%d\r\n", __FUNCTION__, hdl,on);

        uint8_t config[2] = {0};
        if (on)
        {
            config[0] = 1;//NTF == 1
        }
        else
        {
            config[0] = 0;
        }
        sonata_ble_gatt_write(conidx, hdl, 0, 0, 2, config);
        return true;
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetNtfHandle [%d] is wrong, can't opt NTF\r\n", __FUNCTION__, gAppEnv.targetNtfHandle);
    }
    return false;
}

bool app_ble_master_turn_ind(uint8_t conidx, uint8_t hdl, bool on)
{
    if (hdl != 0)
    {
        APP_TRC("APP: %s,targetNtfHandle=%d, on=%d\r\n", __FUNCTION__, hdl,on);

        uint8_t config[2] = {0};
        if (on)
        {
            config[0] = 2;//IND == 2
        }
        else
        {
            config[0] = 0;
        }
        sonata_ble_gatt_write(conidx, hdl, 0, 0, 2, config);
        return true;
    }
    else
    {
        APP_TRC_ERROR("ERR: %s,targetNtfHandle [%d] is wrong, can't opt NTF\r\n", __FUNCTION__, gAppEnv.targetNtfHandle);
    }
    return false;
}

void app_ble_slave_send_data_via_ntf(uint8_t conidx, uint16_t data_len, uint8_t *data, bool log)
{
    dts_send_data(conidx, data, data_len, log);
}

void app_ble_change_conn_params()
{
    uint16_t intv_min = 6;
    uint16_t intv_max = 6;
    uint16_t time_out = 200;
    uint16_t latency = 200;
    APP_TRC_HIGH("APP: %s  . int=%d,lat=%d,timeout=%d\r\n", __FUNCTION__, intv_max, latency, time_out);
    sonata_ble_gap_update_connection_params(0, 0, intv_min, intv_max, latency, time_out, 0xFFFF, 0xFFFF);
}

void app_ble_disconnect(uint8_t conidx, uint8_t reason)
{
    sonata_ble_gap_disconnect(conidx, reason);
}


static uint8_t app_timer_handler(uint16_t id)
{
    //APP_TRC("APP: %s  . id=%d\r\n", __FUNCTION__, id);

    switch (id)
    {
        case APP_TIMER_READ_RSSI :
            dts_timer_handler(id);
            break;
        case APP_TIMER_TEST:
            gTimeStamp = sonata_get_sys_time() - gTimeStamp;
            APP_TRC("APP: %s  . gTimeStamp=%lu\r\n", __FUNCTION__, gTimeStamp);
            break;
        case APP_TIMER_ADV_DATA:
            app_ble_update_adv_data(++gWorkCounter);
//            app_ble_set_periodic_adv_data(++gWorkCounter);
            if (gWorkCounter == 0)
            {
                gWorkCounter = 1;
            }
            sonata_api_app_timer_set(APP_TIMER_ADV_DATA, 2000);
            break;
        case APP_TIMER_NTF_DATA:
            if (gAppEnv.gSendDataLock == 0)
            {
                gAppEnv.gSendDataLock = 1;
                app_cmd_ntf_timer_handler();
            }
            else
            {
                app_command_handle_ntf_complete_ind();
            }
            break;
        case APP_TIMER_CMD1:
        case APP_TIMER_CMD2:
        case APP_TIMER_CMD3:
        case APP_TIMER_OOB:
        case APP_TIMER_MCONN:
            app_cmd_timer_handler(id);
            break;
        default:
            APP_TRC("APP: %s  . No App Timer Handler!\r\n", __FUNCTION__);
            break;
    }
    return 0;
}





void app_reset()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    //Reset local parameter
    custom_svc_start_handle = 0;
    prf_add_diss = false;
    prf_add_bass = false;
    prf_add_ota = false;
    prf_add_dts = false;
    gExtentAdv = false;
    gPeroidAdv = false;
    gDirectAdv = false;
    gWorkCounter = 0;
    gWorkFlag = 0;
    APP_TRC("APP: %s  Done.\r\n", __FUNCTION__);
}


void app_set_adv_scan_response_data_flag(bool on)
{
    gAppEnv.bAdvWithScanResponse = on;
}


void app_att_db_push(uint16_t hdl, uint16_t uuid)
{
    if (hdl >= APP_ATT_DB_ITEM_MAX)
    {
        return;
    }
    gAttDb[hdl].uuid = uuid;
    gAttDb[hdl].hdl = hdl;
}

void app_att_db_update_range(uint16_t hdl, uint16_t uuid,uint8_t start, uint8_t end)
{
    if (hdl >= APP_ATT_DB_ITEM_MAX)
    {
        return;
    }
    gAttDb[hdl].start = start;
    gAttDb[hdl].end = end;
    gAttDb[hdl].svc_uuid = uuid;

}

void app_att_db_update_prop(uint16_t hdl, uint8_t prop)
{
    if (hdl >= APP_ATT_DB_ITEM_MAX)
    {
        return;
    }
    gAttDb[hdl].prop = prop;
}

att_db_item *app_att_db_search(uint16_t uuid)
{
    for (int i = 0; i < APP_ATT_DB_ITEM_MAX; ++i)
    {
        if (gAttDb[i].uuid == uuid)
        {
            return &gAttDb[i];
        }
    }
    return NULL;
}


att_db_item *app_att_db_get()
{
    return &gAttDb[0];
}

void app_att_db_clear()
{
    memset(gAttDb, 0, sizeof(gAttDb));
}


bool app_att_is_db_filled()
{
    if (gAttDb[1].uuid != 0)
    {
        return true;
    }
    return false;
}

bool app_is_dts_on()
{
    return (gAppEnv.gDtsOn == 1);
}



/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{
#if DEMO_PRF_DISS
    sonata_prf_diss_init();
#endif
#if DEMO_PRF_BASS
    sonata_prf_bass_init();
#endif
#if DEMO_PRF_OTA
    sonata_prf_otas_init();
#endif
}

#if defined(CFG_PLF_SONATA) || defined(CFG_PLF_RV32)
uint8_t app_config(uint8_t type, app_config_t * cfg)
{
    APP_TRC("APP: %s ,type=%d, \r\n", __FUNCTION__,type);
    if (type == 0 || cfg == NULL)
    {
        return CB_REJECT;
    }
    app_config_ext_t config = {0};
    app_nv_get_app_config(&config);
    switch (type)
    {
        case APP_CFG_COMPANY_ID:
            cfg->company_id = config.type == 0 ? 0x4321 : config.param.company_id;
            break;
        case APP_CFG_MANUF_NAME:
            cfg->manuf_name = config.type == 0 ? 0x1234 : config.param.manuf_name;
            break;
        case APP_CFG_GAP_ATTR_FLAG:
            if (config.type == APP_CFG_GAP_ATTR_FLAG)
            {
                APP_TRC("APP: Type=%d, Enable=%d, para=0x%04x, \r\n", config.type, config.enable, (uint16_t) config.param.gap_attr_cfg);
                if (config.enable == true)
                {
                    cfg->gap_attr_cfg |= config.param.gap_attr_cfg;
                }
                else
                {
                    cfg->gap_attr_cfg &= ~config.param.gap_attr_cfg;
                }
            }
            else
            {
                //Add reconnection address char
                cfg->gap_attr_cfg |= SONATA_GAP_DB_FLAG_RECONNECTION_ADDR;
                //Remove  Resolvable Private Address only char
                cfg->gap_attr_cfg &= ~SONATA_GAP_DB_FLAG_RSLV_PRIV_ADDR_ONLY;
            }
            break;
        case APP_CFG_GATT_ATTR_FLAG:
            if (config.type == APP_CFG_GATT_ATTR_FLAG)
            {
                APP_TRC("APP: Type=%d, Enable=%d, para=0x%04x, \r\n", config.type, config.enable, (uint16_t) config.param.gatt_attr_cfg);
                if (config.enable == true)
                {
                }
                else
                {
                    cfg->gatt_attr_cfg &= ~config.param.gatt_attr_cfg;
                }
            }
            else
            {
                cfg->gatt_attr_cfg &= ~SONATA_GATT_DB_FLAG_SVC_CHANGED;
                cfg->gatt_attr_cfg &= ~SONATA_GATT_DB_FLAG_CLI_SUP_FEAT;
            }
            break;
        default:
            break;
    }
    return CB_DONE;

}
#endif

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */



static ble_gap_callback ble_gap_callbacks = {
    /*************** GAP Manager's callback ***************/
    //Optional, use for get local devcie informations when call sonata_ble_get_dev_info()
    .get_local_dev_info                 = app_get_dev_info_callback,
    .gap_scan_result                    = app_gap_scan_result_callback,


    /*************** GAP Controller's callback  ***************/
    //Optional
    .gap_param_update_req               = app_gap_param_update_req_callback,
    //Optional
    .gap_param_updated                  = app_gap_param_updated_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_info                  = app_gap_peer_info_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_att_info              = app_gap_peer_att_info_callback,
    //Optional, if peer device get local device's information, app can deal with it in this callback
    .gap_peer_get_local_info            = app_gap_peer_get_local_info_callback,
    //Optional
    .gap_disconnect_ind                 = app_gap_disconnect_ind_callback,
    //Optional, if peer device set local device's name, app can deal with it in this callback
    .gap_peer_set_local_device_name     = app_gap_peer_set_local_device_name_callback,
    //Optional, app can save peer mac address in this callback when connected
    .gap_connection_req                 = app_gap_connection_req_callback,
    .gap_le_pkt_size                    = app_gap_le_pke_size_callback,
    .gap_sync_established               = app_gap_period_sync_callback,
#if (USE_APP_SEC)
    .gap_bond_req                       = app_sec_gap_bond_req_callback,
    .gap_bond                           = app_sec_gap_bond_callback,
    .gap_encrypt_req                    = app_sec_gap_encrypt_req_callback,
    .gap_gen_random_number              = app_sec_gap_gen_random_number_callback,
    .gap_security                       = app_sec_gap_security_callback,
    .gap_encrypt                        = app_sec_gap_encrypt_callback,
    .gap_resolving_address              = app_sec_gap_resolving_address_callback,
    .gap_addr_solved                    = app_sec_address_resolved_callback,
#endif



};

static ble_gatt_callback ble_gatt_callbacks = {
    //Optional, add this callback if app need to save changed mtu value
    .gatt_mtu_changed                   = app_gatt_mtu_changed_callback,
    //Must,If app add custom service, app should add this callback to deal with peer device read request
    .gatt_read_req                      = app_gatt_read_request_callback,
    .gatt_read                          = app_gatt_read_callback,
    //Must,If app add custom service, app should add this callback to deal with peer device write request
    .gatt_write_req                     = app_gatt_write_request_callback,
    //Must if use discovery all servcie function
    .gatt_disc_svc                      = app_gatt_disc_svc_callback,
    //Must if use discovery all characteristic function
    .gatt_disc_char                     = app_gatt_disc_char_callback,
    //Must if use discovery all description function
    .gatt_disc_char_desc                = app_gatt_disc_char_desc_callback,
    //Reception of an indication or notification from peer device
    .gatt_event                         = app_gatt_event_callback,
    .gatt_event_req                     = app_gatt_event_req_callback,
    .gatt_connection_info               = app_gatt_connection_info_callback,
    .gatt_att_info_req                  = app_gatt_att_info_req_callback,
};

static ble_complete_callback ble_complete_callbacks = {
    //Must, app can do next operation in this callback
    .ble_complete_event                 = app_ble_complete_event_handler,
};

static ble_response_callback ble_rsp_callbacks = {
    //Must,IF app add custom service, add should save this service's start handler id,
    //this id will be used in app_gatt_read_request_callback() and app_gatt_write_request_callback()
    .ble_rsp_event                      = app_ble_rsp_event_handler,
};
#if (DEMO_PRF_OTA)
static prf_ota_callback_t ble_prf_ota_callbacks = {
        .prf_ota_before_start     = app_ota_before_start_callback,
        .prf_ota_start            = app_ota_start_callback,
        .prf_ota_stop             = app_ota_stop_callback,
        .prf_ota_fail             = app_ota_fail_callback,
};
#endif


static sonata_app_timer_callback_t app_timer_callbacks = {
        .timeout                  = app_timer_handler,
};

static const sonata_api_app_msg_t myMsgs[] = {
        {APP_EVENT_UART_CMD,    app_uart_cmd_handler},

};
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_init(void)
{
    APP_TRC_HIGH("--------------------------------\r\n");
    APP_TRC_HIGH("     ble_sonata_sdk_demo      \r\n");
    APP_TRC_HIGH("--------------------------------\r\n");

//    asr_flash_erase(PARTITION_PARAMETER_5, 0, NVDS_MAX_SIZE);

    sonata_log_level_set(SONATA_LOG_VERBOSE);

    sonata_ble_register_gap_callback(&ble_gap_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);
    sonata_ble_register_response_callback(&ble_rsp_callbacks);
    #if DEMO_PRF_OTA
    sonata_prf_ota_register_callback(&ble_prf_ota_callbacks);
    #endif
    sonata_api_register_app_timer_callback(&app_timer_callbacks);
    memset(&gAppEnv, 0, sizeof(gAppEnv));
    app_cmd_init_uart_at();
    #ifdef DEMO_RANDOM_RESLOVE_ADDRESS
        app_ble_gen_rand_addr();
    #else
    app_ble_on();
    #endif
    int size = sizeof(myMsgs) / sizeof(sonata_api_app_msg_t);
    for (int i = 0; i < size; ++i)
    {
        sonata_api_app_msg_register(&myMsgs[i]);
    }
}

void app_ble_set_ble_open(uint16_t value)
{
    APP_TRC("APP: %s, value=%d\r\n", __FUNCTION__, value);
    extern uint8_t ble_open;
    ble_open = value; //No use in this code
    {
        extern void atcmd_set_ble_open(uint8_t value) ;
        atcmd_set_ble_open(1);
    }

}


/// @} APP
