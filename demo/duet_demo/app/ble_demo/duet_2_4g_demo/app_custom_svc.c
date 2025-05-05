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
 * @file \app_custom_svc.c
 *
 * @brief custom service db
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_custom_svc.h"
#include <stdio.h>
#include "sonata_gatt_api.h"
#include "app.h"

#ifdef CLOUD_JOYLINK_SUPPORT
#include "joylink_sdk.h"
#include "joylink_adapter.h"
#include "joylink_utils.h"
#endif

/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define APPSVC "APP_SVC"
/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */
/// Possible values for setting client configuration characteristics
enum sonata_prf_cli_conf
{
    /// Stop notification/indication
    SONATA_PRF_CLI_STOP_NTFIND = 0x0000,
    /// Start notification
    SONATA_PRF_CLI_START_NTF,
    /// Start indication
    SONATA_PRF_CLI_START_IND,
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
#ifdef CLOUD_JOYLINK_SUPPORT
uint8_t csvc_uuid[] = {0x70, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct sonata_gatt_att_desc csvc_atts[CSVC_IDX_NB] = {
        // uuid,                                   permission,              maxLen, ext_perm
        //ATT_DECL_CHARACTERISTIC (Read)
        [CSVC_IDX_1_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_1_VAL]     =   {{0X71, 0XFE, 0}, PWR_NA,                  128,    PRI},

        //ATT_DECL_CHARACTERISTIC (Indicate,Read,Write)
        [CSVC_IDX_2_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_2_IND_VAL] =   {{0X72, 0XFE, 0}, PIND_NA|PRD_NA|PWR_NA,   128,    PRI},
        [CSVC_IDX_2_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},
};
#else
uint8_t csvc_uuid[] = {0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct sonata_gatt_att_desc csvc_atts[CSVC_IDX_NB] = {
        // uuid,                                   perm,                    maxLen, ext_perm
        //ATT_DECL_CHARACTERISTIC (Read)
        [CSVC_IDX_1_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_1_VAL]     =   {{0XF1, 0XFF, 0}, PRD_NA,                  128,    PRI},

        //ATT_DECL_CHARACTERISTIC (Read,Write)
        [CSVC_IDX_2_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_2_VAL]     =   {{0XF2, 0XFF, 0}, PRD_NA|PWR_NA,           128,    PRI},
        //ATT_DESC_CLIENT_CHAR_CFG (Notify)
        [CSVC_IDX_3_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [CSVC_IDX_3_NTF_VAL] =   {{0XAA, 0XAA, 0}, PNTF_NA|PRD_NA|PWR_NA,   2,      PRI},
        [CSVC_IDX_3_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},

        [CSVC_IDX_4_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [CSVC_IDX_4_IND_VAL] =   {{0XBB, 0XBB, 0}, PIND_NA|PRD_NA|PWR_NA,   2,      PRI},
        [CSVC_IDX_4_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},

};
#endif

uint16_t start_handle_id = 0;

/*
* FUNCTION DEFINITIONS
****************************************************************************************
*/
#ifdef CLOUD_JOYLINK_SUPPORT
int app_jl_adapter_send_data(uint8_t*data, uint32_t data_len)//SDK: Call this interface to send data, indicate send function to achieve the interface
{
  APP_TRC(" >>>xxx_send_data len = %ld Data:", data_len);
  for (int i = 0; i < data_len; ++i)
  {
      APP_TRC("%02X ", data[i]);
  }
  APP_TRC("\r\n");

  uint8_t connect_idx = 0x00;
  int ret = sonata_ble_gatt_send_indicate_event(connect_idx, (start_handle_id + CSVC_IDX_2_IND_VAL + 1), data_len, data);

  APP_TRC("sonata_ble_gatt_send_indicate_event ret = %d\r\n", ret);

  return ret;
}
#endif

uint16_t app_custom_svc_add_request()
{
    //APP_TRC("APPSVC: %s  \r\n", __FUNCTION__);
    uint16_t start_hdl = 0;//(0 = dynamically allocated)
    uint8_t perm = 0;
    uint8_t nb_att = CSVC_IDX_NB;
    uint8_t *uuid = csvc_uuid;
    struct sonata_gatt_att_desc *atts = csvc_atts;
    uint16_t retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, nb_att, atts);

    return retval;
}

#ifdef CLOUD_JOYLINK_SUPPORT
uint16_t app_custom_svc_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,\r\n", __FUNCTION__, localHandle);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        {
            uint8_t localValue[] = "C1 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_2_IND_VAL:
        {
            uint8_t localValue[] = "C2 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        default:
            break;
    }

    return CB_REJECT;
}

uint16_t app_custom_svc_write_request_handler(uint8_t connection_id,  uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, connection_id = 0x%02x, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,\r\n", __FUNCTION__, connection_id, localHandle, handle, offset);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        case CSVC_IDX_2_IND_VAL:
        case CSVC_IDX_2_CFG:
        {
            APP_TRC(" >>>App Get Data len = %d Data:", length);
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X ", value[i]);
            }
            APP_TRC("\r\n");

            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

            if (length > 2)
            {
              int32_t ret = jl_write_data((uint8_t*)value, length);
              if (ret) {
                  APP_TRC("jl_write_data failed !\n");
              }
            }
        }
            break;
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);
            break;
    }

    return CB_DONE;
}
#else
uint16_t app_custom_svc_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,\r\n", __FUNCTION__, localHandle);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        {
            uint8_t localValue[] = "C1 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_2_VAL:
        {
            uint8_t localValue[] = "C2 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_3_NTF_VAL:
        {
            uint8_t localValue[] = "C3 Notify";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_3_CFG:
        {
            uint8_t localValue[] = "C3 config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_4_IND_VAL:
        {
            uint8_t localValue[] = "C4 Indicate";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_4_CFG:
        {
            uint8_t localValue[] = "C4 Config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        default:
            break;
    }


    return CB_REJECT;
}

uint16_t app_custom_svc_write_request_handler(uint8_t connection_id,  uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,\r\n", __FUNCTION__, localHandle, handle, offset);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        case CSVC_IDX_2_VAL:
        case CSVC_IDX_3_NTF_VAL:
        case CSVC_IDX_4_IND_VAL:
        {
            APP_TRC(" >>>App Get Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;
        case CSVC_IDX_3_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ntf_cfg = value[0];
            if (ntf_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("APP_SVC: NTF Stopped\r\n");
            }
            else if (ntf_cfg == SONATA_PRF_CLI_START_NTF)
            {
                APP_TRC("APP_SVC: NTF Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                //sonata_ble_gatt_send_notify_event(connection_id, handle-(CSVC_IDX_3_VAL-CSVC_IDX_3_NTF_CFG), 2, localvalue);
                sonata_ble_gatt_send_notify_event(connection_id, (start_handle_id + CSVC_IDX_3_NTF_VAL + 1), 2, localvalue);
            }
            break;
        }
        case CSVC_IDX_4_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ind_cfg = value[0];
            if (ind_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("APP_SVC: IND Stopped\r\n");
            }
            else if (ind_cfg == SONATA_PRF_CLI_START_IND)
            {
                APP_TRC("APP_SVC: IND Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                sonata_ble_gatt_send_indicate_event(connection_id, (start_handle_id + CSVC_IDX_4_IND_VAL + 1), 2, localvalue);
            }
            break;
        }
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);

            break;
    }


    return CB_DONE;
}
#endif


void app_custom_svc_set_start_handle(uint16_t handle)
{
    start_handle_id = handle;
}

