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

#include "asr_ecc.h"
#include "asr_sec_hw_common.h"
#include <stdlib.h>
#include <stdio.h>
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

#define SHARED_SECRET_MAX_LENGHT         250
#define MAX_TEST_DATA_SIZE               256
#define ECC_SIGN_TESTS_NUMBER            0x2
#define ECC_DH_TESTS_NUMBER              0x2
typedef struct eccSignDataStuct{
    uint8_t                   eccTest_name[MAX_TEST_DATA_SIZE];
    ECPKI_DomainID_t          eccTest_DomainID;
    ECPKI_HASH_OpMode_t       eccTest_HashMode;
    uint8_t                   eccTest_InputData[MAX_TEST_DATA_SIZE];
    uint32_t                  eccTest_InputDataSize;
    uint8_t                   eccTest_PubKey[MAX_TEST_DATA_SIZE];
    uint32_t                  eccTest_PubKeySize;
    uint8_t                   eccTest_PrivKey[MAX_TEST_DATA_SIZE];
    uint32_t                  eccTest_PrivKeySize;
    uint32_t                  eccTest_SignitureSize;
} eccSignDataStuct;

typedef struct eccDHDataStuct{
    uint8_t                   eccDHTest_name[MAX_TEST_DATA_SIZE];
    ECPKI_DomainID_t          eccDHTest_DomainID;
    uint8_t                   eccDHTest_PubKey1[MAX_TEST_DATA_SIZE];
    uint32_t                  eccDHTest_PubKeySize1;
    uint8_t                   eccDHTest_PrivKey1[MAX_TEST_DATA_SIZE];
    uint32_t                  eccDHTest_PrivKeySize1;
    uint8_t                   eccDHTest_PubKey2[MAX_TEST_DATA_SIZE];
    uint32_t                  eccDHTest_PubKeySize2;
    uint8_t                   eccDHTest_PrivKey2[MAX_TEST_DATA_SIZE];
    uint32_t                  eccDHTest_PrivKeySize2;
} eccDHDataStuct;
eccSignDataStuct eccSignVectors[] = {
    {
        /*Test parameters*/
        {"ECC/Domain-secp256r1/HASH_SHA1/128ByteInputData/Build_external_keys"},
        /*ECPKI_DomainID*/
        ECPKI_DomainID_secp256r1,
        /*ECPKI_HASH_OpMode_t*/
        ECPKI_HASH_SHA1_mode,
        /*input data*/
        {0xd4,0xcd,0xa5,0x8a,0x83,0xcd,0xbb,0x42,0xac,0x9f,0xa5,0xa2,0x6e,0xee,0x31,0xd1,
         0xa7,0x3a,0xb1,0x6b,0xcd,0x2a,0xfc,0x79,0x19,0x23,0xb5,0xa0,0xeb,0x19,0xd0,0x98,
         0x6a,0xa5,0x51,0xe0,0x6a,0x58,0x57,0xb0,0x94,0x50,0x5a,0x30,0xc0,0x08,0xb1,0xe7,
         0xa6,0x74,0x57,0x76,0xb6,0x4e,0xee,0x86,0x44,0xab,0x04,0xff,0xe8,0xd1,0x96,0x7a,
         0x25,0x4c,0xd2,0xfa,0x48,0x3f,0x21,0xd6,0x93,0xf9,0x65,0xfa,0x9b,0xc8,0x7f,0x50,
         0xef,0x12,0x15,0x78,0xfa,0xa1,0x92,0xbf,0x2b,0x3f,0x6e,0x4f,0x53,0x83,0xaf,0xa3,
         0x4e,0xea,0xb0,0x3e,0xe5,0x29,0x21,0x9d,0xf3,0xc2,0x4f,0x6a,0xc7,0xd6,0xa5,0xf2,
         0xca,0x3a,0x73,0xd9,0x61,0xcd,0xef,0x0c,0x15,0x08,0x7a,0xf8,0xf2,0xd6,0x24,0xfa,
         0xa1,0xd1,0x45,0xcf,0xac,0x15,0x8b,0x04,0x3b,0x26,0x62,0xa8,0x17,0x78,0xf7,0x81,
         0x01,0x9a,0x34,0x26,0x46,0x74,0xee,0x61,0x93,0xcd,0x60,0xd8,0x0b,0x10,0xd8},
        /*input data size*/
        128,
        /*PublKey*/
        {0x04,
         0x2a,0xe4,0xa3,0xca,0x17,0x5e,0x29,0x90,0xfa,0x1e,0x8b,0x67,0x7d,0x33,0x6b,0x4c,
         0xbc,0xe2,0xd8,0x53,0xd7,0x0f,0x37,0x81,0x86,0xe4,0x5b,0x39,0x0b,0x52,0x3d,0xa7,
         0x09,0xaf,0x5b,0xa5,0x62,0x5e,0x73,0xed,0x88,0x3b,0xe1,0xe7,0xac,0x48,0x30,0x06,
         0xa7,0x0f,0x66,0xad,0xf3,0x1f,0xbb,0xe5,0x89,0x1b,0x4b,0x0d,0xa4,0x6c,0xaa,0x07},
        /*PublKeySize*/
        65,
        /*PrivKey*/
        {0xf0,0xac,0xa4,0x26,0x28,0x52,0x83,0xa6,0x8c,0xc3,0x4f,0xe7,0xd8,0x62,0xc6,0x2a,
         0xcb,0x33,0xd1,0x4c,0x34,0x0a,0x7d,0xb9,0xa8,0x9b,0x4d,0x6f,0xa0,0xfa,0xb6,0x38},
        /*PrivlKeySize*/
        32,
        /*Signiture Size*/
        64,
    },
    {
            /*Test parameters*/
        {"ECC/Domain-secp256r1/HASH_SHA1/128ByteInputData/Generate_Keys"},
        /*ECPKI_DomainID*/
        ECPKI_DomainID_secp256r1,
        /*ECPKI_HASH_OpMode_t*/
        ECPKI_HASH_SHA1_mode,
        /*input data*/
        {0xd4,0xcd,0xa5,0x8a,0x83,0xcd,0xbb,0x42,0xac,0x9f,0xa5,0xa2,0x6e,0xee,0x31,0xd1,
         0xa7,0x3a,0xb1,0x6b,0xcd,0x2a,0xfc,0x79,0x19,0x23,0xb5,0xa0,0xeb,0x19,0xd0,0x98,
         0x6a,0xa5,0x51,0xe0,0x6a,0x58,0x57,0xb0,0x94,0x50,0x5a,0x30,0xc0,0x08,0xb1,0xe7,
         0xa6,0x74,0x57,0x76,0xb6,0x4e,0xee,0x86,0x44,0xab,0x04,0xff,0xe8,0xd1,0x96,0x7a,
         0x25,0x4c,0xd2,0xfa,0x48,0x3f,0x21,0xd6,0x93,0xf9,0x65,0xfa,0x9b,0xc8,0x7f,0x50,
         0xef,0x12,0x15,0x78,0xfa,0xa1,0x92,0xbf,0x2b,0x3f,0x6e,0x4f,0x53,0x83,0xaf,0xa3,
         0x4e,0xea,0xb0,0x3e,0xe5,0x29,0x21,0x9d,0xf3,0xc2,0x4f,0x6a,0xc7,0xd6,0xa5,0xf2,
         0xca,0x3a,0x73,0xd9,0x61,0xcd,0xef,0x0c,0x15,0x08,0x7a,0xf8,0xf2,0xd6,0x24,0xfa,
         0xa1,0xd1,0x45,0xcf,0xac,0x15,0x8b,0x04,0x3b,0x26,0x62,0xa8,0x17,0x78,0xf7,0x81,
         0x01,0x9a,0x34,0x26,0x46,0x74,0xee,0x61,0x93,0xcd,0x60,0xd8,0x0b,0x10,0xd8},
        /*input data size*/
        128,
        /*PublKey*/
        {0},
        /*PublKeySize*/
        0,
        /*PrivKey*/
        {0},
        /*PrivlKeySize*/
        0,
        /*Signiture Size*/
        64,
    }

};

eccDHDataStuct eccDHVectors[] = {
    {
        /*Test parameters*/
        {"DH/Domain-secp256r1/ExternalKeys"},
        /*ECPKI_DomainID*/
        ECPKI_DomainID_secp256r1,
        /*PublKey 1*/
        {0x04,
         0x4e,0x70,0x81,0xa8,0x89,0xb6,0x26,0x9e,0xd6,0xad,0xa9,0xa2,0x5a,0xa4,0x9b,0x9f,
         0x47,0x85,0x01,0x53,0xe3,0x41,0xdb,0xa9,0xa1,0x9f,0xfa,0xbe,0xe0,0xf7,0xe6,0x50,
         0xcb,0x2a,0x00,0x0a,0x54,0x21,0x70,0x4b,0x6c,0xfb,0x79,0xe1,0xe3,0x0f,0xc7,0x23,
         0xf3,0x00,0x4a,0x12,0x7e,0xa8,0x5b,0xb9,0xd1,0x9e,0x46,0x75,0xf7,0x06,0xb0,0x2f},
        /*PublKeySize 1*/
        65,
        /*PrivKey 1*/
        {0x3e,0xa3,0x6f,0x17,0xa6,0x1a,0x2b,0xeb,0xff,0xba,0x58,0x3d,0xb5,0x9a,0x0c,0xd0,
         0x0a,0xff,0x50,0x25,0xab,0xbc,0xdb,0x6b,0x1b,0xe8,0x2c,0xe4,0x51,0xcf,0x3c,0x9b},
        /*PrivlKeySize 1*/
        32,
         /*PublKey 2*/
        {0x04,
         0x09,0x59,0x82,0xfc,0x5f,0xa8,0x2a,0x0d,0x19,0x3e,0x64,0x72,0x61,0x8d,0x9a,0x78,
         0x45,0x44,0x7f,0xdc,0x72,0x19,0xca,0x5f,0x0a,0x6b,0xfd,0x60,0xb2,0x10,0x2d,0xd8,
         0x5a,0x3a,0x0c,0xdb,0xb3,0xf1,0x2b,0xf2,0x40,0x5c,0xa4,0x3c,0x00,0xdf,0xc5,0x8a,
         0xa6,0x80,0x60,0x73,0x0b,0xee,0xe7,0x73,0xeb,0x79,0xd5,0x16,0xbb,0xc0,0x61,0x6c},
        /*PublKeySize 2*/
        65,
        /*PrivKey 2*/
        {0x53,0x98,0xdb,0xf1,0x28,0xce,0xc4,0xb3,0x79,0x9b,0x56,0x17,0xa8,0xac,0xf2,0x76,
         0x83,0x66,0xf9,0x21,0x15,0x2f,0x4b,0x4d,0xb2,0x5b,0x1d,0x24,0xcc,0x37,0xde,0x13},
        /*PrivlKeySize 2*/
        32
    },
        {
        /*Test parameters*/
        {"DH/Domain-secp256r1/Generatedkeys"},
        /*ECPKI_DomainID*/
        ECPKI_DomainID_secp256r1,
        /*PublKey 1*/
        {0x0},
        /*PublKeySize 1*/
        0,
        /*PrivKey 1*/
        {0x0},
        /*PrivlKeySize 1*/
        0,
         /*PublKey 2*/
        {0x0},
        /*PublKeySize 2*/
        0,
        /*PrivKey 2*/
        {0x0},
        /*PrivlKeySize 2*/
        0
    }
};


 int duet_ecc_dh_tests(void)
{
    int                             ret = 0;
    int                             test_index;
    uint8_t                         *pubKeyBuff1;
    uint8_t                         *privKeyBuff1;
    uint8_t                         *pubKeyBuff2;
    uint8_t                         *privKeyBuff2;
    ECPKI_UserPrivKey_t             UserPrivKey1;
    ECPKI_UserPublKey_t             UserPublKey1;
    ECPKI_UserPrivKey_t             UserPrivKey2;
    ECPKI_UserPublKey_t             UserPublKey2;
    ECPKI_BUILD_TempData_t          TempBuff;
    ECDH_TempData_t                 *TempDHBuffptr;
    ECPKI_KG_TempData_t             *TempECCKGBuffptr;
    ECPKI_KG_FipsContext_t          FipsBuff;
    uint8_t                         *sharedSecret1ptr;
    uint8_t                         *sharedSecret2ptr;
    const ECPKI_Domain_t            *pDomain;

#ifndef DX_LINUX_PLATFORM
    ECDH_TempData_t                 TempDHBuff;
    ECPKI_KG_TempData_t             TempECCKGBuff;
    uint8_t                         sharedSecret1[SHARED_SECRET_MAX_LENGHT];
    uint8_t                         sharedSecret2[SHARED_SECRET_MAX_LENGHT];
#endif
    uint32_t                        sharedSecret1Size = SHARED_SECRET_MAX_LENGHT;
    uint32_t                        sharedSecret2Size = SHARED_SECRET_MAX_LENGHT;

    /*Run all ECC tests*/
    for (test_index = 0; test_index < ECC_DH_TESTS_NUMBER; test_index++)
    {
        /*In case of non Linux platform set data pointers to test's vectors */
        pubKeyBuff1 = eccDHVectors[test_index].eccDHTest_PubKey1;
        privKeyBuff1 = eccDHVectors[test_index].eccDHTest_PrivKey1;
        pubKeyBuff2 = eccDHVectors[test_index].eccDHTest_PubKey2;
        privKeyBuff2 = eccDHVectors[test_index].eccDHTest_PrivKey2;
        INTEG_TEST_PRINT(" Test Parameters : %s \r\n", eccDHVectors[test_index].eccDHTest_name);
        sharedSecret1ptr = sharedSecret1;
        sharedSecret2ptr = sharedSecret2;
        TempDHBuffptr = (ECDH_TempData_t*)&TempDHBuff;
        TempECCKGBuffptr = (ECPKI_KG_TempData_t*)&TempECCKGBuff;
        pDomain = asr_ecpki_getEcDomain(eccDHVectors[test_index].eccDHTest_DomainID);

        if (test_index > 0){
            /*Generate first key pair*/
            ret = asr_ecpki_genkeypair(rndContext_ptr,
                pDomain,
                &UserPrivKey1,
                &UserPublKey1,
                TempECCKGBuffptr,&FipsBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_genkeypair generate first key pair Failed, ret is %#x \r\n", ret);
                return ret;
            }

            /*Generate second key pair*/
            ret = asr_ecpki_genkeypair(rndContext_ptr,
                pDomain,
                &UserPrivKey2,
                &UserPublKey2,
                TempECCKGBuffptr,&FipsBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_genkeypair generate second key pair Failed, ret is %#x \r\n", ret);
                return ret;
            }

        } else {
            /*Call asr_ecpki_buildprivkey to create private key1 from external key buffer*/
            ret = asr_ecpki_buildprivkey(pDomain,
                privKeyBuff1,
                eccDHVectors[test_index].eccDHTest_PrivKeySize1,
                &UserPrivKey1);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_buildprivkey build private key1 Failed, ret is %#x \r\n", ret);
                return ret;
            }

            /*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key1 from external key buffer*/
            ret = asr_ecpki_build_pubkey_fullcheck(pDomain,
                pubKeyBuff1,
                eccDHVectors[test_index].eccDHTest_PubKeySize1,
                &UserPublKey1,
                &TempBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_build_pubkey_fullcheck build public key1 Failed, ret is %#x \r\n", ret);
                return ret;
            }

            /*Call asr_ecpki_buildprivkey to create private key2 from external key buffer*/
            ret = asr_ecpki_buildprivkey(pDomain,
                privKeyBuff2,
                eccDHVectors[test_index].eccDHTest_PrivKeySize2,
                &UserPrivKey2);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_buildprivkey build private key2 Failed, ret is %#x \r\n", ret);
                return ret;
            }

            /*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key2 from external key buffer*/
            ret = asr_ecpki_build_pubkey_fullcheck(pDomain,
                pubKeyBuff2,
                eccDHVectors[test_index].eccDHTest_PubKeySize2,
                &UserPublKey2,
                &TempBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_build_pubkey_fullcheck build public key2 Failed, ret is %#x \r\n", ret);
                return ret;
            }
        }

        /* Generating the Secret for user 1*/
        /*---------------------------------*/
        ret = asr_ecdh_svdp_dh(&UserPublKey2,
            &UserPrivKey1,
            sharedSecret1ptr,
            &sharedSecret1Size,
            TempDHBuffptr);

        if (ret != ASR_SILIB_RET_OK){
            INTEG_TEST_PRINT(" asr_ecdh_svdp_dh generate secret for user 1 Failed, ret is %#x \r\n", ret);
            return ret;
        }

        /* Generating the Secret for user 2*/
        /*---------------------------------*/
        ret = asr_ecdh_svdp_dh(&UserPublKey1,
            &UserPrivKey2,
            sharedSecret2ptr,
            &sharedSecret2Size,
            TempDHBuffptr);

        if (ret != ASR_SILIB_RET_OK){
            INTEG_TEST_PRINT(" asr_ecdh_svdp_dh generate secret for user 2 Failed, ret is %#x \r\n", ret);
            return ret;
        }

        /* Verifying we have the same Secret Key both for user1 & user2   */
        /*----------------------------------------------------------------*/
        ret = (uint32_t)memcmp(sharedSecret1ptr ,
            sharedSecret2ptr,
            sharedSecret2Size);

        if (ret != ASR_SILIB_RET_OK){
            INTEG_TEST_PRINT(" Failed, ret is %#x \r\n", ret);
            return ret;
        }
        INTEG_TEST_PRINT(" Passed \r\n");
    }

return ret;
}

int duet_ecc_sign_verify_tests(void)
{
    int                            ret = 0;
    int                             test_index;
    uint8_t                         dataInBuff[128];
    uint8_t                         signOutBuff[64];
    uint8_t                         pubKeyBuff[65];
    uint8_t                         privKeyBuff[32];
    ECPKI_UserPrivKey_t             UserPrivKey;
    ECPKI_UserPublKey_t             UserPublKey;
    ECPKI_BUILD_TempData_t          TempBuff;
    ECDSA_SignUserContext_t         SignUserContext;
    ECDSA_VerifyUserContext_t       VerifyUserContext;
    ECPKI_KG_TempData_t             TempECCKGBuff;
    ECPKI_KG_FipsContext_t          FipsBuff;
    const ECPKI_Domain_t            *pDomain;

    /*Run all ECC tests*/
    for (test_index = 0; test_index < ECC_SIGN_TESTS_NUMBER; test_index++)
    {
        memset(dataInBuff, 0, 128);
        memset(signOutBuff, 0, 64);
        memset(pubKeyBuff, 0, 65);
        memset(privKeyBuff, 0, 32);

        /*Copy input text data to input buffer*/
        memcpy(dataInBuff, eccSignVectors[test_index].eccTest_InputData, eccSignVectors[test_index].eccTest_InputDataSize);
        /*Copy public key data to pubKeyBuff buffer*/
        memcpy(pubKeyBuff, eccSignVectors[test_index].eccTest_PubKey, eccSignVectors[test_index].eccTest_PubKeySize);
        /*Copy private key data to pubKeyBuff buffer*/
        memcpy(privKeyBuff, eccSignVectors[test_index].eccTest_PrivKey, eccSignVectors[test_index].eccTest_PrivKeySize);
        INTEG_TEST_PRINT(" Test Parameters : %s \r\n", eccSignVectors[test_index].eccTest_name);
        pDomain = asr_ecpki_getEcDomain(eccDHVectors[test_index].eccDHTest_DomainID);

        if (test_index > 0){
            /*Generate first key pair*/
            ret = asr_ecpki_genkeypair (rndContext_ptr,
                pDomain,
                &UserPrivKey,
                &UserPublKey,
                &TempECCKGBuff,&FipsBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_genkeypair generate first key pair Failed, ret is %#x \r\n", ret);
                return ret;
            }
        } else {
            /*Call CRYS_ECPKI_BuildPrivKey to create private key from external key buffer*/
            ret = asr_ecpki_buildprivkey(pDomain,
                privKeyBuff,
                eccSignVectors[test_index].eccTest_PrivKeySize,
                &UserPrivKey);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_genkeypair build private key Failed, ret is %#x \r\n", ret);
                return ret;
            }

            /*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key from external key buffer*/
            ret = asr_ecpki_build_pubkey_fullcheck(pDomain,
                pubKeyBuff,
                eccSignVectors[test_index].eccTest_PubKeySize,
                &UserPublKey,
                &TempBuff);

            if (ret != ASR_SILIB_RET_OK){
                INTEG_TEST_PRINT(" asr_ecpki_build_pubkey_fullcheck build public key Failed, ret is %#x \r\n", ret);
                return ret;
            }
        }

        /*Call CRYS_ECDSA_Sign to create signature from input buffer using created private key*/
        ret = asr_ecdsa_sign(rndContext_ptr,
            &SignUserContext,
            &UserPrivKey,
            eccSignVectors[test_index].eccTest_HashMode,
            dataInBuff,
            eccSignVectors[test_index].eccTest_InputDataSize,
            signOutBuff,
            &eccSignVectors[test_index].eccTest_SignitureSize);

        if (ret != ASR_SILIB_RET_OK){
            INTEG_TEST_PRINT(" asr_ecdsa_sign build signature Failed, ret is %#x \r\n", ret);
            return ret;
        }

        /*Call asr_ecdsa_verify to verify the signature using created public key*/
        ret =  asr_ecdsa_verify(&VerifyUserContext,
            &UserPublKey,
            eccSignVectors[test_index].eccTest_HashMode,
            signOutBuff,
            eccSignVectors[test_index].eccTest_SignitureSize,
            dataInBuff,
            eccSignVectors[test_index].eccTest_InputDataSize);

        if (ret != ASR_SILIB_RET_OK){
            INTEG_TEST_PRINT(" Verify Failed, ret is %#x \r\n", ret);
            return ret;
        }
        INTEG_TEST_PRINT(" Verify Passed \r\n");
    }

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
    duet_ecc_dh_tests();
    duet_ecc_sign_verify_tests();
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