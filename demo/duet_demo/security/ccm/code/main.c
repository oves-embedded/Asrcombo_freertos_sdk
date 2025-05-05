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
#include "asr_ccm.h"
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
    uart_config_struct.port                    = uart_idx;
    uart_config_struct.priv                    = uart_recv_test;
    // set user define config
    uart_config_struct.config.baud_rate        = UART_BAUDRATE_115200;
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
#define MAX_AES_CCM_DATA_SIZE_VECTOR     0x40

typedef struct aesCCMDataStuct
{
    uint8_t               aesCCMTest_name[MAX_TEST_DATA_SIZE];
    AESCCM_Key_t          aesCCMTest_Key;
    AESCCM_KeySize_t      aesCCMTest_KeySize;
    uint8_t               aesCCMTest_NData[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint32_t              aesCCMTest_NDataSize;
    uint8_t               aesCCMTest_AddData[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint32_t              aesCCMTest_AddDataSize;
    uint8_t               aesCCMTest_TextData[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint32_t              aesCCMTest_TextdataSize;
    uint8_t               aesCCMTest_Expected_output_data[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint8_t               aesCCMTest_Output_data[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint32_t              aesCCMTest_OutputDataSize;
    uint8_t               aesCCMTest_Expected_MAC_output_data[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint8_t               aesCCMTest_Output_MAC_data[MAX_AES_CCM_DATA_SIZE_VECTOR];
    uint32_t              aesCCMTest_Output_CCM_MACSize;
} duet_aesCCMDataStuct_t;

duet_aesCCMDataStuct_t aesCCMVectors[] = {{{"CCM/Integrated/4ByteMac/7ByteNonce/16ByteAdata/16ByteTextData/128Key"},
                                                  /*Key*/
                                                  {0xC6, 0xBB, 0x2D, 0x94, 0x90, 0xB9, 0x65, 0x23, 0x98, 0xED, 0x83, 0x3F, 0x9B, 0x9D, 0x02, 0xCC},
                                                  /*Key size*/
                                                  AES_Key128BitSize,
                                                  /*Nonce data*/
                                                  {0x14, 0xD7, 0xF6, 0xC9, 0x3D, 0x1F, 0x53},
                                                  /*size of Nonce*/
                                                  7,
                                                  /*additional data*/
                                                  {0x06, 0x55, 0xAD, 0xA9, 0x1B, 0xFC, 0x6D, 0xD2, 0x88, 0x27, 0xB7, 0x4A, 0x76, 0x9B, 0xDB, 0xD6},
                                                  /*size of additional data*/
                                                  16,
                                                  /*text data*/
                                                  {0xF7, 0x44, 0x6E, 0xB2, 0x73, 0xED, 0x03, 0xE8, 0x1D, 0x24, 0x14, 0xFA, 0xB8, 0x7D, 0xC6, 0xCB},
                                                  /*data input size*/
                                                  16,
                                                  /*expected data */
                                                  {0x29, 0xFC, 0x76, 0xC8, 0x92, 0x89, 0x65, 0x58, 0xE5, 0x99, 0x8B, 0xBA, 0x89, 0x02, 0xC3, 0xDF},
                                                  /*Output*/
                                                  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 16,
                                                  /* AESCCM_Mac_ExpectedRes */
                                                  {0xFD, 0xAF, 0x7E, 0xF2},
                                                  /*Output Mac*/
                                                  {0x00, 0x00, 0x00, 0x00},
                                                  /*size of CCM-MAC (T) */
                                                  4
                                          }

};

int duet_aes_ccm_tests(void)
{
    uint32_t ret = 1;
    int      test_index;
    uint8_t  *dataInBuff;
    uint8_t  *dataOutBuff;
    uint8_t  *aDataBuff;
    uint8_t  *macOutRes;
    uint8_t  *nonce;
    test_index = 0;
    /*In case of non Linux platform set data pointers to test's vectors */
    dataInBuff  = aesCCMVectors[test_index].aesCCMTest_TextData;
    dataOutBuff = aesCCMVectors[test_index].aesCCMTest_Output_data;
    aDataBuff   = aesCCMVectors[test_index].aesCCMTest_AddData;
    macOutRes   = aesCCMVectors[test_index].aesCCMTest_Output_MAC_data;
    nonce       = aesCCMVectors[test_index].aesCCMTest_NData;
    INTEG_TEST_PRINT(" Test Parameters : %s \r\n", aesCCMVectors[test_index].aesCCMTest_name);
    /*Call asr_AESCCM integrated function*/
    ret = asr_AESCCM(AES_ENCRYPT, aesCCMVectors[test_index].aesCCMTest_Key,
                      aesCCMVectors[test_index].aesCCMTest_KeySize, nonce,//aesCCMVectors[test_index].aesCCMTest_NData,
                      aesCCMVectors[test_index].aesCCMTest_NDataSize,
                      aDataBuff,//aesCCMVectors[test_index].aesCCMTest_AddData,,
                      aesCCMVectors[test_index].aesCCMTest_AddDataSize,
                      dataInBuff,//aesCCMVectors[test_index].aesCCMTest_TextData,
                      aesCCMVectors[test_index].aesCCMTest_TextdataSize,
                      dataOutBuff,//aesCCMVectors[test_index]  uint8_t *TextDataOut_ptr,
                      aesCCMVectors[test_index].aesCCMTest_Output_CCM_MACSize,
                      macOutRes /*aesCCMVectors[test_index]  CRYS_AESCCM_Mac_Res_t MacRes*/);
    if (ret != ASR_SILIB_RET_OK)
    {
        INTEG_TEST_PRINT("asr_AESCCM failed with 0x%x \r\n", (int) ret);
        return ret;
    }
    /*Check output text data with expected data*/
    ret = memcmp(dataOutBuff, aesCCMVectors[test_index].aesCCMTest_Expected_output_data,
                 aesCCMVectors[test_index].aesCCMTest_TextdataSize);
    if (ret != 0)
    {
        INTEG_TEST_PRINT("output text buffer of the test failed\r\n");
        return ret;
    }
    /*Check mac output with expected data*/
    ret = memcmp(macOutRes, aesCCMVectors[test_index].aesCCMTest_Expected_MAC_output_data,
                 aesCCMVectors[test_index].aesCCMTest_Output_CCM_MACSize);
    if (ret != 0)
    {
        INTEG_TEST_PRINT("output mac buffer of the test failed\r\n");
        return ret;
    }

    INTEG_TEST_PRINT("Passed \r\n");

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
    duet_aes_ccm_tests();
    asr_security_engine_deinit();
    while (1);
}
