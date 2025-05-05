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

#include "asr_hash.h"
#include "asr_sec_hw_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duet_pinmux.h"
#include "duet_uart.h"
#include "printf_uart.h"

#define INTEG_TEST_PRINT                        printf
#define UART_TX_PIN PAD2
#define UART_RX_PIN PAD3
#define UART_TEST_INDEX UART1_INDEX
#define UART_TEST UART1
#define UART_TEST_INDEX UART1_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0


void uart_recv_test(char data)
{
  //UART_SendData(data);
  UART_SendData(getUartxViaIdx(UART_TEST_INDEX), data);
}

void set_uart_config(uint8_t uart_idx, uint8_t hw_flow_control)
{
  duet_uart_dev_t uart_config_struct = {0};
  duet_uart_struct_init(&uart_config_struct);
  uart_config_struct.port = uart_idx;
  uart_config_struct.priv = uart_recv_test;
  // set user define config
  uart_config_struct.config.baud_rate = UART_BAUDRATE_115200;
  if (hw_flow_control)
    uart_config_struct.config.flow_control = FLOW_CTRL_CTS_RTS;
  else
    uart_config_struct.config.flow_control = FLOW_CTRL_DISABLED;
  duet_uart_init(&uart_config_struct);

}

void set_uart_pinmux(uint8_t uart_idx, uint8_t hw_flow_control)
{
  switch (uart_idx)
  {
    case UART0_INDEX:
      duet_pinmux_config(PAD0, PF_UART0);
      duet_pinmux_config(PAD1, PF_UART0);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD6, PF_UART0);
        duet_pinmux_config(PAD7, PF_UART0);
      }
      break;
    case UART1_INDEX:
      duet_pinmux_config(PAD2, PF_UART1);
      duet_pinmux_config(PAD3, PF_UART1);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD14, PF_UART1);
        duet_pinmux_config(PAD15, PF_UART1);
      }
      break;
    case UART2_INDEX:
      duet_pinmux_config(PAD12, PF_UART2);
      duet_pinmux_config(PAD13, PF_UART2);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD10, PF_UART2);
        duet_pinmux_config(PAD11, PF_UART2);
      }
      break;
    default:
      break;
  }
}

#define HASH_TESTS_NUMBER                0x2
#define MAX_TEST_DATA_SIZE               256
typedef enum OperationType_enum
{
     INTEGRATED_OPERATION   ,
     NON_INTEGRATED_OPERATION  ,
     OperationTypeLast= 0x7FFFFFFF,

}OperationType_t;
typedef struct hashDataStuct{
    uint8_t                 hashTest_Name[MAX_TEST_DATA_SIZE];
    HASH_OperationMode_t    hashTest_TST_OperationMode;
    uint8_t                 hashTest_InputData[MAX_TEST_DATA_SIZE];
    uint32_t                hashTest_InputDataSize;
    HASH_Result_t           hashTest_ExpOutData;
    uint32_t                hashTest_ExpOutDataSize;
} hashDataStuct;
hashDataStuct hashVectors[] = {
    {
        /*Test parameters*/
        {"HASH-SHA1/Input_data_16"},
        /*Hash operation mode*/
        HASH_SHA1_mode,
        /*input_data*/
        {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a},
        /*input_data*/
        16*sizeof(uint8_t),
        /*SHA1 Expected output data*/
        {0x3fb53721,0xd3d034ef,0xc388c31e,0x21de6e96,0x7bd0505f},
        /*SHA1 expected dataSize*/
        HASH_SHA1_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),
    },

    {
        /*Test parameters*/
        {"HASH-SHA256/Input_data_32"},
        /*Hash operation mode*/
        HASH_SHA256_mode,
        /*input_data*/
        {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
         0x65,0xa2,0x32,0xd6,0xbc,0xd0,0xf9,0x39,0xed,0x1f,0xe1,0x28,0xc1,0x3b,0x0e,0x1b},
        /*input_data*/
        32*sizeof(uint8_t),
        /*SHA256 Expected output data*/
        {0x9bb3cf75,0x9274c462,0x97ad2a1e,0x8b0f219c,0x9d0a1869,0x96f2e958,0x4a90b9a4,0x40aae7e6},
        /*SHA1 expected dataSize*/
        HASH_SHA256_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),
    }
};
int duet_hash_tests(void)
{
    int                             ret = 0;
    int                             test_index;
    OperationType_t                 operation_index;
    uint8_t                         *dataInBuff;
    HASH_Result_t                   hashOutBuff={0};
    HASHUserContext_t               ContextID;
    #ifdef DX_LINUX_PLATFORM
    uint32_t* threadReturnValue = malloc(sizeof(uint32_t));
    #endif
    /*Run all HASH tests*/
    for(operation_index = INTEGRATED_OPERATION; operation_index <= NON_INTEGRATED_OPERATION; operation_index++){
        for (test_index = 0; test_index < HASH_TESTS_NUMBER; test_index++)
        {
            /*In case of non Linux platform set data pointers to test's vectors */
            dataInBuff = hashVectors[test_index].hashTest_InputData;
            INTEG_TEST_PRINT(" Test Parameters : %s \r\n", hashVectors[test_index].hashTest_Name);

            if (operation_index == INTEGRATED_OPERATION){ /*Perform Inegrated operation*/
                INTEG_TEST_PRINT(" Integrated operation \r\n");

                ret = asr_hash(hashVectors[test_index].hashTest_TST_OperationMode,
                    dataInBuff,
                    hashVectors[test_index].hashTest_InputDataSize,
                    hashOutBuff);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_hash Failed, ret is %#x \r\n", ret);
                    goto end;
                }
            } else { /*Perform NonInegrated operation*/
                INTEG_TEST_PRINT(" No-Integrated operation \r\n");

                ret = asr_hash_init(&ContextID,
                    hashVectors[test_index].hashTest_TST_OperationMode);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_hash_init Failed, ret is %#x \r\n", ret);
                    goto end;
                }

                ret = asr_hash_update(&ContextID,
                    dataInBuff,
                    hashVectors[test_index].hashTest_InputDataSize);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_hash_update Failed, ret is %#x \r\n", ret);
                    goto end;
                }

                ret = asr_hash_finish(&ContextID ,
                    hashOutBuff);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_hash_finish Failed, ret is %#x \r\n", ret);
                    goto end;
                }
            }
            /*Compare HASH output with expected data*/
            ret = memcmp(hashOutBuff,hashVectors[test_index].hashTest_ExpOutData,hashVectors[test_index].hashTest_ExpOutDataSize);


            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" Failed, ret is %#x \r\n", ret);
                goto end;
            }
            INTEG_TEST_PRINT(" Passed \r\n");
        }
    }
end:
    return ret;
}

int main(void)
{
    uint8_t uart_idx        = UART_TEST_INDEX;
    uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
    // set pin mux
    set_uart_pinmux(uart_idx, hw_flow_control);
    set_uart_config(uart_idx, hw_flow_control);
    printf_uart_register(uart_idx);
    asr_security_engine_init();
    duet_hash_tests();
    asr_security_engine_deinit();
    while(1);
}

/*
void * _sbrk(int nbytes)
{
    //variables from linker script
    extern char _end[];
    extern char _estack[];
    extern char _Min_Stack_Size[];

    static char *heap_ptr = _end;

    if((unsigned int)(_estack - (heap_ptr+nbytes)) > (unsigned int)_Min_Stack_Size)
    {
        void *base = heap_ptr;
        heap_ptr += nbytes;
        return base;
    }
    else
    {
        return (void *)(-1);
    }
}
*/