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

#include "duet_uart.h"
#include "duet_psram.h"
#include "duet_pinmux.h"
#include "printf_uart.h"

void psram_read_write_test(void)
{
    uint32_t addr;

    for(addr = PSRAM_AMBA_BASE; addr < PSRAM_FLASH_A1_TOP; addr+=4) {
        REG_WR(addr,0xA5A55A5A);
    }
    printf("mapped write %X~%X writen \n",PSRAM_AMBA_BASE, (unsigned int)addr);

    for(addr = PSRAM_AMBA_BASE; addr < PSRAM_FLASH_A1_TOP; addr+=4) {
        uint32_t read_data = REG_RD(addr);
        if( read_data != 0xA5A55A5A) {
            printf("mapped read %X fail, expect %X, actual %X !!!\n", (unsigned int)addr, 0xA5A55A5A, (unsigned int)read_data);
            return;
        }
    }
    printf("mapped read %X~%X success!!! \n",PSRAM_AMBA_BASE, (unsigned int)addr);
}

int main(void)
{
    printf_uart_log_init(UART1_INDEX);

    printf("\nCombo S2C psram verify\r\n");

    psram_set_channel(PSRAM_CHANNEL_4_9);

    duet_pinmux_config(PAD0,PF_SWD);//set pad0 mux to swc if need, not necessary
    duet_pinmux_config(PAD1,PF_SWD);//set pad1 mux to swd if need, not necessary

    printf("\n\nsingle test start \n");
    psram_config(PSRAM_MODE_SPI);
    psram_read_write_test();

    printf("\n\nquad test start \n");
    psram_config(PSRAM_MODE_QSPI);
    psram_read_write_test();

    printf("\npsram verify end!!! \r\n");

    while(1);
}
