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
#include "duet_uart.h"
#include "duet_cm4.h"
#include "duet_ram_layout.h"
#include "printf_uart.h"

#define ITCM_DETA   (32*1024)

void HardFault_Handler(void)
{
    printf("hard fault\r\n");
    while(1);
}

int sram_layout_test(uint32_t addr, uint32_t size)
{
    for(uint32_t i = 0;i < size;i++)\
        *(volatile uint8_t *)(addr+i) = (i%0xff);

    for(uint32_t i = 0;i < size;i++)
    {
        if(*(volatile uint8_t *)(addr+i) != (i%0xff))
        {
            printf("0x%x test fail \r\n",(unsigned int)(addr+i));
            return -1;
        }
    }

    for(uint32_t i = 0;i < size;i++)
        *(volatile uint8_t *)(addr+i) = (0xff);

    for(uint32_t i = 0;i < size;i++)
    {
        if(*(volatile uint8_t *)(addr+i) != (0xff))
        {
            printf("0x%x test 0xff fail \r\n",(unsigned int)(addr+i));
            return -1;
        }
    }

    for(uint32_t i = 0;i < size;i++)
    {
        if(i%2)
        {
            *(volatile uint8_t *)(addr+i) = 0xa5;
        }
        else
        {
            *(volatile uint8_t *)(addr+i) = 0x5a;
        }
    }

    for(uint32_t i = 0;i < size;i++)
    {
        if(i%2)
        {
            if(*(volatile uint8_t *)(addr+i) != 0xa5)
            {
                printf("0x%x test a5_5a fail\r\n",(unsigned int)(addr+i));
                return -1;
            }
        }
        else
        {
            if(*(volatile uint8_t *)(addr+i) != 0x5a)
            {
                printf("0x%x test a5_5a fail\r\n",(unsigned int)(addr+i));
                return -1;
            }
        }
    }

    for(uint32_t i = 0;i < size;i++)
    {
        if(i%2)
        {
            *(volatile uint8_t *)(addr+i) = 0x5a;
        }
        else
        {
            *(volatile uint8_t *)(addr+i) = 0xa5;
        }
    }

    for(uint32_t i = 0;i < size;i++)
    {
        if(i%2)
        {
            if(*(volatile uint8_t *)(addr+i) != 0x5a)
            {
                printf("0x%x test 5a_a5 fail\r\n",(unsigned int)(addr+i));
                return -1;
            }
        }
        else
        {
            if(*(volatile uint8_t *)(addr+i) != 0xa5)
            {
                printf("0x%x test 5a_a5 fail\r\n",(unsigned int)(addr+i));
                return -1;
            }
        }
    }
    return 0;
}

int main(void)
{
    int ret = 0;
    uint32_t tmp_value = REG_RD(PERI_CLK_EN_REG0);
    REG_WR(PERI_CLK_EN_REG0, (tmp_value | (0x00FF8000))); //open clock of wifi and ble

    SCB_EnableICache();

    printf_uart_log_init(UART1_INDEX);

    printf("\r\nsram verify except the first 32K itcm\r\n\r\n");

    for(Tcm_Config_Type tcm_config = ITCM_DTCM_32_192; tcm_config < ITCM_DTCM_NUM; tcm_config++)
    {
        for(Wifi_Ram_Config_Type wifi_config = WIFI_RAM_0; wifi_config < WIFI_RAM_NUM; wifi_config++)
        {
            for(Bt_Ram_Config_Type bt_config = BT_RAM_0; bt_config < BT_RAM_NUM; bt_config++)
            {
                if(duet_ram_layout_init(tcm_config, wifi_config, bt_config) == 0)
                {
                    Ram_Layout_Type ram_layout;
                    ret += duet_get_ram_layout(&ram_layout);
                    printf("itcm addr:0x%x, size:%dK\r\n", (unsigned int)ram_layout.itcm_addr, (unsigned int)ram_layout.itcm_size/1024);
                    ret += sram_layout_test(ram_layout.itcm_addr + ITCM_DETA, ram_layout.itcm_size - ITCM_DETA);

                    printf("dtcm addr:0x%x, size:%dK\r\n", (unsigned int)ram_layout.dtcm_addr, (unsigned int)ram_layout.dtcm_size/1024);
                    ret += sram_layout_test(ram_layout.dtcm_addr, ram_layout.dtcm_size);

                    printf(" soc addr:0x%x, size:%dK\r\n", (unsigned int)ram_layout.soc_addr,  (unsigned int)ram_layout.soc_size/1024);
                    ret += sram_layout_test(ram_layout.soc_addr,  ram_layout.soc_size);

                    printf("wifi addr:0x%x, size:%dK\r\n", (unsigned int)ram_layout.wifi_addr, (unsigned int)ram_layout.wifi_size/1024);
                    ret += sram_layout_test(ram_layout.wifi_addr, ram_layout.wifi_size);

                    printf("  bt addr:0x%x, size:%dK\r\n", (unsigned int)ram_layout.bt_addr,   (unsigned int)ram_layout.bt_size/1024);
                    ret += sram_layout_test(ram_layout.bt_addr,   ram_layout.bt_size);

                    printf("\r\n");
                }
            }
        }
    }

    if(ret == 0)
        printf("sram verify OK!!\r\n");
    else
        printf("sram verify FAILED!!\r\n");

    while(1);
}
