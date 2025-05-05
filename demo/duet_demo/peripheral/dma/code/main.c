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
#include <string.h>
#include "duet_pinmux.h"
#include "duet_uart.h"
#include "duet_timer.h"
#include "duet_spi.h"
#include "duet_dma.h"
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

duet_uart_dev_t uart_config_struct  = {0};
void set_uart_config(uint8_t uart_idx,uint8_t hw_flow_control)
{
    duet_uart_struct_init(&uart_config_struct);
    uart_config_struct.port = uart_idx;
    uart_config_struct.priv = NULL;
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

#define SPI_CS_PIN   PAD12
#define SPI_CLK_PIN  PAD13
#define SPI_MOSI_PIN PAD14
#define SPI_MISO_PIN PAD15

#define SPI0_INDEX 0
#define SPI1_INDEX 1
#define SPI2_INDEX 2

#define SPI_TEST_INDEX SPI2_INDEX
#define PF_SPI  PF_SPI2

uint8_t tx_data[11] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa};
uint8_t rx_data[11] = {0};
duet_spi_dev_t duet_spi = {0};
void spi_callback(uint8_t data)
{
    static uint8_t index = 0;
    rx_data[index++] = data;
    if(index > 10)
    {
        printf("spi recv data:");
        for(uint8_t i = 0;i<11;i++)
        {
            printf(" 0x%2x ",rx_data[i]);
        }
        printf("\r\n");
        index = 0;
        memset(rx_data,0,11);
    }
}


void duet_spi_test(void)
{
    duet_spi.port = SPI_TEST_INDEX;
    duet_spi.config.freq = 2000000;
    duet_spi.config.mode = 1;
    duet_spi.priv = NULL;
    duet_pinmux_config(SPI_CS_PIN,PF_SPI);
    duet_pinmux_config(SPI_CLK_PIN,PF_SPI);
    duet_pinmux_config(SPI_MOSI_PIN,PF_SPI);
    duet_pinmux_config(SPI_MISO_PIN,PF_SPI);
    duet_spi_init(&duet_spi);
}

duet_timer_dev_t duet_timer = {0};
uint8_t spi_send_flag = 0;
void timer_callback(void *data)
{
   spi_send_flag = 1;
}


void duet_timer_test(void)
{
    duet_timer.port = DUET_TIMER2_INDEX;
    duet_timer.config.period = 5000000;
    duet_timer.config.reload_mode = TIMER_RELOAD_AUTO;
//    duet_timer.config.reload_mode = TIMER_RELOAD_MANU;
    duet_timer.config.cb = timer_callback;
    duet_timer_init(&duet_timer);
    duet_timer_start(&duet_timer);

}

uint8_t dma_src_data[512]={0};
uint8_t dma_dst_data[512]={0};
uint8_t dma_tx_data[512]={0};
uint8_t dma_rx_data[64]={0};
uint8_t dma_spitx_data[32]={0};
uint8_t dma_spirx_data[32]={0};

void dma_mem_callback(uint32_t i)
{
    printf("dma int:%d\r\n",(unsigned int)i);
    for(uint16_t temp = 0; temp < 512; temp++)
    {
        if(dma_src_data[temp] != dma_dst_data[temp])
        {
            printf("mem2mem copy by dma fails\n");
            return;
        }
    }
    printf("mem2mem copy by dma pass\n");
}

void dma_callback(uint32_t i)
{
    printf("dma int:%d\r\n",(unsigned int)i);
}

void dma_rx_callback(uint32_t i)
{
    printf("dma int:%d,rx:%s,data_len:%d\r\n",(unsigned int)i,dma_rx_data,strlen((char*)dma_rx_data));
}

void dma_spirx_callback(uint32_t i)
{
    printf("dma int:%d,rx:\r\n",(unsigned int)i);
    for(uint16_t i = 0;i<32;i++)
    {
        printf(" 0x%x \r\n",dma_spirx_data[i]);
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
    duet_spi_test();
    duet_timer_test();
    duet_dma_init();
   //duet_uart_stop(getUartxViaIdx(uart_idx));
    printf("dma demo runing!\r\n");
//DMA mem
#if 1
    for(uint16_t i = 0;i<512;i++)
    {
        dma_src_data[i] = i;
    }
    duet_dma_callback_register(0,dma_mem_callback);
    duet_dma_mem2mem(0,dma_src_data,dma_dst_data,512);
#endif
//DMA UART
#if 1
    for(uint16_t i = 0;i<512;i++)
    {
        dma_tx_data[i] = i;
    }
//    //UART TX
    duet_uart_dma_config(&uart_config_struct,UART_DMA_TX_EN,ENABLE);
    duet_dma_callback_register(DMA_CH_UART1_TX,dma_callback);
    duet_dma_uart_tx(1,dma_tx_data,512);
    //UART RX
    duet_uart_dma_config(&uart_config_struct,UART_DMA_RX_EN,ENABLE);
    duet_dma_callback_register(DMA_CH_UART1_RX,dma_rx_callback);
    duet_dma_uart_rx(uart_config_struct.port,dma_rx_data,32);
#endif
//DMA SPI
#if 1
    //SPI data
    for(uint8_t i = 0;i<32;i++)
    {
        dma_spitx_data[i] = i;
    }
    //SPI RX
    duet_spi_dma_config(&duet_spi,SPI_DMA_RX_EN,ENABLE);
    duet_dma_callback_register(DMA_CH_SPI2_RX,dma_spirx_callback);
    duet_dma_spi_rx(duet_spi.port,dma_spirx_data,32);
//    duet_spi_send(&duet_spi,dma_spitx_data,32,0);
    //SPI TX
    duet_spi_dma_config(&duet_spi,SPI_DMA_TX_EN,ENABLE);
    duet_dma_callback_register(DMA_CH_SPI2_TX,dma_callback);
    duet_dma_spi_tx(duet_spi.port,dma_spitx_data,32);
#endif

   while(1);

}
