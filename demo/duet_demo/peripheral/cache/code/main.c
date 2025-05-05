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
#include "duet_cm4.h"
#include "system_cm4.h"
#include "duet_uart.h"
#include "printf_uart.h"
#include "duet_timer.h"

volatile int flag = 1;
uint32_t test_index = 0;
duet_timer_dev_t duet_timer = {0};

void HardFault_Handler(void)
{
    while(1);
}

void timer_callback(void *data)
{
    flag = 0;
    duet_timer_stop(&duet_timer);
}

void config_timer(void)
{
    duet_timer.port = DUET_TIMER1_INDEX;
    duet_timer.config.period = 1000000;
    duet_timer.config.reload_mode = TIMER_RELOAD_MANU;
    duet_timer.config.cb = timer_callback;
    duet_timer_init(&duet_timer);
    duet_timer_start(&duet_timer);
}

void read_from_flash()
{
    flag = 1;
    test_index = 0;
    config_timer();
    while(flag)
    {
        for(uint32_t i = 0; i < 1024; i++) {    //read 1KB from flash
            (*(volatile uint32_t *)(APP_FLASH_START_ADDR + i));
        }
        test_index++;
    }
}

int main(void)
{
    printf_uart_log_init(UART1_INDEX);
    printf("cache test begin!!!\n\n");

    SCB_DisableICache();
    SCB_DisableDCache();
    read_from_flash();
    printf("cache disable, test_index = %d after 1s\n\n", (unsigned int)test_index);

    SCB_EnableICache();
    SCB_EnableDCache();
    read_from_flash();
    printf("cache enable, test_index = %d after 1s\n\n", (unsigned int)test_index);

    while(1);
}
