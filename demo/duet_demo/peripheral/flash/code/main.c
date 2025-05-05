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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "duet_uart.h"
#include "duet_flash.h"
#include "duet_cm4.h"
#include "duet_flash_alg.h"
#include "printf_uart.h"

char test_string[] = "this is a flash test program!";

void flash_api_test(void)
{
    int32_t ret;
    int offset = 0x00;
    int param_part = PARTITION_PARAMETER_2;

    char rd_buf[32];

    printf("flash test begin\r\n");

    ret = duet_flash_init();    //all interrupt must be disabled when flash initial

    //this is the flash normal operation
    ret = duet_flash_erase(param_part, offset, strlen(test_string));
    if(ret != 0)
    {
        printf("flash erase failed\r\n");
        return;
    }

    printf("flash write start offset is %d\r\n", offset);
    ret = duet_flash_write(param_part, (uint32_t*)&offset, test_string, strlen(test_string));
    if(ret != 0)
    {
        printf("flash write failed\r\n");
        return;
    }
    else
    {
        printf("flash write sucessful \r\n");
        printf("flash write end offset is %d\r\n",offset);
    }

    memset(&rd_buf, 0, sizeof(rd_buf));
    offset = 0x00;
    printf("flash read start offset is %d\r\n",offset);
    ret = duet_flash_read(param_part, (uint32_t*)&offset, rd_buf, strlen(test_string));
    if(ret == 0)
    {
        printf("flash read buffer data is: %s\r\n", rd_buf);
        printf("flash read end offset is %d\r\n", offset);
    }
    else
    {
        printf("flash read failed\r\n");
        return;
    }
    printf("flash test all pass!!\r\n");
}

int main(void)
{
    printf_uart_log_init(UART1_INDEX);

    flash_api_test();

    while(1);
}
