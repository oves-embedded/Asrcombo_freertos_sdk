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
#include "duet_timer.h"
#include "duet_i2c.h"
#include "printf_uart.h"
#include "duet_dma.h"

#define I2C_TEST_INDEX                 I2C_DEVICE0

//I2C TEST MODE
#define I2C_MASTER_FIFO_WRITE               0
#define I2C_MASTER_FIFO_READ                1
#define I2C_MASTER_FIFO_MULTI_WRITE_READ    2
#define I2C_SLAVE_MODE                      3
#define I2C_MASTER_DMA_WRITE                4
#define I2C_MASTER_DMA_READ                 5
#define I2C_MST_MEM_WR_TEST                 6
#define I2C_MST_MEM_RD_TEST                 7
#define TEST_MODE_BIT(n)                    (1 << n)

#define I2C_TEST_MODE (TEST_MODE_BIT(I2C_MASTER_FIFO_WRITE) | TEST_MODE_BIT(I2C_MASTER_FIFO_READ) | TEST_MODE_BIT(I2C_MASTER_FIFO_MULTI_WRITE_READ))

#define TEST_CYCLE          1000
#define I2C_ADDR_MODE I2C_ADDR_WIDTH_7BIT
#define I2C_SCL_FREQ I2C_STANDARD_SPEED
#define I2C_TAR_ADDR  0x57
#define MEM_ADDR 0x30
#define I2C_TEST_DATA_SIZE 1
#define I2C_MEM_ADDR_SIZE 1
#define I2C_TIMEOUT 0xFFFFFFFF
#define I2C_DMA_TEST_SIZE 6

#define UART_TEST_INDEX UART0_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0

extern duet_uart_callback_func g_duet_uart_callback_handler[UART_NUM];
static uint8_t tx_data[16] = {0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
static uint8_t data_rec[16];
uint32_t dma_i2ctx_data[32]={0};
uint32_t dma_i2crx_data[32]={0xff, 0xff, 0xff, 0xff, 0xff, 0};
duet_i2c_dev_t g_duet_i2c;

void HardFault_Handler(void)
{
    while(1);
}

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
}

void set_uart_pinmux(uint8_t uart_idx,uint8_t hw_flow_control)
{
    switch(uart_idx)
    {
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

void duet_i2c_dma_init(void)
{
    if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_DMA_WRITE))
    {
        dma_i2ctx_data[0] = I2C_TB | I2C_SEND_START | (I2C_TAR_ADDR << 1);
        dma_i2ctx_data[1] = I2C_TB | tx_data[0];
        dma_i2ctx_data[2] = I2C_TB | tx_data[1];
        dma_i2ctx_data[3] = I2C_TB | tx_data[2];
        dma_i2ctx_data[4] = I2C_TB | tx_data[3];
        dma_i2ctx_data[5] = I2C_TB | I2C_SEND_STOP | tx_data[4];
    }
    else if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_DMA_READ))
    {
        dma_i2ctx_data[0] = I2C_TB | I2C_SEND_START | (I2C_TAR_ADDR << 1) | I2C_READ;
        dma_i2ctx_data[1] = I2C_TB;
        dma_i2ctx_data[2] = I2C_TB;
        dma_i2ctx_data[3] = I2C_TB;
        dma_i2ctx_data[4] = I2C_TB;
        dma_i2ctx_data[5] = I2C_TB | I2C_SEND_STOP | I2C_SEND_NACK;
    }
}

void uart_init(void )
{
    //init idx and flowControl
    uint8_t uart_idx = UART_TEST_INDEX;
    uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
    //set pin mux
    set_uart_pinmux(uart_idx,hw_flow_control);
    set_uart_config(uart_idx,hw_flow_control);

    printf_uart_register(uart_idx);
}

int main(void)
{
    int ret = 0;
    int test_cnt = 0;
    uart_init();

    g_duet_i2c.port = I2C_TEST_INDEX;
    g_duet_i2c.config.mode = I2C_MASTER;
    g_duet_i2c.config.freq = I2C_SCL_FREQ;
    g_duet_i2c.config.dev_addr = I2C_ADDRESS;

    duet_i2c_init(&g_duet_i2c);

    printf("I2C demo runing!\r\n");
    while(test_cnt++ < TEST_CYCLE)
    {
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_FIFO_WRITE))
        {
            ret = duet_i2c_master_send(&g_duet_i2c, I2C_TAR_ADDR, tx_data, I2C_TEST_DATA_SIZE);
            if(ret)
            {
                printf("%d: ret: %d\n", __LINE__, ret);
            }
        }
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_FIFO_READ))
        {
            ret = duet_i2c_master_recv(&g_duet_i2c, I2C_TAR_ADDR, data_rec, I2C_TEST_DATA_SIZE);
            if(ret)
            {
                printf("%d: ret: %d\n", __LINE__, ret);
            }

        }
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_FIFO_MULTI_WRITE_READ))
        {
            ret = duet_i2c_master_repeated_write_read(&g_duet_i2c, I2C_TAR_ADDR, tx_data, data_rec, I2C_TEST_DATA_SIZE, I2C_TEST_DATA_SIZE);
            if(ret)
            {
                printf("%d: ret: %d\n", __LINE__, ret);
            }
        }
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_DMA_WRITE))
        {
            duet_dma_init();
            duet_i2c_dma_init();
            duet_i2c_master_dma_send(g_duet_i2c.port, dma_i2ctx_data, I2C_DMA_TEST_SIZE);
        }
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MASTER_DMA_READ))
        {
            duet_dma_init();
            duet_i2c_dma_init();
            duet_i2c_master_dma_send(g_duet_i2c.port, dma_i2ctx_data, I2C_DMA_TEST_SIZE);
            duet_i2c_master_dma_recv(g_duet_i2c.port, dma_i2crx_data, I2C_DMA_TEST_SIZE - 1);
        }
        if (I2C_TEST_MODE & TEST_MODE_BIT(I2C_MST_MEM_WR_TEST))
        {
            duet_i2c_mem_write(&g_duet_i2c, I2C_TAR_ADDR, MEM_ADDR, tx_data, I2C_TEST_DATA_SIZE);
        }
        if(I2C_TEST_MODE & TEST_MODE_BIT(I2C_MST_MEM_RD_TEST))
        {
            duet_i2c_mem_read(&g_duet_i2c, I2C_TAR_ADDR, MEM_ADDR, data_rec, I2C_TEST_DATA_SIZE);
        }
    }
    printf("I2C demo end!\r\n");
    while(1);
}
