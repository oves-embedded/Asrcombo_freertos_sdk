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
#include "asr_rnd.h"
#include "asr_sec_hw_common.h"
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

#define MAX_TEST_DATA_SIZE               256
#define MAX_RND_SIZE_VECTOR              0x20
#define RND_STATE_VALIDATION_TAG_ERROR            (RND_MODULE_ERROR_BASE + 0xEUL)

typedef struct rndtesDataStuct
{
    uint8_t  hmacTest_Name[MAX_TEST_DATA_SIZE];
    uint32_t rndTest_AddInputSize;
    uint8_t  rndTest_AddInputData[MAX_RND_SIZE_VECTOR];
    uint32_t rndTest_RandomVectorSize;
    uint8_t  rndTest_RandomVectorData1[MAX_RND_SIZE_VECTOR];
    uint8_t  rndTest_RandomVectorData2[MAX_RND_SIZE_VECTOR];
    uint32_t rndTest_RandomVectorInRangeSize1;
    uint8_t  rndTest_RandomVectorInRangeData[MAX_RND_SIZE_VECTOR];
    uint32_t rndTest_RandomVectorInRangeSize2;
    uint8_t  rndTest_MaxVectorInRange2[MAX_RND_SIZE_VECTOR];
    uint8_t  rndTest_RandomVectorInRangeData2[MAX_RND_SIZE_VECTOR];

} duet_rndDataStuct;

 duet_rndDataStuct rndVectors[] = {{
                                          /*Test parameters*/
                                          {"RND test"},
                                          /*Additional input size*/
                                          16,  //MAX 48 bytes
                                          /*Additional input data*/
                                          {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
                                          /*Size for random vectors*/
                                          16,
                                          /*First random vector*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                                          /*Second random vector*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                                          /*RandomVectorInRangeSize1*/
                                          32 * sizeof(uint8_t),
                                          /*Random vector in range 1*/
                                          {
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                          },
                                          /*RandomVectorInRangeSize2*/
                                          16 *
                                          sizeof(uint8_t), {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
                                          /*Random vector in range 2*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                                  }
                                  ,

};
/*rnd_test - performs basic integration test for RND module*/
int duet_rnd_tests(void)
{
    uint32_t                        ret = 0/*,MaxVectorSize = 0*/;
    int                             test_index = 0;

    /*Set additional input for rng seed*/
    ret = asr_RND_AddAdditionalInput(rndContext_ptr,
                                      rndVectors[test_index].rndTest_AddInputData,
                                      rndVectors[test_index].rndTest_AddInputSize);


    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput passed\r\n");

    /*Reseed rnd using added input (new seed will be generated using additional input)*/
    ret = asr_RND_Reseeding (rndContext_ptr,
                              rndWorkBuff_ptr);
    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_Reseeding failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_Reseeding passed\r\n");

    /*Generate random vector 1*/
    ret = asr_RND_GenerateVector(&rndContext_ptr->rndState,
                                  rndVectors[test_index].rndTest_RandomVectorSize,
                                  rndVectors[test_index].rndTest_RandomVectorData1);
    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVector for vector 1 failed with 0x%x \r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_GenerateVector for first vector passed\r\n");
    /*Generate rnadom vector 2*/
    ret = asr_RND_GenerateVector(&rndContext_ptr->rndState,
                                  rndVectors[test_index].rndTest_RandomVectorSize,
                                  rndVectors[test_index].rndTest_RandomVectorData2);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVector for vector 2 failed with 0x%x \r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_GenerateVector for second vector passed\r\n");

    /*Compare two generated vectors - should not be the same value*/
    ret = memcmp(rndVectors[test_index].rndTest_RandomVectorData1,
                         rndVectors[test_index].rndTest_RandomVectorData2,
                         rndVectors[test_index].rndTest_RandomVectorSize);

    if (ret == ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n Two random vectors should not are the same \r\n");
        return ret;
    }

    INTEG_TEST_PRINT("\n Two generated vectors are different as expected\r\n");

    /*Generate random vector in range when max value is NULL*/
    ret = asr_RND_GenerateVectorInRange(rndContext_ptr,
                                         rndVectors[test_index].rndTest_RandomVectorInRangeSize1,
                                         NULL,
                                         rndVectors[test_index].rndTest_RandomVectorInRangeData);


    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVectorInRange for vector with fixed size failed with 0x%x \r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_GenerateVectorInRange 1 passed\r\n");

    /*Generate random vector in range with max vector */
    ret = asr_RND_GenerateVectorInRange(rndContext_ptr,
                                         rndVectors[test_index].rndTest_RandomVectorInRangeSize2,
                                         rndVectors[test_index].rndTest_MaxVectorInRange2,
                                         rndVectors[test_index].rndTest_RandomVectorInRangeData2);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVectorInRange failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_GenerateVectorInRange 2 passed\r\n");


    /*Perform UnInstantiation*/
    ret = asr_RND_UnInstantiation(rndContext_ptr);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_UnInstantiation failed with 0x%x \r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_UnInstantiation passed\r\n");

    /*Try to create random vector without instantiation - should fail*/
    ret = asr_RND_GenerateVector(&rndContext_ptr->rndState,
                                  rndVectors[test_index].rndTest_RandomVectorSize,
                                  rndVectors[test_index].rndTest_RandomVectorData2);

    if (ret != RND_STATE_VALIDATION_TAG_ERROR){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVector returned wrong error 0x%x,\n CRYS_RND_STATE_VALIDATION_TAG_ERROR should be returned\r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_GenerateVector failed as expected\r\n");

    /*Set additional input for RND seed*/
    ret = asr_RND_AddAdditionalInput(rndContext_ptr,
                                      rndVectors[test_index].rndTest_AddInputData,
                                      rndVectors[test_index].rndTest_AddInputSize);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput failed with 0x%x \r\n",(int)ret);
        return ret;
    }

    INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput passed\r\n");

    /*Perform instantiation for new seed*/
    ret = asr_RND_Instantiation(rndContext_ptr,
                                 rndWorkBuff_ptr);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_AddAdditionalInput passed\r\n");

    /*Try to create two vectors and check that the vectors are different*/
    ret = asr_RND_GenerateVector(&rndContext_ptr->rndState,
                                  rndVectors[test_index].rndTest_RandomVectorSize,
                                  rndVectors[test_index].rndTest_RandomVectorData1);
    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT("\n asr_RND_GenerateVector for vector 1 failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_GenerateVector 1 passed\r\n");

    ret = asr_RND_GenerateVector(&rndContext_ptr->rndState,
                                  rndVectors[test_index].rndTest_RandomVectorSize,
                                  rndVectors[test_index].rndTest_RandomVectorData2);

    if (ret != ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT(" asr_RND_GenerateVector for vector 2 failed with 0x%x \r\n",(int)ret);
        return ret;
    }
    INTEG_TEST_PRINT("\n asr_RND_GenerateVector 2 passed\r\n");

    ret = memcmp(rndVectors[test_index].rndTest_RandomVectorData1,
                         rndVectors[test_index].rndTest_RandomVectorData2,
                         rndVectors[test_index].rndTest_RandomVectorSize);

    if (ret == ASR_SILIB_RET_OK){
        INTEG_TEST_PRINT(" Two random vectors should not are the same \r\n");
        return ret;
    }
    INTEG_TEST_PRINT("\n Compare passed\r\n");
    INTEG_TEST_PRINT("\n All RND tests passed\r\n");
    INTEG_TEST_PRINT("\n==========================\r\n");

    ret = ASR_SILIB_RET_OK;

    return ret;
}

int main(void)
{

    uint8_t uart_idx        = UART_TEST_INDEX;
    uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
    // set pin mux
    set_uart_pinmux(uart_idx, hw_flow_control);
    set_uart_config(uart_idx, hw_flow_control);
    //    duet_uart_start(getUartxViaIdx(uart_idx));
    printf_uart_register(uart_idx);
    asr_security_engine_init();
    duet_rnd_tests();
    asr_security_engine_deinit();
    while (1);
}

