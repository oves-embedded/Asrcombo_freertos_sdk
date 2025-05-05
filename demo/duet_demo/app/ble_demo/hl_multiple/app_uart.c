
/**
 ****************************************************************************************
 *
 * @file (app_uart.c)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_uart.h"
#include <stdio.h>
#include "app.h"
#include "arch.h"
#include "app_command.h"
#include "sonata_utils_api.h"


/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define DEMO_UART_TIMER             (1)
#define TIMER_UART_AUTO_SEND        (100)

#define MIN_USEFUL_DEC              (32)
#define MAX_USEFUL_DEC              (127)

typedef void(*handled)(uint32_t dwpara);



uint8_t dts_uart_cmd_handler(void *param);


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
static uint16_t uart_idx = 0;
static uint16_t cur_idx = 0;
static uint16_t start_idx = 0;
static uint8_t uart_buf_len[COUNT_UART_CMD] = {0};
static uint8_t uart_buf[COUNT_UART_CMD][LEN_UART_RXBUF];
static uart_data_t uartData = {0};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
uint8_t app_uart_timer_handler(uint16_t id)
{
    switch (id)
    {
        case APP_TIMER_UART:
        {
            if (uart_idx != 0)
            {
                app_uart_irq_handler('\r');
            }
            app_uart_irq_timer_start();
            break;
        }

        default:
            break;

    }
    return 0;
}

CRITICAL_FUNC_SEG uint8_t app_uart_cmd_handler(void *param)
{
    uart_data_t *data = (uart_data_t *) param;
    if (data != NULL)
    {
        app_cmd_data_adapter(data->length, data->data);
        app_uart_irq_command_done(0);
    }
    return 0;
}



CRITICAL_FUNC_SEG uint8_t app_uart_cmd_handler_local()
{
    uart_data_t *data = (uart_data_t *) &uartData;
    if (data != NULL)
    {
        app_cmd_data_adapter(data->length, data->data);
    }
    return 0;
}

void print_command()
{
    if (0)
    {
        printf("\r\n");
        printf("%s  [%02d]:", __FUNCTION__, uart_idx);
        for (int i = 0; i < uart_idx; ++i)
        {
            printf("%c", uart_buf[start_idx][i]);
        }
        printf("\r\n");
    }
}



CRITICAL_FUNC_SEG void app_uart_irq_command_done(uint32_t dwparam)
{
    memset(uart_buf[cur_idx], 0, LEN_UART_RXBUF);
    if (cur_idx == COUNT_UART_CMD - 1)
    {
        cur_idx = 0;
    }
    else
    {
        cur_idx++;
    }
}

CRITICAL_FUNC_SEG void app_uart_irq_handler(char ch)
{
    if ((start_idx + 1) % COUNT_UART_CMD != cur_idx)
    {
        if (ch == '\r' || ch == '\0')//in windows platform '\r\n' added to end
        {
            uart_buf[start_idx][uart_idx] = '\0';
            uart_buf_len[start_idx] = uart_idx;
            print_command();
            uart_idx = 0;
            uartData.length = uart_buf_len[start_idx];
            uartData.data = uart_buf[start_idx];
            uartData.pHandled = app_uart_irq_command_done;
            if (start_idx == COUNT_UART_CMD - 1)
            {
                start_idx = 0;
            }
            else
            {
                start_idx++;
            }

            // send message to ble core stack
            if (0)
            {
                sonata_api_send_app_msg(APP_MSG_UART_CMD, &uartData);
            }
            else
            {
                app_uart_cmd_handler_local();
                app_uart_irq_command_done(0);
            }

        }
        else if (ch == '\b')
        {
            if (uart_idx > 0)
            {
                uart_buf[start_idx][--uart_idx] = '\0';
            }
        }
        else if (ch >= MIN_USEFUL_DEC && ch <= MAX_USEFUL_DEC)
        {
            uart_buf[start_idx][uart_idx++] = ch;
            if (uart_idx > LEN_UART_RXBUF - 1)
            {
                //dbg_at("error:uart char_str length must <= 127,return\n");
                uart_idx = 0;
            }
        }
    }

}




void app_uart_irq_timer_start()
{
    //APP_TRC("UART: %s\r\n", __FUNCTION__);
    #if DEMO_UART_TIMER
    sonata_api_app_timer_clear(APP_TIMER_UART);
    sonata_api_app_timer_set(APP_TIMER_UART, TIMER_UART_AUTO_SEND);
    uart_idx = 0;
    #endif
}

void app_uart_irq_timer_stop()
{
    APP_TRC("UART: %s \r\n", __FUNCTION__);
    uart_idx = 0;
    //app_set_dt_mode(false);
    sonata_api_app_timer_clear(APP_TIMER_UART);
}


