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

#include <string.h>
#include "system_cm4.h"
#include "duet_pinmux.h"
#include "duet_uart.h"
#include "printf_uart.h"
#include "duet_i2s.h"

#define I2S_MCLK_PIN  PAD7
#define I2S_SCLK_PIN  PAD8
#define I2S_LRCLK_PIN PAD9
#define I2S_DO_PIN    PAD10
#define I2S_DI_PIN    PAD11

void HardFault_Handler(void)
{
    while(1);
}

#define DATA_NUM    10

uint32_t left_rcv_data[DATA_NUM] = {0};
uint32_t right_rcv_data[DATA_NUM] = {0};
uint32_t g_index = 0;
uint32_t left_chan_data[DATA_NUM] = {0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0x77777777,0x88888888,0x99999999,0xaaaaaaaa};
uint32_t right_chan_data[DATA_NUM] = {0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666,0x77777777,0x88888888,0x99999999,0xaaaaaaaa};
void i2s_recv_test(uint32_t l_data,uint32_t r_data)
{
    left_rcv_data[g_index] = l_data;
    right_rcv_data[g_index] = r_data;
    if (((left_rcv_data[g_index] == 0) && (right_rcv_data[g_index] == 0)) || ((left_rcv_data[g_index] == 0xFFFF) && (right_rcv_data[g_index] == 0xFFFF)))
    {
        left_rcv_data[g_index] = 0;
        right_rcv_data[g_index] = 0;
    }
    else
    {
        if (g_index < DATA_NUM - 1)
        {
            g_index++;
        }
        else
        {
            printf("recv left data:");
            for(uint8_t i = 0;i < DATA_NUM;i++)
            {
                printf(" 0x%x ",(unsigned int)left_rcv_data[i]);
            }
            printf("\r\n");
            printf("recv right data:");
            for(uint8_t i = 0;i < DATA_NUM;i++)
            {
                printf(" 0x%x ",(unsigned int)right_rcv_data[i]);
            }
            printf("\r\n");
            g_index = 0;
        }
    }
}

void i2s_config(void)
{
    int ret = 0;
    /* i2s config structrue init */
    duet_i2s_dev_t i2s_init_struct;
    duet_i2s_struct_init(&i2s_init_struct);

    /* initialize i2s */
    ret = duet_i2s_init(I2S, &i2s_init_struct);
    if(ret == 0)
    {
        duet_i2s_interrupt_config(I2S, I2S_INTERRUPT_RXDA, ENABLE);
        g_duet_i2s_callback_handler = i2s_recv_test;
        duet_i2s_cmd(I2S, ENABLE);
        REG_WR(0X40000944, 0x1<<24); //enable the i2s irq
        NVIC_EnableIRQ(I2S_IRQn);
    }
    else
        printf("i2s init failed, the clock source may not fit for the sample rate!\n\n");
}

int main(void)
{
    duet_pinmux_config(I2S_MCLK_PIN,PF_I2S);
    duet_pinmux_config(I2S_SCLK_PIN,PF_I2S);
    duet_pinmux_config(I2S_LRCLK_PIN,PF_I2S);
    duet_pinmux_config(I2S_DI_PIN,PF_I2S);
    duet_pinmux_config(I2S_DO_PIN,PF_I2S);

    printf_uart_log_init(UART1_INDEX);

    printf("i2s test begin!!!\n\n");

    i2s_config();

    while(1)
    {
        duet_i2s_send_data(I2S,left_chan_data,right_chan_data,DATA_NUM);
        delay(10000);
    }
}
