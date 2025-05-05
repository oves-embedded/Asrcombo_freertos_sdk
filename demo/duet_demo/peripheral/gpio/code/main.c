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

#include <stdio.h>
#include "duet_pinmux.h"
#include "duet_uart.h"
#include "duet_gpio.h"
#include "printf_uart.h"

void HardFault_Handler(void)
{
    while(1);
}

extern duet_uart_callback_func g_duet_uart_callback_handler[UART_NUM];

#define UART_TEST_INDEX UART1_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0

void uart_recv_test(char data)
{
    //UART_SendData(data);
    UART_SendData(getUartxViaIdx(UART_TEST_INDEX),data);
}


void set_uart_config(uint8_t uart_idx,uint8_t hw_flow_control)
{
    duet_uart_dev_t uart_config_struct  = {0};
    duet_uart_struct_init(&uart_config_struct);
    uart_config_struct.port = uart_idx;
    uart_config_struct.priv = uart_recv_test;
//set user define config
    uart_config_struct.config.baud_rate = UART_BAUDRATE_115200;
    if(hw_flow_control)
        uart_config_struct.config.flow_control = FLOW_CTRL_CTS_RTS;
    else
        uart_config_struct.config.flow_control = FLOW_CTRL_DISABLED;
//make config take effect
    duet_uart_init(&uart_config_struct);
//    duet_uart_set_callback(uart_idx,uart_recv_test);

}

void set_uart_pinmux(uint8_t uart_idx,uint8_t hw_flow_control)
{
    switch(uart_idx){
        case UART0_INDEX:
            duet_pinmux_config(PAD0,PF_UART0);
            duet_pinmux_config(PAD1,PF_UART0);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD6,PF_UART0);
                duet_pinmux_config(PAD7,PF_UART0);
            }
            break;
        case UART1_INDEX:
            duet_pinmux_config(PAD2,PF_UART1);
            duet_pinmux_config(PAD3,PF_UART1);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD14,PF_UART1);
                duet_pinmux_config(PAD15,PF_UART1);
            }
            break;
        case UART2_INDEX:
            duet_pinmux_config(PAD12,PF_UART2);
            duet_pinmux_config(PAD13,PF_UART2);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD10,PF_UART2);
                duet_pinmux_config(PAD11,PF_UART2);
            }
            break;
        default:
            break;
    }
}

void duet_gpio0_irq_handler(void *arg) //for gpio0
{
    printf("duet gpio0 callback function\r\n\r");
}

duet_gpio_dev_t g_duet_gpio0, g_duet_gpio1;

//note: before test, please short GPIO0 and GPIO1. GPIO0 functions as input, while GPIO1 output

void gpio_api_test(void)
{
    uint32_t input_value = 0;
//    uint32_t i;

    //gpio0 input
    g_duet_gpio0.port = GPIO0_INDEX;
    g_duet_gpio0.config = DUET_INPUT_PULL_DOWN;
    g_duet_gpio0.priv = NULL;
    duet_gpio_init(&g_duet_gpio0);
    duet_gpio_enable_irq(&g_duet_gpio0, DUET_IRQ_TRIGGER_FALLING_EDGE, duet_gpio0_irq_handler, NULL);

    //gpio1 output
    g_duet_gpio1.port = GPIO1_INDEX;
    g_duet_gpio1.config = DUET_OUTPUT_PUSH_PULL;
    g_duet_gpio1.priv = NULL;
    duet_gpio_init(&g_duet_gpio1);

    printf("gpio test begin\r\n");

    while(1)
    {
        //delay
        for(int i = 0; i < 100000; i++);
        printf("gpio1 output toggle\r\n\r");
        duet_gpio_output_toggle(&g_duet_gpio1);

        duet_gpio_input_get(&g_duet_gpio0, &input_value);
        printf("gpio0 input value %u\r\n\r", (unsigned int)(input_value));
    }
}


int main(void)
{
//init idx and flowControl
    uint8_t uart_idx = UART_TEST_INDEX;
    uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
//set pin mux
    set_uart_pinmux(uart_idx,hw_flow_control);

    set_uart_config(uart_idx,hw_flow_control);

//    duet_uart_start(getUartxViaIdx(uart_idx));

    printf_uart_register(uart_idx);
    printf("gpio test runing!\r\n");
    gpio_api_test();

   while(1);

}
