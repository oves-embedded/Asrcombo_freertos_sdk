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
 * @file (app_uart.c)
 *
 * @brief
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_uart.h"
#include "app.h"
#include "sonata_ble_api.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */







#define MIN_USEFUL_DEC                  (32)
#define MAX_USEFUL_DEC                  (127)
#define SONATA_UART_RXBUF_LEN           (50)
#define SONATA_UART_CMD_COUNT           (2)

typedef void(*handled)(uint32_t dwpara);

typedef struct uart_data
{
    uint16_t length;
    uint8_t *data;
    handled pHandled;
} uart_data_t;

uint8_t app_uart_cmd_handler(void *param);
static uint8_t uart_idx = 0;
static uint8_t cur_idx = 0;
static uint8_t start_idx = 0;
static uint8_t sonata_uart_buf_len[SONATA_UART_CMD_COUNT] = {0};
static uint8_t sonata_uart_buf_ble[SONATA_UART_CMD_COUNT][SONATA_UART_RXBUF_LEN];
//static uint16_t gStartHandleId = 0;
static uart_data_t uartData = {0};

extern uint8_t targetAddr1[SONATA_GAP_BD_ADDR_LEN];

uint8_t hexChar2Value(char ch)
{
    uint8_t result = 0;

    if (ch >= '0' && ch <= '9')
    {
        result = ch - '0';
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        result = (ch - 'a') + 10;
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
        result = (ch - 'A') + 10;
    }
    else
    {
        result = -1;
    }
    return result;
}



void app_uart_data_adapter(uint16_t length, const uint8_t *value)
{
    if (strncmp((char *) value, "MAC", 3) == 0) //MACAB89674523001
    {
        uint8_t d0, d1, d2, d3, d4, d5;
        d0 = hexChar2Value(value[3]) * 16 + hexChar2Value(value[4]);
        d1 = hexChar2Value(value[5]) * 16 + hexChar2Value(value[6]);
        d2 = hexChar2Value(value[7]) * 16 + hexChar2Value(value[8]);
        d3 = hexChar2Value(value[9]) * 16 + hexChar2Value(value[10]);
        d4 = hexChar2Value(value[11]) * 16 + hexChar2Value(value[12]);
        d5 = hexChar2Value(value[13]) * 16 + hexChar2Value(value[14]);
        APP_TRC("APPUART: %s  MAC   =%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__, d0, d1, d2, d3, d4, d5);
        targetAddr1[0] = d5;//MAC in stack should reverse with Real MAC
        targetAddr1[1] = d4;
        targetAddr1[2] = d3;
        targetAddr1[3] = d2;
        targetAddr1[4] = d1;
        targetAddr1[5] = d0;
        app_ble_config_initiating();
    }
}



uint8_t app_uart_cmd_handler(void *param)
{
    uart_data_t *data = (uart_data_t *) param;
    APP_TRC("APP: %s  . \r\n", __FUNCTION__);
    if (data != NULL)
    {
        app_uart_data_adapter(0, data->data);
        data->pHandled(0);//Should clear UART data for next UART input
    }
    return 0;
}

void app_uartirq_command_done(uint32_t dwparam)
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

void app_uartirq_handler(char ch)
{
    //APP_TRC("APP: %s  . \r\n", __FUNCTION__);
    if ((start_idx + 1) % SONATA_UART_CMD_COUNT != cur_idx)
    {
        if (ch == '\r')//in windows platform '\r\n' added to end
        {
            sonata_uart_buf_ble[start_idx][uart_idx] = '\0';
            sonata_uart_buf_len[start_idx] = uart_idx;
            //print_command();
            uart_idx = 0;
            uartData.length = sonata_uart_buf_len[start_idx];
            uartData.data = sonata_uart_buf_ble[start_idx];
            uartData.pHandled = app_uartirq_command_done;
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




