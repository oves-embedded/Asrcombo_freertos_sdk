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
#include "printf_uart.h"
#include "duet_efuse.h"
#include "duet_common.h"

#define MAX_CMD_LEN         15
#define MAX_PARAM_LEN       15

#define MIN_ASCII           33
#define MAX_ASCII           126
#define MAX_UART_BUF                (256)

volatile char uart_buf[MAX_UART_BUF];
volatile uint8_t uart_wr_idx;
volatile uint8_t uart_rd_idx;

void uart_recv_test(char data)
{
    uart_buf[uart_wr_idx] = data;
    uart_wr_idx = (uart_wr_idx + 1) % MAX_UART_BUF;
}

void roll_index(volatile unsigned char *idx, int size)
{
    if(*idx==size-1)     *idx=0;
    else     *idx +=1;

}

// clear one byte buf and move index
void clear_one_uart_buf(void)
{
    uart_buf[uart_rd_idx]=0;  //reset uart buf
    roll_index(&uart_rd_idx, MAX_UART_BUF);
}

// receive a string from uart
void get_string_from_uart(char *str)
{
    char *p=str;
    char flag=0;

    while(!flag)
    {
        if(uart_wr_idx!=uart_rd_idx){
            // 33 is first asci charactor, 126 is last asci charactor
            if(MIN_ASCII<=uart_buf[uart_rd_idx] && uart_buf[uart_rd_idx]<=MAX_ASCII){
                *p++ = uart_buf[uart_rd_idx];  //valid character
                if((p-str)>=MAX_CMD_LEN)   flag=1;
            }
            else if( p!=str){ // it received one charactor at least
                flag=1;
            }
            clear_one_uart_buf();
        } // end of if
    } // end if while
}

// parse a int type data from a string which is received from uart buf
int parse_int_parameter(void)
{
    char str[MAX_PARAM_LEN];

    duet_memset(str, 0, MAX_PARAM_LEN);
    get_string_from_uart(str);
    printf("%s",str);
    return(convert_str_to_int(str));
}

void rdefuse_cmd_func(void)
{
    unsigned int mode;
    unsigned int addr;
    unsigned int value;
    char str[11] = "\0";
    mode = parse_int_parameter(); // 0 - byte read; 1 - word read
    addr = parse_int_parameter();
    duet_efuse_init(EFUSE_LDO25_OPEN);
    if(mode)
    {
        value = duet_efuse_word_read(addr);
        convert_int_to_str(value, sizeof(int), str);
    }
    else
    {
        value = duet_efuse_byte_read(addr);
        convert_int_to_str(value, sizeof(char), str);
    }
    printf("\r\n");
    printf("%s",str);
    //print32(value);
}

//write a register
void wrreg_cmd_func(void)
{
    unsigned int addr, value;

    addr = parse_int_parameter();
    value = parse_int_parameter();

    REG_WR(addr, value);
    printf("\r\n write is done");
}

// read a register
void rdreg_cmd_func(void)
{
    unsigned int addr, value;

    addr = parse_int_parameter();
    value = REG_RD(addr);
    printf("0x%x",value);
}

void help_cmd_func(void)
{
    printf("\r\n wrefuse mode addr value");
    printf("\r\n rdefuse mode addr");
    printf("\r\n wrreg addr value");
    printf("\r\n rdreg addr");
}

typedef struct _cmd_entry_{
    char *command;
    void (*function)(void);
} cmd_entry;

// command table
static const cmd_entry uart_cmd_table[] = {
    {"rdefuse", (void *)rdefuse_cmd_func},
    {"wrreg",(void *)wrreg_cmd_func},
    {"rdreg",(void *)rdreg_cmd_func},
    {"help", (void *)help_cmd_func}
};

// handle command input by uart
void handle_uart_cmd(void)
{
    char cmd[MAX_CMD_LEN];
    unsigned int i;

    while(1)
    {
        duet_memset(cmd,0, MAX_CMD_LEN);
        uart_rd_idx = uart_wr_idx;
        printf("\r\n>>>");
        get_string_from_uart(cmd); // read a string from uart buffer
        printf("%s ",cmd);
        for(i = 0; i < sizeof(uart_cmd_table) / sizeof(uart_cmd_table[0]); i++){
            if(strcmp(cmd, uart_cmd_table[i].command) == 0){  // find if there is a valid cmd input, if yes, run the accordingly function
                uart_cmd_table[i].function();
                break;
            }
        }
     } // end of while()
} // end of func

void HardFault_Handler(void)
{
    while(1);
}

extern duet_uart_callback_func g_duet_uart_callback_handler[UART_NUM];

#define UART_TEST_INDEX UART1_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0



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

int main(void)
{
    uint32_t tmp_value;
#if 1
    REG_WR(0xE000EF50,0X0);
    tmp_value = REG_RD(0XE000ED14);
    REG_WR(0XE000ED14, (tmp_value | (0x00030000)));
    __asm("DSB");
    __asm("ISB");
#endif
//init idx and flowControl
    uint8_t uart_idx = UART_TEST_INDEX;
    uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
//set pin mux
    set_uart_pinmux(uart_idx,hw_flow_control);

    set_uart_config(uart_idx,hw_flow_control);

//    duet_uart_start(getUartxViaIdx(uart_idx));

    printf_uart_register(uart_idx);
    printf("efuse demo runing!\r\n");

    handle_uart_cmd();

}
