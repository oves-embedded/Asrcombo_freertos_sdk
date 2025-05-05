
/**
 ****************************************************************************************
 *
 * @file (app_uart.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */
#ifndef _APP_UART_H
#define _APP_UART_H

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app.h"


/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define LEN_UART_RXBUF              (200)
#define COUNT_UART_CMD              (2)

/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
typedef void(*handled)(uint32_t dwpara);

typedef struct uart_data
{
    uint16_t length;
    uint8_t *data;
    handled pHandled;
} uart_data_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */



uint8_t app_uart_cmd_handler_dtmode();

void app_uart_irq_handler(char ch);

void app_uart_irq_command_done(uint32_t dwparam);

uint8_t app_uart_timer_handler(uint16_t id);

void app_uart_irq_timer_start();

void app_uart_irq_timer_stop();

uint8_t app_uart_cmd_handler_local();

uint8_t app_uart_cmd_handler(void *param);


#endif //BLE_CODE_APP_UART_H
