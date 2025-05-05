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

#define FLASH_TEST_LENGTH   0x1000

#define FLASH_PARTITION0    PARTITION_PARAMETER_1
#define FLASH_PARTITION1    PARTITION_PARAMETER_2
#define FLASH_PARTITION2    PARTITION_PARAMETER_3
#define FLASH_PARTITION3    PARTITION_APPLICATION
#define FLASH_PARTITION4    PARTITION_OTA_TEMP

#define FLASH_PARTITION    FLASH_PARTITION4

void flash_test_api_test(void)
{
    duet_logic_partition_t *partition = duet_flash_get_info(FLASH_PARTITION);
    unsigned char in_buf[FLASH_TEST_LENGTH];
    unsigned char out_buf[FLASH_TEST_LENGTH];
    int32_t ret;
    unsigned long offset;
    uint32_t i;
    uint32_t j;
    unsigned char bValue = 0;
    printf("flash test begin\r\n");
    for(i = 0; i < FLASH_TEST_LENGTH; i++)
    {
        in_buf[i] = bValue++;
    }

    //get partition info
    printf("flash partition info:\r\n\r\n");
    printf("partition_owner:\t%d\r\n", partition->partition_owner);
    printf("partition_owner:\t%s\r\n", partition->partition_description);
    printf("partition_start_addr:\t0x%x\r\n", (unsigned int)partition->partition_start_addr);
    printf("partition_length:\t%dKB\r\n", (unsigned int)partition->partition_length/1024);
    printf("partition_options:\t");
    if(partition->partition_options & PAR_OPT_READ_EN)
        printf("r");
    if(partition->partition_options & PAR_OPT_WRITE_EN)
        printf("w");
    printf("\n\n");

    for(j = 0; j < partition->partition_length/FLASH_TEST_LENGTH; j++)
    {
        offset = FLASH_TEST_LENGTH *j;
        int param_part = FLASH_PARTITION;

        ret = duet_flash_init();    //all interrupt must be disabled when flash initial

        ret = duet_flash_erase(param_part, offset, FLASH_TEST_LENGTH);
        if(ret != 0)
        {
            printf("flash erase failed\r\n");
        }
        memset(out_buf, 0, FLASH_TEST_LENGTH);
        offset = FLASH_TEST_LENGTH * j;
        printf("flash read start offset is 0x%x\r\n", (unsigned int)offset);
        ret = duet_flash_read(param_part, (uint32_t*)&offset, out_buf, FLASH_TEST_LENGTH);
        offset = FLASH_TEST_LENGTH * j;
        if(ret == 0)
        {
            for(i = 0; i < FLASH_TEST_LENGTH; i++)
            {
                if(out_buf[i] != 0xFF)
                {
                    printf("0x%x erase test fail!!! \r\n", (unsigned int)(partition->partition_start_addr + offset + i));
                    goto EXIT;
                }
            }
            printf("flash read end\r\n");
        }
        else
            printf("flash erase failed!\r\n");

        printf("flash write start offset is 0x%x\r\n", (unsigned int)offset);
        ret = duet_flash_write(param_part, (uint32_t*)&offset, in_buf, FLASH_TEST_LENGTH);
        if(ret != 0)
        {
            printf("flash write failed\r\n");
        }
        printf("flash write end offset is 0x%x\r\n", (unsigned int)offset);

        memset(out_buf, 0, FLASH_TEST_LENGTH);
        offset = FLASH_TEST_LENGTH * j;
        printf("flash read start offset is 0x%x\r\n", (unsigned int)offset);
        ret = duet_flash_read(param_part, (uint32_t*)&offset, out_buf, FLASH_TEST_LENGTH);
        offset = FLASH_TEST_LENGTH * j;
        if(ret == 0)
        {
            for(i = 0; i < FLASH_TEST_LENGTH; i++)
            {
                if(out_buf[i] != in_buf[i])
                {
                    printf("0x%x write/read test fail!!! \r\n", (unsigned int)(partition->partition_start_addr + offset + i));
                    goto EXIT;
                }
            }
            printf("flash read end\r\n");
        }
        else
            printf("flash write/read failed!\r\n");
    }
    printf("\r\nhal flash memory verify OK\n"
            "partition:%s\n"
            "start_addr:0x%x\n"
            "length:%dKB\r\n",
            partition->partition_description,
            (unsigned int)partition->partition_start_addr,
            (unsigned int)partition->partition_length/1024);
EXIT:
    while(1);
}

int main(void)
{
    printf_uart_log_init(UART1_INDEX);

    flash_test_api_test();

    while(1);
}
