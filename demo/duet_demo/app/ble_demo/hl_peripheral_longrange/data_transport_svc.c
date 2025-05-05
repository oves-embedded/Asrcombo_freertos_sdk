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
 * @file data_transport_svc.c
 *
 * @brief Data transport service
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "data_transport_svc.h"
#include <stdio.h>
#include "sonata_gatt_api.h"
#include "app.h"
#include "sonata_utils_api.h"
#include "sonata_ble_api.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */

#define DTSSVC "DTS"

#define MIN_USEFUL_DEC                  (32)
#define MAX_USEFUL_DEC                  (127)
#define SONATA_UART_RXBUF_LEN           (50)
#define SONATA_UART_CMD_COUNT           (2)


#define NUM_PKG       (10)
#define NUM_PKG_SIZE  (10)


#define NUM_READ_RSSI_TIMER (2000)

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
 * Type DEFINITIONS
 ****************************************************************************************
 */
typedef void(*handled)(uint32_t dwpara);

typedef struct uart_data
{
    uint16_t length;
    uint8_t *data;
    handled pHandled;
} uart_data_t;


uint8_t dts_uart_cmd_handler(void *param);

static void gen_value();

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

static uint8_t uart_idx = 0;
static uint8_t cur_idx = 0;
static uint8_t start_idx = 0;
static uint8_t sonata_uart_buf_len[SONATA_UART_CMD_COUNT] = {0};
static uint8_t sonata_uart_buf_ble[SONATA_UART_CMD_COUNT][SONATA_UART_RXBUF_LEN];
static uint16_t start_handle_id = 0;
static uint8_t custom_value[NUM_PKG_SIZE] = {0};
static uint8_t count = 0;
static uint16_t app_mtu = 20;
static bool tx_ready = false;
static uart_data_t uartData = {0};

///UUID defines
uint8_t dts_uuid[] = {0x0A, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
///DB defines
struct sonata_gatt_att_desc dts_atts[DTS_IDX_NB] = {
                            // uuid,              perm,                 maxLen, ext_perm
        [DTS_CHAR_1]        =   {{0X03, 0X28, 0}, PRD_NA,                2,  0},
        [DTS_SIMP_1_VAL]    =   {{0X01, 0XB0, 0}, PRD_NA|PWR_NA,         80, PRI},
        [DTS_CHAR_1_DSEC]   =   {{0X01, 0X29, 0}, PRD_NA|PWR_NA,         2,  0},

        [DTS_CHAR_2]        =   {{0X03, 0X28, 0}, PRD_NA,                2, 0},
        [DTS_SIMP_2_VAL]    =   {{0X02, 0XB0, 0}, PRD_NA|PWR_NA ,        80,  PRI},

        [DTS_CHAR_3]        =   {{0X03, 0X28, 0}, PRD_NA,                2, 0},
        [DTS_SIMP_3_VAL]    =   {{0X03, 0XB0, 0}, PNTF_NA|PRD_NA|PWR_NA, 80, PRI},
        [DTS_CHAR_3_CFG]    =   {{0X02, 0X29, 0}, PRD_NA|PWR_NA,         2,  PRI},

        [DTS_CHAR_4]        =   {{0X03, 0X28, 0}, PRD_NA,                20, 0},
        [DTS_SIMP_4_VAL]    =   {{0X04, 0XB0, 0}, PRD_NA|PWR_NA|PIND_NA, 80,  PRI},
        [DTS_CHAR_4_CFG]    =   {{0X02, 0X29, 0}, PRD_NA|PWR_NA,         2,  PRI},




};

///UART callback function. Chars in UART will be handled here. The parameter see @uart_data_t
static const sonata_api_app_msg_t myMsgs[] = {
        {APP_MSG_UART_CMD, dts_uart_cmd_handler},
};


/*
* FUNCTION DEFINITIONS
****************************************************************************************
*/
/*!
 * @brief DTS should know MTU value, and  the real mtu is "app_mtu = mtu -3"
 * @param mtu
 */
void dts_set_mtu(uint16_t mtu)
{
    app_mtu = mtu - (SONATA_ATT_CODE_LEN + SONATA_ATT_HANDLE_LEN);
    if (app_mtu > NUM_PKG_SIZE)
    {
        app_mtu = NUM_PKG_SIZE;
    }
}
/*!
 * @brief Demo function for sending data
 * @param param
 * @return
 */
uint8_t dts_pkg_send_handler(void *param)
{
    APP_TRC("APPSVC: %s  count=%d:", __FUNCTION__, count);
    if (count >= NUM_PKG)
    {
        APP_TRC("APPSVC: %s  END", __FUNCTION__);
        return 0;
    }
    gen_value();
    sonata_ble_gatt_send_notify_event(app_get_connection_id(), (start_handle_id + DTS_SIMP_3_VAL + 1), app_mtu, custom_value);
    count++;
    return 0;
}

void dts_send_dec_data(uint8_t data)
{
    APP_TRC("APPSVC: %s  count=%d \r\n", __FUNCTION__, count);
    custom_value[0] = data;
    uint8_t data1 = data / 100;
    uint8_t data2 = (data - data1 * 100) / 10;
    uint8_t data3 = (data % 100) % 10;
    custom_value[0] = data1;
    custom_value[1] = data2;
    custom_value[2] = data3;

    sonata_ble_gatt_send_notify_event(app_get_connection_id(), (start_handle_id + DTS_SIMP_3_VAL + 1), app_mtu, custom_value);
    count++;
}
/*!
 * @brief Send String getting form UART via GATT
 * @param param @see uart_data_t
 * @return
 */
uint8_t dts_uart_cmd_handler(void *param)
{
    uart_data_t *data = (uart_data_t *) param;
    if (data != NULL)
    {
        uint16_t len = util_min(app_mtu, data->length);
        APP_TRC("APPSVC: %s  cmdLen=%d,app_mtu=%d\r\n", __FUNCTION__, data->length, app_mtu);
        if (tx_ready == true)
        {
            sonata_ble_gatt_send_notify_event(app_get_connection_id(), (start_handle_id + DTS_SIMP_3_VAL + 1), len, data->data);
        }
        else
        {
            APP_TRC("APPSVC: %s  TX Not ready\r\n", __FUNCTION__);
        }

        APP_TRC("APPSVC: %s  data->data[0]=%d\r\n", __FUNCTION__,data->data[0]);
        if (data->data[0] == '0') //Here can do some command for example
        {
            sonata_ble_gap_disconnect(0, 0);
        }
        data->pHandled(0);//Should clear UART data for next UART input
    }
    return 0;
}

/*!
 * @brief Add DTS service
 * @return
 */
uint16_t dts_add()
{
    APP_TRC("APPSVC: %s  \r\n", __FUNCTION__);
    uint16_t start_hdl = 0;//(0 = dynamically allocated)
    uint8_t perm = 0;
    uint8_t nb_att = DTS_IDX_NB;
    uint8_t *uuid = dts_uuid;
    struct sonata_gatt_att_desc *atts = dts_atts;
    uint16_t retval = sonata_ble_gatt_add_service(start_hdl, perm, uuid, nb_att, atts);

    int size = sizeof(myMsgs) / sizeof(sonata_api_app_msg_t);
    for (int i = 0; i < size; ++i)
    {
        sonata_api_app_msg_register(&myMsgs[i]);
    }
    return retval;
}
/*!
 * @brief DTS read request
 * @param connection_id
 * @param handle
 * @return
 */
uint16_t dts_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,\r\n", __FUNCTION__, localHandle);
    switch (localHandle)
    {
          case DTS_SIMP_1_VAL:
          {
              uint8_t localValue[] = "Char 1 val";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
          case DTS_CHAR_1_DSEC:
          {
              uint8_t localValue[] = "Char 1 Desc";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;

          case DTS_SIMP_2_VAL:
          {
              uint8_t localValue[] = "Char 2 Val";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
          case DTS_SIMP_3_VAL:
          {
              uint8_t localValue[] = "Char 3 Val";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
          case DTS_CHAR_3_CFG:
          {
              uint8_t localValue[] = "Char 3 Cfg";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
          case DTS_SIMP_4_VAL:
          {
              uint8_t localValue[] = "Char 4 Val";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
          case DTS_CHAR_4_CFG:
          {
              uint8_t localValue[] = "Char 4 Cfg";
              sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
              return CB_DONE;
          }
              break;
        default:
            break;
    }
    return CB_REJECT;
}
/*!
 * @brief Generate some value
 */
static void gen_value()
{
    for (int j = 0; j < NUM_PKG_SIZE; ++j)
    {
        custom_value[j] = count;
    }
}
/*!
 * @brief DTS write request
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
uint16_t dts_write_request_handler(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,\r\n", __FUNCTION__, localHandle, handle, offset);
    switch (localHandle)
    {

        case DTS_SIMP_1_VAL:
        {
            APP_TRC(" >>>Char 1 Val Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;
        case DTS_CHAR_1_DSEC:
        {
            APP_TRC(" >>>Char 1 Desc Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;


        case DTS_SIMP_2_VAL:
        {
            APP_TRC(" >>>Char 2 Val Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;

        case DTS_SIMP_3_VAL:
        {
            APP_TRC(" >>>Char 3 Val Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;
        case DTS_CHAR_3_CFG:
        {
            APP_TRC(" >>>Char 1 Cfg Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;


        case DTS_SIMP_4_VAL:
        {
            APP_TRC(" >>>Char 4 Val Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;
        case DTS_CHAR_4_CFG:
        {
            APP_TRC(" >>>Char 4 Cfg Set Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;

//        case DTS_CHAR_3_CFG:
//        case DTS_CHAR_4_CFG:
//        {
//            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
//            uint16_t ntf_cfg = value[0];
//            if (ntf_cfg == SONATA_PRF_CLI_STOP_NTFIND)
//            {
//                count = 0;
//                APP_TRC("APP_SVC: NTF Stopped\r\n");
//                tx_ready = false;
//                sonata_api_app_timer_clear(APP_TIMER_READ_RSSI);
//
//            }
//            else if (ntf_cfg == SONATA_PRF_CLI_START_NTF)
//            {
//                APP_TRC("APP_SVC: NTF Started. start_handle_id=%d\r\n", start_handle_id);
//                tx_ready = true;
//                //Use random value for example
//                count = 0;
//                if (DEMO_SEND_RSSI)
//                {
//                    sonata_api_app_timer_set(APP_TIMER_READ_RSSI, NUM_READ_RSSI_TIMER);
//                }
//                else
//                {
//                    gen_value();
//                    dts_pkg_send_handler(NULL);
//                }
//            }
//            break;
//        }
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);

            break;
    }


    return CB_DONE;
}

/*!
 * @brief Seav DTS start handle id
 * @param handle
 */
void dts_set_start_handle(uint16_t handle)
{
    start_handle_id = handle;
}
/*!
 * @brief Interface for sending data done
 * @param status
 */
void dts_data_send_done(uint8_t status)
{
    if (DEMO_SEND_RSSI)
    {
        sonata_api_app_timer_set(APP_TIMER_READ_RSSI, NUM_READ_RSSI_TIMER);
    }
    else
    {
        dts_pkg_send_handler(NULL);
    }

}


uint8_t dts_get_preferred_mtu()
{
    return NUM_PKG_SIZE;
}


uint8_t dts_timer_handler(uint16_t id)
{
    APP_TRC("APP: %s ->", __FUNCTION__);
    switch (id)
    {
        case APP_TIMER_READ_RSSI:
        {
            app_ble_get_peer_rssi();
            break;
        }

        default:
            break;

    }
    return 0;
}



/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void print_command()
{
    if (0)
    {
        printf("\r\n");
        printf("%s  [%02d]:", __FUNCTION__, uart_idx);
        for (int i = 0; i < uart_idx; ++i)
        {
            printf("%c", sonata_uart_buf_ble[start_idx][i]);
        }
        printf("\r\n");
    }
}



void dts_uartirq_command_done(uint32_t dwparam)
{
    memset(sonata_uart_buf_ble[cur_idx], 0, SONATA_UART_RXBUF_LEN);
    if (cur_idx == SONATA_UART_CMD_COUNT - 1)
    {
        cur_idx = 0;
    }
    else
    {
        cur_idx++;
    }
}

void dts_uartirq_handler(char ch)
{
    if ((start_idx + 1) % SONATA_UART_CMD_COUNT != cur_idx)
    {
        if (ch == '\r')//in windows platform '\r\n' added to end
        {
            sonata_uart_buf_ble[start_idx][uart_idx] = '\0';
            sonata_uart_buf_len[start_idx] = uart_idx;
            print_command();
            uart_idx = 0;
            uartData.length = sonata_uart_buf_len[start_idx];
            uartData.data = sonata_uart_buf_ble[start_idx];
            uartData.pHandled = dts_uartirq_command_done;
            if (start_idx == SONATA_UART_CMD_COUNT - 1)
            {
                start_idx = 0;
            }
            else
            {
                start_idx++;
            }

            // send message to ble core stack
            sonata_api_send_app_msg(APP_MSG_UART_CMD, &uartData);

        }
        else if (ch == '\b')
        {
            if (uart_idx > 0)
            {
                sonata_uart_buf_ble[start_idx][--uart_idx] = '\0';
            }
        }
        else if (ch >= MIN_USEFUL_DEC && ch <= MAX_USEFUL_DEC)
        {
            sonata_uart_buf_ble[start_idx][uart_idx++] = ch;
            if (uart_idx > SONATA_UART_RXBUF_LEN - 1)
            {
                //dbg_at("error:uart char_str length must <= 127,return\n");
                uart_idx = 0;
            }
        }
    }
}







