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
#include "duet_common.h"
#include "duet_uart.h"
#include "printf_uart.h"
#include "duet_flash_encrypt.h"

#define CASE_IMAGE              0   //pls program into flash, before and after encryption
#define CASE_ENCRYPT            1   //pls run in ram.
#define CASE_DISABLE_ENCRYPT    2   //pls run in ram.

#define TEST_CASE CASE_IMAGE

void HardFault_Handler(void)
{
    while(1);
}

int main(void)
{
    int ret;

    (void)ret;	//avoid warning
    printf_uart_log_init(UART1_INDEX);
    printf("Flash encrypt enable:%d, rest operation time:%d\r\n", duet_flash_encrypt_status(), duet_flash_encrypt_operation_rest());
#if (TEST_CASE == CASE_IMAGE)
    printf("This is flash image running!\r\n");
#elif (TEST_CASE == CASE_ENCRYPT)
    printf("Now, enable flash encrypt!\r\n");
    ret = duet_flash_encrypt();
    printf("Flash enable encrypt end with ret:%d, and the rest operation time:%d\r\n", ret, duet_flash_encrypt_operation_rest());
#elif (TEST_CASE == CASE_DISABLE_ENCRYPT)
    printf("Now, disable flash encrypt!\r\n");
    ret = duet_flash_disable_encrypt();
    printf("Flash disable encrypt end with ret:%d, and the rest operation time:%d\r\n", ret, duet_flash_encrypt_operation_rest());
#endif

    while(1);
}
