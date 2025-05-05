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

#include "duet_pinmux.h"

#define PINMUX_PAD_NUM      24
#define PINMUX_FUNC_NUM     8
const char g_pinmux_array[PINMUX_PAD_NUM*PINMUX_FUNC_NUM] = 
{
    PF_GPIO0,   PF_UART0,   PF_SWD,     PF_SPI1,    PF_PWM5,    PF_DEBUG,   PF_NA,      PF_NA,
    PF_GPIO1,   PF_UART0,   PF_SWD,     PF_SPI1,    PF_PWM7,    PF_DEBUG,   PF_NA,      PF_NA,
    PF_GPIO2,   PF_UART1,   PF_UART1,   PF_SPI1,    PF_I2C0,    PF_DEBUG,   PF_NA,      PF_NA,
    PF_GPIO3,   PF_UART1,   PF_UART1,   PF_SPI1,    PF_I2C0,    PF_DEBUG,   PF_NA,      PF_NA,
    PF_SWD,     PF_GPIO4,   PF_SDIO0,   PF_UART0,   PF_PWM0,    PF_DEBUG,   PF_NA,      PF_PSRAM,
    PF_SWD,     PF_GPIO5,   PF_SDIO0,   PF_UART0,   PF_PWM2,    PF_DEBUG,   PF_NA,      PF_PSRAM,
    PF_GPIO6,   PF_SPI0,    PF_SDIO0,   PF_UART0,   PF_PWM4,    PF_DEBUG,   PF_NA,      PF_PSRAM,
    PF_GPIO7,   PF_SPI0,    PF_SDIO0,   PF_UART0,   PF_PWM6,    PF_DEBUG,   PF_I2S,     PF_PSRAM,
    PF_GPIO8,   PF_SPI0,    PF_SDIO0,   PF_I2C1,    PF_UART1,   PF_DEBUG,   PF_I2S,     PF_PSRAM,
    PF_GPIO9,   PF_SPI0,    PF_SDIO0,   PF_I2C1,    PF_UART1,   PF_DEBUG,   PF_I2S,     PF_PSRAM,
    PF_SEL3,    PF_PWM1,    PF_GPIO10,  PF_UART2,   PF_SPI2,    PF_DEBUG,   PF_I2S,     PF_NA,
    PF_GPIO11,  PF_PWM3,    PF_SDIO0,   PF_UART2,   PF_SPI2,    PF_DEBUG,   PF_I2S,     PF_NA,
    PF_GPIO12,  PF_GPIO12,  PF_SPI2,    PF_UART2,   PF_GPIO12,  PF_DEBUG,   PF_I2S,     PF_NA,
    PF_GPIO13,  PF_GPIO13,  PF_SPI2,    PF_UART2,   PF_GPIO13,  PF_DEBUG,   PF_NA,      PF_NA,
    PF_SEL1,    PF_PWM0,    PF_SPI2,    PF_UART1,   PF_GPIO14,  PF_DEBUG,   PF_NA,      PF_NA,
    PF_SEL2,    PF_PWM2,    PF_SPI2,    PF_UART1,   PF_GPIO15,  PF_DEBUG,   PF_NA,      PF_NA,
    PF_GPIO16,  PF_UART0,   PF_SDIO0,   PF_SPI1,    SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO17,  PF_UART0,   PF_SDIO0,   PF_SPI1,    SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO18,  PF_UART1,   PF_SDIO0,   PF_SPI1,    SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO19,  PF_UART1,   PF_SDIO0,   PF_SPI1,    SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO20,  PF_I2C0,    PF_SDIO0,   PF_UART0,   SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO21,  PF_I2C0,    PF_SDIO0,   PF_UART0,   SEC_FLS,    PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO22,  PF_I2C1,    PF_SDIO0,   PF_UART2,   PF_GPIO22,  PF_DEBUG,   PF_PSRAM,   PF_NA,
    PF_GPIO23,  PF_I2C1,    PF_GPIO23,  PF_UART2,   PF_GPIO23,  PF_DEBUG,   PF_NA,      PF_NA
};

Pad_Config_State duet_pinmux_config(Pad_Num_Type pad_num, Pad_Func_Type pad_func)
{
    if(pad_num < PINMUX_PAD_NUM && pad_func != PF_NA)
    {
        const char* p_pinmux_line = g_pinmux_array + pad_num*PINMUX_FUNC_NUM;
        for(int func_index = 0; func_index < PINMUX_FUNC_NUM; func_index++)
        {
            if(p_pinmux_line[func_index] == pad_func)
            {
                uint32_t pad_reg_addr = PINMUX_REG_BASE + 4*(pad_num>>3); // pinmux register address of pad pad_num
                uint32_t reg_offset = (pad_num%8)*4;  // offset from pad_reg_addr in bits
                *(volatile uint32_t *)(pad_reg_addr) &= ~(0xf<<reg_offset); // mask
                *(volatile uint32_t *)(pad_reg_addr) |= (func_index<<reg_offset); // set pinmux value
                return Config_Success;
            }
        }
    }
    return Config_Fail;
}

void duet_pad_config(Pad_Num_Type pad_num, Pad_Pull_Type pull_type)
{
    /* assert params */
    // to-do
     switch(pull_type)
     {
         case PULL_UP:
             *(volatile uint32_t *)(HW_CTRL_PE_PS) &= ~(0x1 << (pad_num));
             *(volatile uint32_t *)(PAD_PE_REG) |= (0x1 << (pad_num));
             *(volatile uint32_t *)(PAD_PS_REG) |= (0x1 << (pad_num));
             break;
         case PULL_DOWN:
             *(volatile uint32_t *)(HW_CTRL_PE_PS) &= ~(0x1 << (pad_num));
             *(volatile uint32_t *)(PAD_PE_REG) |= (0x1 << (pad_num));
             *(volatile uint32_t *)(PAD_PS_REG) &= ~(0x1 << (pad_num));
             break;
         case PULL_NONE:
             *(volatile uint32_t *)(HW_CTRL_PE_PS) &= ~(0x1 << (pad_num));
             *(volatile uint32_t *)(PAD_PE_REG) &= ~(0x1 << (pad_num));
             break;
         default:
             break;
     }
}

