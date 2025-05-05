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
#include "asr_sec_hw_common.h"
#include "asr_hmac.h"
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


//#define INTEG_TEST_PRINT(format, ...)
#define MAX_TEST_DATA_SIZE               256
#define HMAC_TESTS_NUMBER                0x2
typedef enum OperationType_enum
{
     INTEGRATED_OPERATION   ,
     NON_INTEGRATED_OPERATION  ,
     OperationTypeLast= 0x7FFFFFFF,

}OperationType_t;

typedef struct hmacDataStuct{
    uint8_t                 hmacTest_Name[MAX_TEST_DATA_SIZE];
    HASH_OperationMode_t    hmacTest_TST_OperationMode;
    uint8_t                 hmacTest_Key[HMAC_KEY_SIZE_IN_BYTES];
    uint16_t                hmacTest_KeySize;
    uint8_t                 hmacTest_InputData[MAX_TEST_DATA_SIZE];
    uint32_t                hmacTest_InputDataSize;
    HASH_Result_t           hmacTest_ExpOutData;
    uint32_t                hmacTest_ExpOutDataSize;
} duet_hmacDataStuct;

duet_hmacDataStuct hmacVectors[] = {
    {
        /*Test parameters*/
        {"HMAC-SHA1/Input_data_32/Key_32/"},
        /*Hash operation mode*/
        HASH_SHA1_mode,
        /*key*/
        {0xf6,0x4a,0xb2,0x3f,0x7a,0x95,0x92,0x9e,0x9e,0xa6,0x24,0xd9,0x5a,0x68,0x36,0xd5,
        0x12,0x83,0x65,0x0b,0x67,0xdc,0x90,0x2e,0x53,0x5a,0x8d,0x0d,0x75,0xf0,0x1a,0xbc},
        /*keySize */
        32*sizeof(uint8_t),
        /*input_data*/
        {0x57,0x22,0xd9,0xe2,0xed,0x48,0xf4,0x25,0xa8,0xce,0x7f,0x70,0x3d,0xe6,0xef,0x37,
         0xe7,0x32,0x71,0xc1,0x59,0xe4,0x7d,0x74,0xf8,0x24,0xae,0x53,0x47,0xb2,0xd1,0xc5},
        /*input_dataSize*/
        32*sizeof(uint8_t),
        /*SHA1_Expected_output_data*/
        {0x6568524f,0x6783ca5f,0x8f7a8e8e,0x139d0804,0x167b05f2},
        /*SHA1_Expected_output_dataSize*/
        HASH_SHA1_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),
    },
    {
        /*Test parameters*/
        {"HMAC-SHA256/Input_data_65/Key_65/"},
        /*Hash operation mode*/
        HASH_SHA256_mode,
        /*key*/
        {0x15,0xb2,0x9a,0xd8,0xae,0x2a,0xad,0x73,0xa7,0x26,0x43,0x50,0x70,0xe8,0xe9,0xda,
         0x9b,0x47,0x69,0xc3,0xe3,0xa4,0xee,0x99,0x6e,0x20,0x6a,0x9b,0x4f,0x0c,0x35,0xca,
         0x4f,0xa2,0xf7,0x43,0xed,0xf2,0xc7,0xcb,0xa3,0x1e,0x94,0xac,0x6b,0xca,0xc4,0xc0,
         0x82,0xcf,0x1c,0xcb,0x6c,0x2f,0xe0,0x0d,0x38,0x4e,0x3b,0x18,0x05,0x5f,0xe0,0xe0},
        /*keySize */
        64*sizeof(uint8_t),
        /*input_data*/
        {0x99,0xfd,0x18,0xa3,0x5d,0x50,0x81,0x84,0xa6,0xf3,0x61,0xc6,0x7c,0xd9,0xb1,0x0b,
         0x4c,0xd1,0xd8,0xb2,0x46,0x57,0x2a,0x4d,0x03,0xb0,0xae,0x55,0x6b,0x36,0x24,0x1d,
         0xd6,0xf0,0x46,0x05,0x71,0x65,0x4f,0xf0,0xe4,0xb2,0xba,0xf8,0x31,0xdb,0x4c,0x60,
         0xdf,0x5f,0x54,0xc9,0x59,0x0f,0x32,0xa9,0x91,0x1f,0x16,0xfa,0xe8,0x7e,0x0a,0x2f,
         0x52},
        /*input_dataSize*/
        65*sizeof(uint8_t),
        /*SHA256_Expected_output_data*/
        {0xE0903CC8,0x24C89469,0x71B12528,0x6DEFD88C,
         0xF662C7FC,0x971C4DD1,0x5755CB85,0x8E72FD6F},
        /*SAH256_expected_dataSize*/
        HASH_SHA256_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),
    }
};

int duet_hmac_tests(void)
{
    int                             ret = 0;
    int                             test_index,operation_index;
    uint8_t                         *dataInBuff;
    uint8_t                         *keyptr;
    HASH_Result_t                   hmacOutBuff = {0};
    HMACUserContext_t               ContextID;
    /*Run all HMAC tests*/
    for(operation_index = INTEGRATED_OPERATION; operation_index <= NON_INTEGRATED_OPERATION; operation_index++){
        for (test_index = 0; test_index < HMAC_TESTS_NUMBER; test_index++)
        {
            /*In case of non Linux platform set data pointers to test's vectors */
            dataInBuff = hmacVectors[test_index].hmacTest_InputData;
            keyptr = hmacVectors[test_index].hmacTest_Key;
            //hmacOutBuff = hmacVectors[test_index].hmacTest_OutputData;
            INTEG_TEST_PRINT("\n HASH Test numer 0x%x Parameters : \n-----%s-----\n",test_index,hmacVectors[test_index].hmacTest_Name);

            if (operation_index == INTEGRATED_OPERATION){ /*Perform Inegrated operation*/

                INTEG_TEST_PRINT("Integrated operation\n");

                ret = asr_HMAC(hmacVectors[test_index].hmacTest_TST_OperationMode,
                    keyptr,
                    hmacVectors[test_index].hmacTest_KeySize,
                    dataInBuff,
                    hmacVectors[test_index].hmacTest_InputDataSize,
                    hmacOutBuff);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_HMAC failed with 0x%x \n", ret);
                    goto end;
                }
            } else { /*Perform NonInegrated operation*/
                INTEG_TEST_PRINT("Non integrated operation\n");
                ret = asr_HMAC_Init(&ContextID,
                    hmacVectors[test_index].hmacTest_TST_OperationMode,
                    keyptr,
                    hmacVectors[test_index].hmacTest_KeySize);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_HMAC_Init failed with 0x%x \n", ret);
                    goto end;
                }

                ret = asr_HMAC_Update(&ContextID,
                    dataInBuff,
                    hmacVectors[test_index].hmacTest_InputDataSize);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_HMAC_Update failed with 0x%x \n", ret);
                    goto end;
                }

                ret = asr_HMAC_Finish(&ContextID,
                    hmacOutBuff);

                if (ret != ASR_SILIB_RET_OK){
                    INTEG_TEST_PRINT(" asr_HMAC_Finish failed with 0x%x \n", ret);
                    goto end;
                }
            }
            ret = memcmp(hmacOutBuff,hmacVectors[test_index].hmacTest_ExpOutData,hmacVectors[test_index].hmacTest_ExpOutDataSize);
            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_PalMemCmp failed \n");
                goto end;
            }
            INTEG_TEST_PRINT("Passed\n");
            INTEG_TEST_PRINT("==========================\n");
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
    duet_hmac_tests();
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