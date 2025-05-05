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
#include <stdlib.h>
#include <stdio.h>
#include "mbedtls_hw.h"
#include "asr_sec_common.h"
#include "asr_sec_hw_common.h"

// #include "mbedtls/pk.h"
// #include "mbedtls/aes.h"
// #include "mbedtls/ecp.h"
// #include "mbedtls/ecdsa.h"

#if defined(ALIOS_SUPPORT)
#include "lega_rhino.h"
#elif defined(HARMONYOS_SUPPORT)
#include "lega_rtos.h"
#elif defined(SYSTEM_SUPPORT_OS)
#include "lega_rtos_api.h"
#endif

// #define MBEDTLS_ECC_TEST

// #define MBEDTLS_RSA_TEST

// #define MBEDTLS_AES_TEST

// #define MBEDTLS_HASH_TEST

//************************************************************  HASH  ***********************************************************************************
#ifdef MBEDTLS_HASH_TEST

#include "asr_hash.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"

#define HASH_TEST_PRINT(...) printf(__VA_ARGS__)

#define HASH_DATA_LEN (2*1024)
#define HASH_UINT_LEN (1*1024)

#define HASH_VALUE_MAX_LEN 64

#define IS_SHA224 1
#define IS_SHA256 0

int hash_hal_efficiency_test(void)
{
    int in_len = 0, ret = 0;
    int tick1 = 0, tick2 = 0;
    int tick3 = 0, tick4 = 0;

    unsigned char hash_val[HASH_VALUE_MAX_LEN];
    unsigned char hal_hash_val[HASH_VALUE_MAX_LEN];

    unsigned char *DataInBuff = lega_rtos_malloc(HASH_DATA_LEN);

    if(DataInBuff == NULL)
    {
        ret = -1;
        HASH_TEST_PRINT("hash_hal_efficiency_test malloc fail...\r\n");
        goto exit;
    }

    /*************************** function **********************************/
    HASH_TEST_PRINT("hash_hal efficiency test start...\r\n");

    for(int k = 1; k <= HASH_DATA_LEN/HASH_UINT_LEN; k++)
    {
        in_len = k*HASH_UINT_LEN;
        memset(DataInBuff, 0x55, HASH_DATA_LEN);

        HASH_TEST_PRINT("cnt[%d]\r\n", k);

        /*************************** hash step all **********************************/
        tick1 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, hash_val, IS_SHA224);

        tick2 = lega_rtos_get_time();


        tick3 = lega_rtos_get_time();

        ret = asr_hash(HASH_SHA224_mode,
                    (uint8_t *)DataInBuff,
                    in_len,
                    (uint32_t *)hal_hash_val);

        if(ret != 0)
        {
            HASH_TEST_PRINT("hash_hal_efficiency_test function[asr_hash return %d], test failed!\r\n", ret);
            goto exit;
        }

        tick4 = lega_rtos_get_time();

        if(memcmp(hash_val, hal_hash_val, 28) != 0)
        {
            mbed_dump_u8("hash value", hash_val, 28);
            mbed_dump_u8("hw hash value", hal_hash_val, 28);
            HASH_TEST_PRINT("data cmp not equal, hash_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

        HASH_TEST_PRINT("\r\n-%d %d %d %d %d %d--\r\n", tick1, tick2, tick3, tick4);
        HASH_TEST_PRINT("mbedtls_sha224 %d, hal_sha224 %d\r\n", tick2-tick1, tick4-tick3);
        HASH_TEST_PRINT("mbedtls_sha224 all %d K bytes time:---------> %f ms, ", k, (float)(tick2-tick1)/1.0);
        HASH_TEST_PRINT("mbedtls_sha224 1 K bytes time:---------> %f ms, ", (float)(tick2-tick1)/k/1.0);
        HASH_TEST_PRINT("hal_sha224 all %d K bytes time:---------> %f ms, ", k, (float)(tick4-tick3)/1.0);
        HASH_TEST_PRINT("hal_sha224 1 K bytes time:---------> %f ms, ", (float)(tick4-tick3)/k/1.0);

        tick1 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, hash_val, IS_SHA256);

        tick2 = lega_rtos_get_time();

        tick3 = lega_rtos_get_time();

        ret = asr_hash(HASH_SHA256_mode,
                    (uint8_t *)DataInBuff,
                    in_len,
                    (uint32_t *)hal_hash_val);

        if(ret != 0)
        {
            HASH_TEST_PRINT("hash_hal_efficiency_test function[asr_hash return %d], test failed!\r\n", ret);
            goto exit;
        }

        tick4 = lega_rtos_get_time();

        if(memcmp(hash_val, hal_hash_val, 32) != 0)
        {
            mbed_dump_u8("hash value", hash_val, 32);
            mbed_dump_u8("hw hash value", hal_hash_val, 32);
            HASH_TEST_PRINT("data cmp not equal, hash_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

        HASH_TEST_PRINT("\r\n-%d %d %d %d %d %d--\r\n", tick1, tick2, tick3, tick4);
        HASH_TEST_PRINT("mbedtls_sha256 %d, hal_sha256 %d\r\n", tick2-tick1, tick4-tick3);
        HASH_TEST_PRINT("mbedtls_sha256 all %d K bytes time:---------> %f ms, ", k, (float)(tick2-tick1)/1.0);
        HASH_TEST_PRINT("mbedtls_sha256 1 K bytes time:---------> %f ms, ", (float)(tick2-tick1)/k/1.0);
        HASH_TEST_PRINT("hal_sha256 all %d K bytes time:---------> %f ms, ", k, (float)(tick4-tick3)/1.0);
        HASH_TEST_PRINT("hal_sha256 1 K bytes time:---------> %f ms, ", (float)(tick4-tick3)/k/1.0);

        tick1 = lega_rtos_get_time();

        mbedtls_sha1(DataInBuff, in_len, hash_val);

        tick2 = lega_rtos_get_time();

        tick3 = lega_rtos_get_time();

        ret = asr_hash(HASH_SHA1_mode,
                    (uint8_t *)DataInBuff,
                    in_len,
                    (uint32_t *)hal_hash_val);

        if(ret != 0)
        {
            HASH_TEST_PRINT("hash_hal_efficiency_test function[asr_hash return %d], test failed!\r\n", ret);
            goto exit;
        }

        tick4 = lega_rtos_get_time();

        if(memcmp(hash_val, hal_hash_val, 20) != 0)
        {
            mbed_dump_u8("hash value", hash_val, 20);
            mbed_dump_u8("hw hash value", hal_hash_val, 20);
            HASH_TEST_PRINT("data cmp not equal, hash_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

        HASH_TEST_PRINT("\r\n-%d %d %d %d--\r\n", tick1, tick2, tick3, tick4);
        HASH_TEST_PRINT("mbedtls_sha1 %d, hal_sha1 %d\r\n", tick2-tick1, tick4-tick3);
        HASH_TEST_PRINT("mbedtls_sha1 all %d K bytes time:---------> %f ms, ", k, (float)(tick2-tick1)/1.0);
        HASH_TEST_PRINT("mbedtls_sha1 1 K bytes time:---------> %f ms, ", (float)(tick2-tick1)/k/1.0);
        HASH_TEST_PRINT("hal_sha1 all %d K bytes time:---------> %f ms, ", k, (float)(tick4-tick3)/1.0);
        HASH_TEST_PRINT("hal_sha1 1 K bytes time:---------> %f ms, ", (float)(tick4-tick3)/k/1.0);
    }

    ret = 0;
    HASH_TEST_PRINT("hash_hal efficiency test complete\r\n");

exit:
    if(DataInBuff != NULL)
        lega_rtos_free(DataInBuff);

    return ret;
}

#endif /* MBEDTLS_HASH_TEST */

//************************************************************  AES  ***********************************************************************************
#ifdef MBEDTLS_AES_TEST

#include "asr_aes.h"
#include "mbedtls/aes.h"

#define AES_TEST_PRINT(...) printf(__VA_ARGS__)

#define AES_DATA_LEN    (1*1024)
#define AES_UINT_LEN    (1*1024)
#define AES_LOOP        (1*100)
#define AES_IV_LEN      (16)
#define AES128_KEY_LEN  (16)
#define AES256_KEY_LEN  (32)
#define AES_KEY_BIT_128 (128)
#define AES_KEY_BIT_256 (256)

int aes_hal_efficiency_test(void)
{
    int in_len = 0, ret = 0;
    int ret1 = 0, ret2 = 0;
    int tick1 = 0, tick2 = 0;
    int tick3 = 0, tick4 = 0;
    int tick5 = 0, tick6 = 0;

    mbedtls_aes_context ctx;
    mbedtls_aes_context ctx_256;

    unsigned char aes128_key[AES128_KEY_LEN] = {0x00, 0x01, 0x02, 0x03,
                                                0x04, 0x05, 0x06, 0x07,
                                                0x08, 0x09, 0x0a, 0x0b,
                                                0x0c, 0x0d, 0x0e, 0x0f};

    unsigned char aes_iv_buff[AES_IV_LEN] = {0x00, 0x01, 0x02, 0x03,
                                             0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0a, 0x0b,
                                             0x0c, 0x0d, 0x0e, 0x0f};

    unsigned char aes256_key[AES256_KEY_LEN] = {0x00};

    unsigned char aes_iv[AES_IV_LEN] = {0x00};

    unsigned char *DataInBuff = lega_rtos_malloc(AES_DATA_LEN);

    unsigned char *DataDecyptedBuff = lega_rtos_malloc(AES_DATA_LEN);

    unsigned char *DataAes256DecyptedBuff = lega_rtos_malloc(AES_DATA_LEN);

    unsigned char *aes_mbedtls_ResultBuff = lega_rtos_malloc(AES_DATA_LEN);

    unsigned char *aes256_mbedtls_ResultBuff = lega_rtos_malloc(AES_DATA_LEN);

    if((DataInBuff == NULL) || (DataDecyptedBuff == NULL) || (DataAes256DecyptedBuff == NULL) ||
        (aes_mbedtls_ResultBuff == NULL) || (aes256_mbedtls_ResultBuff == NULL))
    {
        ret = -1;
        AES_TEST_PRINT("aes_hal_efficiency_test malloc fail...\r\n");
        goto exit;
    }

    /*************************** function **********************************/

    AES_TEST_PRINT("aes_hal efficiency test start...\r\n");

    for(int k = 1; k <= AES_DATA_LEN/AES_UINT_LEN; k++)
    {
        in_len = k*AES_UINT_LEN;
        memset(DataInBuff, 0x55, AES_DATA_LEN);
        memset(aes256_key, 0x22, AES256_KEY_LEN);

        AES_TEST_PRINT("cnt[%d]\r\n", k);

        /*************************** aes step all **********************************/
        ret1 = 0;
        ret2 = 0;

        tick1 = lega_rtos_get_time();
        for(int l = 1; l <= AES_LOOP; l++)
        {
            memcpy(aes_iv, aes_iv_buff, AES_IV_LEN);
            mbedtls_aes_init(&ctx);
            ret1 = mbedtls_aes_setkey_enc(&ctx, aes128_key, AES_KEY_BIT_128);
            ret2 = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, in_len, aes_iv, DataInBuff, aes_mbedtls_ResultBuff);
            mbedtls_aes_free(&ctx);

            if((ret1) || (ret2))
                break;
        }
        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_setkey_enc return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_crypt_cbc return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

#ifndef MBEDTLS_AES_ALT

        ret1 = 0;
        ret2 = 0;

        tick5 = lega_rtos_get_time();
        for(int l = 1; l <= AES_LOOP; l++)
        {
            memcpy(aes_iv, aes_iv_buff, AES_IV_LEN);
            mbedtls_aes_init(&ctx_256);
            ret1 = mbedtls_aes_setkey_enc(&ctx_256, aes256_key, AES_KEY_BIT_256);
            ret2 = mbedtls_aes_crypt_cbc(&ctx_256, MBEDTLS_AES_ENCRYPT, in_len, aes_iv, DataInBuff, aes256_mbedtls_ResultBuff);
            mbedtls_aes_free(&ctx_256);

            if((ret1) || (ret2))
                break;
        }
        tick6 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_setkey_enc return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_crypt_cbc return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }
#endif

        AES_TEST_PRINT("\r\n-%d %d %d %d %d %d--\r\n", tick1, tick2, tick3, tick4, tick5, tick6);
        AES_TEST_PRINT("mbedtls_aes128 %d, mbedtls_aes256 %d\r\n", tick2-tick1, tick6-tick5);
        AES_TEST_PRINT("mbedtls_aes128 encrypt step all %d K bytes time:---------> %f ms, ", k, (float)(tick2-tick1)/AES_LOOP/1.0);
        AES_TEST_PRINT("mbedtls_aes128 encrypt step all 1 K bytes time:---------> %f ms, ", (float)(tick2-tick1)/AES_LOOP/k/1.0);
#ifndef MBEDTLS_AES_ALT
        AES_TEST_PRINT("mbedtls_aes256 encrypt step all %d K bytes time:---------> %f ms, ", k, (float)(tick6-tick5)/AES_LOOP/1.0);
        AES_TEST_PRINT("mbedtls_aes256 encrypt step all 1 K bytes time:---------> %f ms, ", (float)(tick6-tick5)/AES_LOOP/k/1.0);
#endif

        /*************************** aes step all **********************************/
        ret1 = 0;
        ret2 = 0;

        tick1 = lega_rtos_get_time();
        for(int l = 1; l <= AES_LOOP; l++)
        {
            memcpy(aes_iv, aes_iv_buff, AES_IV_LEN);
            mbedtls_aes_init(&ctx);
            ret1 = mbedtls_aes_setkey_dec(&ctx, aes128_key, AES_KEY_BIT_128);
            ret2 = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, in_len, aes_iv, aes_mbedtls_ResultBuff, DataDecyptedBuff);
            mbedtls_aes_free(&ctx);

            if((ret1) || (ret2))
                break;
        }
        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_setkey_dec return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_crypt_cbc return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }
        if(memcmp(DataDecyptedBuff, DataInBuff, in_len) != 0)
        {
            AES_TEST_PRINT("data cmp not equal, aes_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

#ifndef MBEDTLS_AES_ALT
        ret1 = 0;
        ret2 = 0;

        tick5 = lega_rtos_get_time();
        for(int l = 1; l <= AES_LOOP; l++)
        {
            memcpy(aes_iv, aes_iv_buff, AES_IV_LEN);
            mbedtls_aes_init(&ctx_256);
            ret1 = mbedtls_aes_setkey_dec(&ctx_256, aes256_key, AES_KEY_BIT_256);
            ret2 = mbedtls_aes_crypt_cbc(&ctx_256, MBEDTLS_AES_DECRYPT, in_len, aes_iv, aes256_mbedtls_ResultBuff, DataAes256DecyptedBuff);
            mbedtls_aes_free(&ctx_256);

            if((ret1) || (ret2))
                break;
        }
        tick6 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_setkey_dec return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            AES_TEST_PRINT("aes_hal_efficiency_test function[mbedtls_aes_crypt_cbc return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }
        if(memcmp(DataAes256DecyptedBuff, DataInBuff, in_len) != 0)
        {
            AES_TEST_PRINT("data cmp not equal, aes_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }
#endif

        AES_TEST_PRINT("\r\n-%d %d %d %d %d %d--\r\n", tick1, tick2, tick3, tick4, tick5, tick6);
        AES_TEST_PRINT("mbedtls_aes128 %d, mbedtls_aes256 %d\r\n", tick2-tick1, tick6-tick5);
        AES_TEST_PRINT("mbedtls_aes128 decrypt step all %d K bytes time:---------> %f ms, ", k, (float)(tick2-tick1)/AES_LOOP/1.0);
        AES_TEST_PRINT("mbedtls_aes128 decrypt step all 1 K bytes time:---------> %f ms, ", (float)(tick2-tick1)/AES_LOOP/k/1.0);
#ifndef MBEDTLS_AES_ALT
        AES_TEST_PRINT("mbedtls_aes256 decrypt step all %d K bytes time:---------> %f ms, ", k, (float)(tick6-tick5)/AES_LOOP/1.0);
        AES_TEST_PRINT("mbedtls_aes256 decrypt step all 1 K bytes time:---------> %f ms, ", (float)(tick6-tick5)/AES_LOOP/k/1.0);
#endif
    }

    ret = 0;
    AES_TEST_PRINT("aes_hal efficiency test complete\r\n");

exit:
    if(DataInBuff != NULL)
        lega_rtos_free(DataInBuff);

    if(DataDecyptedBuff != NULL)
        lega_rtos_free(DataDecyptedBuff);

    if(DataAes256DecyptedBuff != NULL)
        lega_rtos_free(DataAes256DecyptedBuff);

    if(aes_mbedtls_ResultBuff != NULL)
        lega_rtos_free(aes_mbedtls_ResultBuff);

    if(aes256_mbedtls_ResultBuff != NULL)
        lega_rtos_free(aes256_mbedtls_ResultBuff);

    return ret;
}

#endif /* MBEDTLS_AES_TEST */

//************************************************************  RSA  ***********************************************************************************

#ifdef MBEDTLS_RSA_TEST

#include "asr_rsa.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"

#define RSA_TEST_PRINT(...) printf(__VA_ARGS__)

#define RSA_DATA_LEN (1*1024)
#define RSA_UINT_LEN (1*1024)
#define RSA1024_ENCRYPT_DATA_OUT_MAX_LEN (RSA_DATA_LEN+1152)

#define SHA256_SUM_LEN (32)
#define TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT (11)
#define TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT (66) // 2 * hashLen + 2 = 2 * 32 + 2

#ifndef USE_SHA256
#define USE_SHA256 (0)
#endif

#define KEY_LEN_1024 (128)

#define RSA1024_N   "9292758453063D803DD603D5E777D788" \
                    "8ED1D5BF35786190FA2F23EBC0848AEA" \
                    "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                    "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                    "93A89813FBF3C4F8066D2D800F7C38A8" \
                    "1AE31942917403FF4946B0A83D3D3E05" \
                    "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                    "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA1024_E   "10001"

#define RSA1024_D   "24BF6185468786FDD303083D25E64EFC" \
                    "66CA472BC44D253102F8B4A9D3BFA750" \
                    "91386C0077937FE33FA3252D28855837" \
                    "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                    "DF79C5CE07EE72C7F123142198164234" \
                    "CABB724CF78B8173B9F880FC86322407" \
                    "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                    "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA1024_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                    "2C01CAD19EA484A87EA4377637E75500" \
                    "FCB2005C5C7DD6EC4AC023CDA285D796" \
                    "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA1024_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                    "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                    "910E4168387E3C30AA1E00C339A79508" \
                    "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA1024_DP  "C1ACF567564274FB07A0BBAD5D26E298" \
                    "3C94D22288ACD763FD8E5600ED4A702D" \
                    "F84198A5F06C2E72236AE490C93F07F8" \
                    "3CC559CD27BC2D1CA488811730BB5725"

#define RSA1024_DQ  "4959CBF6F8FEF750AEE6977C155579C7" \
                    "D8AAEA56749EA28623272E4F7D0592AF" \
                    "7C1F1313CAC9471B5C523BFE592F517B" \
                    "407A1BD76C164B93DA2D32A383E58357"

#define RSA1024_QP  "9AE7FBC99546432DF71896FC239EADAE" \
                    "F38D18D2B2F0E2DD275AA977E2BF4411" \
                    "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                    "A74206CEC169D74BF5A8C50D6F48EA08"

const unsigned char rsa1024_D[] =
{
    /*TST_PrivetExponent_D*/
    0x24,0xBF,0x61,0x85,0x46,0x87,0x86,0xFD,0xD3,0x03,0x08,0x3D,0x25,0xE6,0x4E,0xFC,
    0x66,0xCA,0x47,0x2B,0xC4,0x4D,0x25,0x31,0x02,0xF8,0xB4,0xA9,0xD3,0xBF,0xA7,0x50,
    0x91,0x38,0x6C,0x00,0x77,0x93,0x7F,0xE3,0x3F,0xA3,0x25,0x2D,0x28,0x85,0x58,0x37,
    0xAE,0x1B,0x48,0x4A,0x8A,0x9A,0x45,0xF7,0xEE,0x8C,0x0C,0x63,0x4F,0x99,0xE8,0xCD,
    0xDF,0x79,0xC5,0xCE,0x07,0xEE,0x72,0xC7,0xF1,0x23,0x14,0x21,0x98,0x16,0x42,0x34,
    0xCA,0xBB,0x72,0x4C,0xF7,0x8B,0x81,0x73,0xB9,0xF8,0x80,0xFC,0x86,0x32,0x24,0x07,
    0xAF,0x1F,0xED,0xFD,0xDE,0x2B,0xEB,0x67,0x4C,0xA1,0x5F,0x3E,0x81,0xA1,0x52,0x1E,
    0x07,0x15,0x13,0xA1,0xE8,0x5B,0x5D,0xFA,0x03,0x1F,0x21,0xEC,0xAE,0x91,0xA3,0x4D
};

const unsigned char rsa1024_E[] =
{
    /*TST_PublicExponent_E*/
    0x01,0x00,0x01
};

const unsigned char rsa1024_N[] =
{
    /*TST_Modulus_N*/
    0x92,0x92,0x75,0x84,0x53,0x06,0x3D,0x80,0x3D,0xD6,0x03,0xD5,0xE7,0x77,0xD7,0x88,
    0x8E,0xD1,0xD5,0xBF,0x35,0x78,0x61,0x90,0xFA,0x2F,0x23,0xEB,0xC0,0x84,0x8A,0xEA,
    0xDD,0xA9,0x2C,0xA6,0xC3,0xD8,0x0B,0x32,0xC4,0xD1,0x09,0xBE,0x0F,0x36,0xD6,0xAE,
    0x71,0x30,0xB9,0xCE,0xD7,0xAC,0xDF,0x54,0xCF,0xC7,0x55,0x5A,0xC1,0x4E,0xEB,0xAB,
    0x93,0xA8,0x98,0x13,0xFB,0xF3,0xC4,0xF8,0x06,0x6D,0x2D,0x80,0x0F,0x7C,0x38,0xA8,
    0x1A,0xE3,0x19,0x42,0x91,0x74,0x03,0xFF,0x49,0x46,0xB0,0xA8,0x3D,0x3D,0x3E,0x05,
    0xEE,0x57,0xC6,0xF5,0xF5,0x60,0x6F,0xB5,0xD4,0xBC,0x6C,0xD3,0x4E,0xE0,0x80,0x1A,
    0x5E,0x94,0xBB,0x77,0xB0,0x75,0x07,0x23,0x3A,0x0B,0xC7,0xBA,0xC8,0xF9,0x0F,0x79
};

const unsigned char rsa1024_P[] =
{
    /*TST_P*/
    0xC3,0x6D,0x0E,0xB7,0xFC,0xD2,0x85,0x22,0x3C,0xFB,0x5A,0xAB,0xA5,0xBD,0xA3,0xD8,
    0x2C,0x01,0xCA,0xD1,0x9E,0xA4,0x84,0xA8,0x7E,0xA4,0x37,0x76,0x37,0xE7,0x55,0x00,
    0xFC,0xB2,0x00,0x5C,0x5C,0x7D,0xD6,0xEC,0x4A,0xC0,0x23,0xCD,0xA2,0x85,0xD7,0x96,
    0xC3,0xD9,0xE7,0x5E,0x1E,0xFC,0x42,0x48,0x8B,0xB4,0xF1,0xD1,0x3A,0xC3,0x0A,0x57
};

const unsigned char rsa1024_Q[] =
{
    /*TST_Q*/
    0xC0,0x00,0xDF,0x51,0xA7,0xC7,0x7A,0xE8,0xD7,0xC7,0x37,0x0C,0x1F,0xF5,0x5B,0x69,
    0xE2,0x11,0xC2,0xB9,0xE5,0xDB,0x1E,0xD0,0xBF,0x61,0xD0,0xD9,0x89,0x96,0x20,0xF4,
    0x91,0x0E,0x41,0x68,0x38,0x7E,0x3C,0x30,0xAA,0x1E,0x00,0xC3,0x39,0xA7,0x95,0x08,
    0x84,0x52,0xDD,0x96,0xA9,0xA5,0xEA,0x5D,0x9D,0xCA,0x68,0xDA,0x63,0x60,0x32,0xAF
};

const unsigned char rsa1024_dP[] =
{
    /*TST_dP*/
    0xC1,0xAC,0xF5,0x67,0x56,0x42,0x74,0xFB,0x07,0xA0,0xBB,0xAD,0x5D,0x26,0xE2,0x98,
    0x3C,0x94,0xD2,0x22,0x88,0xAC,0xD7,0x63,0xFD,0x8E,0x56,0x00,0xED,0x4A,0x70,0x2D,
    0xF8,0x41,0x98,0xA5,0xF0,0x6C,0x2E,0x72,0x23,0x6A,0xE4,0x90,0xC9,0x3F,0x07,0xF8,
    0x3C,0xC5,0x59,0xCD,0x27,0xBC,0x2D,0x1C,0xA4,0x88,0x81,0x17,0x30,0xBB,0x57,0x25
};

const unsigned char rsa1024_dQ[] =
{
    /*TST_dQ*/
    0x49,0x59,0xCB,0xF6,0xF8,0xFE,0xF7,0x50,0xAE,0xE6,0x97,0x7C,0x15,0x55,0x79,0xC7,
    0xD8,0xAA,0xEA,0x56,0x74,0x9E,0xA2,0x86,0x23,0x27,0x2E,0x4F,0x7D,0x05,0x92,0xAF,
    0x7C,0x1F,0x13,0x13,0xCA,0xC9,0x47,0x1B,0x5C,0x52,0x3B,0xFE,0x59,0x2F,0x51,0x7B,
    0x40,0x7A,0x1B,0xD7,0x6C,0x16,0x4B,0x93,0xDA,0x2D,0x32,0xA3,0x83,0xE5,0x83,0x57
};

const unsigned char rsa1024_Qinv[] =
{
    /*TST_Qinv*/
    0x9A,0xE7,0xFB,0xC9,0x95,0x46,0x43,0x2D,0xF7,0x18,0x96,0xFC,0x23,0x9E,0xAD,0xAE,
    0xF3,0x8D,0x18,0xD2,0xB2,0xF0,0xE2,0xDD,0x27,0x5A,0xA9,0x77,0xE2,0xBF,0x44,0x11,
    0xF5,0xA3,0xB2,0xA5,0xD3,0x36,0x05,0xAE,0xBB,0xCC,0xBA,0x7F,0xEB,0x9F,0x2D,0x2F,
    0xA7,0x42,0x06,0xCE,0xC1,0x69,0xD7,0x4B,0xF5,0xA8,0xC5,0x0D,0x6F,0x48,0xEA,0x08
};

static int rsa_myrand( void *rng_state, unsigned char *output, size_t len )
{
    size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

    for( i = 0; i < len; ++i )
        output[i] = rand();

    return( 0 );
}

int rsa_hal_efficiency_test(int pcks_type)
{
    int in_len = 0, ret = 0;
    int ret1 = 0, ret2 = 0;
    int tick1 = 0, tick2 = 0;
    int tick3 = 0, tick4 = 0;
    int tick5 = 0, tick6 = 0;
    int tick7 = 0, tick8 = 0;
    int out_len = 0;

    mbedtls_rsa_context rsa;
    RSAUserPrivKey_t UserPrivKey;
    RSAUserPubKey_t UserPubKey;
    RSAPrimeData_t PrimeData;
    RSAPrivUserContext_t ContextPrivate;
    RSAPubUserContext_t ContextPub;
    uint8_t ucRsaPkcsType[2][10] = {"PKCS_V15","PKCS_V21"};

    unsigned char sha256sum[SHA256_SUM_LEN];
    int ActualDecDataSize = 0;
    int ActualSignatureSize = 0;

    int blockNum = 0;
    int blockRemain = 0;
    int cal_len = 0;

    unsigned char *DataInBuff = lega_rtos_malloc(RSA_DATA_LEN);

    unsigned char *rsa1024_DataDecryptedBuff = lega_rtos_malloc(RSA_DATA_LEN);

    unsigned char *rsa1024_SignBuff = lega_rtos_malloc(KEY_LEN_1024);

    unsigned char *rsa1024_mbedtls_ResultBuff = lega_rtos_malloc(RSA1024_ENCRYPT_DATA_OUT_MAX_LEN);

    unsigned char *rsa1024_hal_Buff = lega_rtos_malloc(RSA1024_ENCRYPT_DATA_OUT_MAX_LEN);

    unsigned char *rsa1024_hal_ResultBuff = lega_rtos_malloc(RSA1024_ENCRYPT_DATA_OUT_MAX_LEN);

    if((DataInBuff == NULL) || (rsa1024_DataDecryptedBuff == NULL) || (rsa1024_SignBuff == NULL) ||
        (rsa1024_mbedtls_ResultBuff == NULL) || (rsa1024_hal_Buff == NULL) || (rsa1024_hal_ResultBuff == NULL))
    {
        ret =-1;
        RSA_TEST_PRINT("rsa_hal_efficiency_test malloc fail...\r\n");
        goto exit;
    }

    if(pcks_type > 1)
    {
        ret =-1;
        RSA_TEST_PRINT("pcks_type error...\r\n");
        goto exit;
    }
    /*************************** function **********************************/

    RSA_TEST_PRINT("rsa_hal efficiency test start...\r\n");

    RSA_TEST_PRINT("rsa PKCS type is %s\r\n", ucRsaPkcsType[pcks_type]);

    for(int k = 1; k <= RSA_DATA_LEN/RSA_UINT_LEN; k++)
    {
        in_len = k*RSA_UINT_LEN;
        memset(DataInBuff, 0x55, k*RSA_UINT_LEN);

        RSA_TEST_PRINT("cnt[%d]\r\n", k);

        /***************************** rsa 1024 ************************************/
        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            blockNum = in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT);
            blockRemain = in_len%(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT);
        }
        else
        {
            blockNum = in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT);
            blockRemain = in_len%(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT);
        }

        if(blockRemain)
        {
            blockNum += 1;
        }
        RSA_TEST_PRINT("blockNum=%d, %d\r\n", blockNum, blockRemain);

        /*************************** rsa encrypt&decrypt step all **********************************/
        ret1 = 0;
        ret2 = 0;
        tick1 = lega_rtos_get_time();
        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }

        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        for(int m = 0; m < blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                cal_len = KEY_LEN_1024 - TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT));
                }
                ret2 = mbedtls_rsa_pkcs1_encrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PUBLIC, cal_len, DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)), rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)) );
            }
            else
            {
                cal_len = KEY_LEN_1024 - TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT));
                }
                ret2 = mbedtls_rsa_pkcs1_encrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PUBLIC, cal_len, DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)), rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)));
            }
            if(ret2)
                break;
        }
        mbedtls_rsa_free( &rsa );

        tick2 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_encrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret1 = 0;
        ret2 = 0;
        tick3 = lega_rtos_get_time();

        if(pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }

        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        out_len = 0;
        for(int m = 0; m < blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                ret2 = mbedtls_rsa_pkcs1_decrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, (size_t *)&ActualDecDataSize,
                        (const unsigned char *)rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)),
                        rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)), blockNum*KEY_LEN_1024 );
            }
            else
            {
                ret2 = mbedtls_rsa_pkcs1_decrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, (size_t *)&ActualDecDataSize,
                        (const unsigned char *)rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)),
                        rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)), blockNum*KEY_LEN_1024 );
            }

            if(ret2)
                break;

            out_len += ActualDecDataSize;
        }
        mbedtls_rsa_free( &rsa );

        tick4 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_decrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        if(out_len != in_len)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }
        if(memcmp(rsa1024_DataDecryptedBuff, DataInBuff, in_len) != 0)
        {
            RSA_TEST_PRINT("data cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick1, tick2, tick3, tick4);
        RSA_TEST_PRINT("mbedtls_rsa-1024 %d, mbedtls_rsa-1024 %d\r\n", tick2-tick1,  tick4-tick3);
        RSA_TEST_PRINT("mbedtls_rsa-1024 encrypt step all %d K bytes time:---------> %f ms,", k, (float)(tick2-tick1)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 encrypt step all 1 K bytes time:---------> %f ms,", (float)(tick2-tick1)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 decrypt step all %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 decrypt step all 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);

        ret1 = 0;
        ret2 = 0;
        tick5 = lega_rtos_get_time();

        ret1 = asr_rsa_build_pubkey(&UserPubKey,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_pubkey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        for(int m = 0; m < blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                cal_len = KEY_LEN_1024 - TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len - ((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT));
                }
                ret2 = asr_rsa_pkcs1v15_encrypt(rndContext_ptr,
                                                &UserPubKey,
                                                &PrimeData,
                                                DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)),
                                                cal_len,
                                                rsa1024_hal_Buff+(m*(KEY_LEN_1024)));
            }
            else
            {
                cal_len = KEY_LEN_1024 - TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len - ((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT));
                }
                ret2 = asr_rsa_oaep_encrypt(rndContext_ptr,
                                            &UserPubKey,
                                            &PrimeData,
                                            RSA_HASH_SHA256_mode,
                                            NULL,
                                            0,
                                            PKCS1_MGF1,
                                            DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)),
                                            cal_len,
                                            rsa1024_hal_Buff+(m*(KEY_LEN_1024)));
            }

            if(ret2 != 0)
            {
                RSA_TEST_PRINT("asr_rsa_encrypt return %x,%d test failed!\r\n", ret2, m);
				break;
            }
        }

        tick6 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_encrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret1 = 0;
        ret2 = 0;
        tick7 = lega_rtos_get_time();

        ret1 = asr_rsa_build_prikey(&UserPrivKey,
                        (uint8_t *)rsa1024_D,
                        KEY_LEN_1024,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_prikey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        out_len = 0;
        for(int m = 0; m< blockNum; m++)
        {
            ActualDecDataSize = KEY_LEN_1024*8;
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                ret2 = asr_rsa_pkcs1v15_decrypt(&UserPrivKey,
                                                &PrimeData,
                                                rsa1024_hal_Buff+(m*(KEY_LEN_1024)),
                                                KEY_LEN_1024,
                                                rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)),//Inplace operation
                                                (uint16_t *)&ActualDecDataSize);
            }
            else
            {
                ret2 = asr_rsa_oaep_decrypt(&UserPrivKey,
                                            &PrimeData,
                                            RSA_HASH_SHA256_mode,
                                            NULL,
                                            0,
                                            PKCS1_MGF1,
                                            rsa1024_hal_Buff+(m*(KEY_LEN_1024)),
                                            KEY_LEN_1024,
                                            rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)),//Inplace operation
                                            (uint16_t *)&ActualDecDataSize);
            }

            if(ret2 != 0)
            {
                RSA_TEST_PRINT("asr_rsa_decrypt return %x,%d test failed!\r\n", ret2, m);
                break;
            }

            out_len += ActualDecDataSize;
        }

        tick8 = lega_rtos_get_time();
        
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_decrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        if(out_len != in_len)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }
        if(memcmp(rsa1024_DataDecryptedBuff, DataInBuff, in_len) != 0)
        {
            RSA_TEST_PRINT("data cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("rsa_hal-1024 encrypt %d, rsa_hal-1024 decrypt %d\r\n", tick6-tick5,  tick8-tick7);
        RSA_TEST_PRINT("rsa_hal-1024 encrypt step all %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 encrypt step all 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 decrypt step all %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 decrypt step all 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        /*************************** rsa sign&verify step all **********************************/
        ret1 = 0;
        ret2 = 0;
        tick1 = lega_rtos_get_time();

        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }

        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_rsa_pkcs1_sign( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, SHA256_SUM_LEN, sha256sum, rsa1024_SignBuff );

        mbedtls_rsa_free( &rsa );

        tick2 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_signs return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret1 = 0;
        ret2 = 0;
        tick3 = lega_rtos_get_time();

        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }

        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, SHA256_SUM_LEN, sha256sum, rsa1024_SignBuff );

        mbedtls_rsa_free( &rsa );

        tick4 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_verify return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick1, tick2, tick3, tick4);
        RSA_TEST_PRINT("mbedtls_rsa-1024 sign %d, mbedtls_rsa-1024 verify %d\r\n", tick2-tick1,  tick4-tick3);
        RSA_TEST_PRINT("mbedtls_rsa-1024 sign step all %d K bytes time:---------> %f ms,", k, (float)(tick2-tick1)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 sign step all 1 K bytes time:---------> %f ms,", (float)(tick2-tick1)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 verify step all %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 verify step all 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);

        ret1 = 0;
        ret2 = 0;
        tick5 = lega_rtos_get_time();

        ret1 = asr_rsa_build_prikey(&UserPrivKey,
                        (uint8_t *)rsa1024_D,
                        KEY_LEN_1024,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_prikey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ActualSignatureSize = KEY_LEN_1024;
        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            ret2 = asr_rsa_pkcs1v15_sign(rndContext_ptr,
                            &ContextPrivate,
                            &UserPrivKey,
                            RSA_HASH_SHA256_mode,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff,
                            (uint16_t *)&ActualSignatureSize);
        }
        else
        {
            ret2 = asr_rsa_pss_sign(rndContext_ptr,
                            &ContextPrivate,
                            &UserPrivKey,
                            RSA_HASH_SHA256_mode,
                            PKCS1_MGF1,
                            0,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff,
                            (uint16_t *)&ActualSignatureSize);
        }

        tick6 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_sign return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }
        if(ActualSignatureSize != KEY_LEN_1024)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

        ret1 = 0;
        ret2 = 0;
        tick7 = lega_rtos_get_time();

        ret1 = asr_rsa_build_pubkey(&UserPubKey,
                            (uint8_t *)rsa1024_E,
                            sizeof(rsa1024_E),
                            (uint8_t *)rsa1024_N,
                            KEY_LEN_1024);

        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            ret2 = asr_rsa_pkcs1v15_verify(&ContextPub,
                            &UserPubKey,
                            RSA_HASH_SHA256_mode,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff);
        }
        else
        {
            ret2 = asr_rsa_pss_verify(&ContextPub,
                            &UserPubKey,
                            RSA_HASH_SHA256_mode,
                            PKCS1_MGF1,
                            0,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff);
        }

        tick8 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_pubkey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_verify return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("hal_rsa-1024 vsign %d, hal_rsa-1024 verify %d\r\n", tick6-tick5,  tick8-tick7);
        RSA_TEST_PRINT("hal_rsa-1024 sign step all %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 sign step all 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 verify step all %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 verify step all 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        /*************************** rsa encrypt&decrypt step by step **********************************/
        ret1 = 0;
        tick1 = lega_rtos_get_time();
        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }
        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick3 = lega_rtos_get_time();

        for(int m = 0; m< blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                cal_len = KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT;
                if((m== (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT));
                }
                ret2 = mbedtls_rsa_pkcs1_encrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PUBLIC, cal_len, DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)), rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)) );
            }
            else
            {
                cal_len = KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT;
                if((m== (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT));
                }
                ret2 = mbedtls_rsa_pkcs1_encrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PUBLIC, cal_len, DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)), rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)) );
            }

            if(ret2)
                break;
        }

        tick4 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_encrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret1 = 0;
        tick5 = lega_rtos_get_time();

        out_len = 0;
        for(int m = 0; m< blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                ret1 = mbedtls_rsa_pkcs1_decrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, (size_t *)&ActualDecDataSize,
                        (const unsigned char *)rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)),
                        rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)), blockNum*KEY_LEN_1024 );
                out_len += ActualDecDataSize;
            }
            else
            {
                ret1 = mbedtls_rsa_pkcs1_decrypt( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, (size_t *)&ActualDecDataSize,
                        (const unsigned char *)rsa1024_mbedtls_ResultBuff+(m*(KEY_LEN_1024)),
                        rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)), blockNum*KEY_LEN_1024 );
                out_len += ActualDecDataSize;
            }

            if(ret1)
                break;
        }

        tick6 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_decrypt return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        tick7 = lega_rtos_get_time();

        mbedtls_rsa_free( &rsa );

        tick8 = lega_rtos_get_time();

        if(out_len != in_len)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }
        if(memcmp(rsa1024_DataDecryptedBuff, DataInBuff, in_len) != 0)
        {
            RSA_TEST_PRINT("data cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d %d %d %d %d-\r\n", tick1, tick2, tick3, tick4, tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("mbedtls_rsa-1024 init %d, mbedtls_rsa-1024 encrypt %d, mbedtls_rsa-1024 decrypt %d, mbedtls_rsa-1024 finalize %d\r\n", tick2-tick1,  tick4-tick3, tick6-tick5,  tick8-tick7);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step init time:---------> %f ms,", (float)(tick2-tick1)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step encrypt %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step encrypt 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step decrypt %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step decrypt 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step finalize %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step finalize 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        ret1 = 0;
        tick1 = lega_rtos_get_time();

        ret1 = asr_rsa_build_pubkey(&UserPubKey,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_pubkey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret1 = 0;
        tick3 = lega_rtos_get_time();

        ret1 = asr_rsa_build_prikey(&UserPrivKey,
                        (uint8_t *)rsa1024_D,
                        KEY_LEN_1024,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        tick4 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_prikey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick5 = lega_rtos_get_time();

        for(int m = 0; m< blockNum; m++)
        {
            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                cal_len = KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT));
                }
                ret2 = asr_rsa_pkcs1v15_encrypt(rndContext_ptr,
                                &UserPubKey,
                                &PrimeData,
                                DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)),
                                cal_len,
                                rsa1024_hal_ResultBuff+(m*(KEY_LEN_1024)));
            }
            else
            {
                cal_len = KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT;
                if((m == (blockNum-1))&&(blockRemain))
                {
                    cal_len = in_len-((in_len/(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT))*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT));
                }
                ret2 = asr_rsa_oaep_encrypt(rndContext_ptr,
                                &UserPubKey,
                                &PrimeData,
                                RSA_HASH_SHA256_mode,
                                NULL,
                                0,
                                PKCS1_MGF1,
                                DataInBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)),
                                cal_len,
                                rsa1024_hal_ResultBuff+(m*(KEY_LEN_1024)));
            }
            if(ret2)
                break;
        }

        tick6 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_encrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret2 = 0;
        tick7 = lega_rtos_get_time();

        out_len = 0;
        for(int m = 0; m< blockNum; m++)
        {
            ActualDecDataSize = KEY_LEN_1024*8;

            if(pcks_type == MBEDTLS_RSA_PKCS_V15)
            {
                ret2 = asr_rsa_pkcs1v15_decrypt(&UserPrivKey,
                                &PrimeData,
                                rsa1024_hal_ResultBuff+(m*(KEY_LEN_1024)),
                                KEY_LEN_1024,
                                rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT)),//Inplace operation
                                (uint16_t *)&ActualDecDataSize);
            }
            else
            {
                ret2 = asr_rsa_oaep_decrypt(&UserPrivKey,
                                &PrimeData,
                                RSA_HASH_SHA256_mode,
                                NULL,
                                0,
                                PKCS1_MGF1,
                                rsa1024_hal_ResultBuff+(m*(KEY_LEN_1024)),
                                KEY_LEN_1024,
                                rsa1024_DataDecryptedBuff+(m*(KEY_LEN_1024-TST_LESS_DATA_FOR_PKCS1V21_ENCRYPT)),//Inplace operation
                                (uint16_t *)&ActualDecDataSize);
            }


            if(ret2 != 0)
            {
                RSA_TEST_PRINT("asr_rsa_pkcs1v15_decrypt return %x,%d test failed!\r\n", ret2, m);
                break;
            }

            out_len += ActualDecDataSize;
        }

        tick8 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_decrypt return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        if(out_len != in_len)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }
        if(memcmp(rsa1024_DataDecryptedBuff, DataInBuff, in_len) != 0)
        {
            RSA_TEST_PRINT("data cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d %d %d %d %d-\r\n", tick1, tick2, tick3, tick4, tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("rsa_hal-1024 init %d, rsa_hal-1024 encrypt %d, rsa_hal-1024 decrypt %d\r\n", tick4-tick1,  tick6-tick5, tick8-tick7);
        RSA_TEST_PRINT("rsa_hal-1024 step init time:---------> %f ms,", (float)(tick4-tick1)/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 step encrypt %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 step encrypt 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 step decrypt %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("rsa_hal-1024 step decrypt 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        /*************************** rsa sign&verify step by step **********************************/
        ret1 = 0;
        tick1 = lega_rtos_get_time();

        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
        }
        else
        {
            mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        }

        rsa.len = KEY_LEN_1024;
        ret1 += mbedtls_mpi_read_string( &rsa.N , 16, RSA1024_N  );
        ret1 += mbedtls_mpi_read_string( &rsa.E , 16, RSA1024_E  );
        ret1 += mbedtls_mpi_read_string( &rsa.D , 16, RSA1024_D  );
        ret1 += mbedtls_mpi_read_string( &rsa.P , 16, RSA1024_P  );
        ret1 += mbedtls_mpi_read_string( &rsa.Q , 16, RSA1024_Q  );
        ret1 += mbedtls_mpi_read_string( &rsa.DP, 16, RSA1024_DP );
        ret1 += mbedtls_mpi_read_string( &rsa.DQ, 16, RSA1024_DQ );
        ret1 += mbedtls_mpi_read_string( &rsa.QP, 16, RSA1024_QP );

        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_mpi_read_string return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick3 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_rsa_pkcs1_sign( &rsa, rsa_myrand, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, SHA256_SUM_LEN, sha256sum, rsa1024_SignBuff );

        tick4 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_sign return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret2 = 0;
        tick5 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, SHA256_SUM_LEN, sha256sum, rsa1024_SignBuff );

        tick6 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[mbedtls_rsa_pkcs1_verify return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        tick7 = lega_rtos_get_time();

        mbedtls_rsa_free( &rsa );

        tick8 = lega_rtos_get_time();

        RSA_TEST_PRINT("\r\n-%d %d %d %d %d %d %d %d-\r\n", tick1, tick2, tick3, tick4, tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("mbedtls_rsa-1024 init %d, mbedtls_rsa-1024 sign %d, mbedtls_rsa-1024 verify %d, mbedtls_rsa-1024 finalize %d\r\n", tick2-tick1,  tick4-tick3, tick6-tick5,  tick8-tick7);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step init time:---------> %f ms,", (float)(tick1-tick1)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step sign %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step sign 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step verify %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step verify 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step finalize %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("mbedtls_rsa-1024 step finalize 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        ret1 = 0;

        tick1 = lega_rtos_get_time();

        ret1 = asr_rsa_build_prikey(&UserPrivKey,
                        (uint8_t *)rsa1024_D,
                        KEY_LEN_1024,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_prikey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret1 = 0;
        tick3 = lega_rtos_get_time();

        ret1 = asr_rsa_build_pubkey(&UserPubKey,
                        (uint8_t *)rsa1024_E,
                        sizeof(rsa1024_E),
                        (uint8_t *)rsa1024_N,
                        KEY_LEN_1024);

        tick4 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_build_pubkey return %#x], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick5 = lega_rtos_get_time();

        ActualSignatureSize = KEY_LEN_1024;
        if (pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            ret2 = asr_rsa_pkcs1v15_sign(rndContext_ptr,
                            &ContextPrivate,
                            &UserPrivKey,
                            RSA_HASH_SHA256_mode,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff,
                            (uint16_t *)&ActualSignatureSize);
        }
        else
        {
            ret2 = asr_rsa_pss_sign(rndContext_ptr,
                            &ContextPrivate,
                            &UserPrivKey,
                            RSA_HASH_SHA256_mode,
                            PKCS1_MGF1,
                            0,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff,
                            (uint16_t *)&ActualSignatureSize);
        }
        tick6 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_sign return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }
        if(ActualSignatureSize != KEY_LEN_1024)
        {
            RSA_TEST_PRINT("data out length cmp not equal, rsa_hal_efficiency_test failed!\r\n");
            ret = -1;
            goto exit;
        }

        ret2 = 0;
        tick7 = lega_rtos_get_time();

        if(pcks_type == MBEDTLS_RSA_PKCS_V15)
        {
            ret2 = asr_rsa_pkcs1v15_verify(&ContextPub,
                            &UserPubKey,
                            RSA_HASH_SHA256_mode,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff);
        }
        else
        {
            ret2 = asr_rsa_pss_verify(&ContextPub,
                            &UserPubKey,
                            RSA_HASH_SHA256_mode,
                            PKCS1_MGF1,
                            0,
                            DataInBuff,
                            in_len,
                            rsa1024_SignBuff);
        }

        tick8 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            RSA_TEST_PRINT("rsa_hal_efficiency_test function[asr_rsa_pkcs1v15_verify return %#x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        RSA_TEST_PRINT("\r\n-%d %d %d %d %d %d %d %d-\r\n", tick1, tick2, tick3, tick4, tick5, tick6, tick7, tick8);
        RSA_TEST_PRINT("hal_rsa-1024 init %d, hal_rsa-1024 sign %d, hal_rsa-1024 verify %d\r\n", tick4-tick1,  tick6-tick5, tick8-tick7);
        RSA_TEST_PRINT("hal_rsa-1024 step init time:---------> %f ms,", (float)(tick4-tick1)/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 step sign %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 step sign 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 step verify %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        RSA_TEST_PRINT("hal_rsa-1024 step verify 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);
    }

    ret = 0;
    RSA_TEST_PRINT("\r\nrsa_hal efficiency test complete.\r\n");

exit:
    if(DataInBuff != NULL)
        lega_rtos_free(DataInBuff);

    if(rsa1024_DataDecryptedBuff != NULL)
        lega_rtos_free(rsa1024_DataDecryptedBuff);

    if(rsa1024_SignBuff != NULL)
        lega_rtos_free(rsa1024_SignBuff);

    if(rsa1024_mbedtls_ResultBuff != NULL)
        lega_rtos_free(rsa1024_mbedtls_ResultBuff);

    if(rsa1024_hal_Buff != NULL)
        lega_rtos_free(rsa1024_hal_Buff);

    if(rsa1024_hal_ResultBuff != NULL)
        lega_rtos_free(rsa1024_hal_ResultBuff);

    return ret;
}

#endif /* MBEDTLS_RSA_TEST */

//************************************************************  ECC ***********************************************************************************

#ifdef MBEDTLS_ECC_TEST

#include "asr_ecc.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecdh.h"

#define ECC_TEST_PRINT(...) printf(__VA_ARGS__)

#define ECC_DATA_LEN (2*1024)
#define ECC_UINT_LEN (1*1024)

#define SHARED_SECRET_MAX_LENGHT 250

#define SHA256_SUM_LEN (32)
#define TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT (11)

#ifndef USE_SHA256
#define USE_SHA256 (0)
#endif

#define SIG_BUF_LEN (MBEDTLS_ECDSA_MAX_LEN)

static int ecc_myrand( void *rng_state, unsigned char *output, size_t len )
{
    size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

    for( i = 0; i < len; ++i )
        output[i] = rand();

    return( 0 );
}

int ecc_hal_efficiency_test(void)
{
    int in_len = 0, ret = 0;
    int ret1 = 0, ret2 = 0;
    int tick1 = 0, tick2 = 0;
    int tick3 = 0, tick4 = 0;
    int tick5 = 0, tick6 = 0;
    int tick7 = 0, tick8 = 0;

    mbedtls_ecdsa_context ecdsa;

    ECPKI_UserPrivKey_t UserPrivKey;
    ECPKI_UserPublKey_t UserPubKey;
    ECDSA_SignUserContext_t SignUserContext;
    ECDSA_VerifyUserContext_t VerifyUserContext;
    ECPKI_KG_TempData_t TempECCKGBuff;
    ECPKI_KG_FipsContext_t FipsBuff;
    const ECPKI_Domain_t *pDomain;

    pDomain = asr_ecpki_getEcDomain(ECPKI_DomainID_secp256r1);

    unsigned char sha256sum[SHA256_SUM_LEN];
    int ActualSignatureSize = 0;

    unsigned char *DataInBuff = lega_rtos_malloc(ECC_DATA_LEN);

    int Ecc_SignBuff_len = SIG_BUF_LEN;
    unsigned char *Ecc_SignBuff = lega_rtos_malloc(SIG_BUF_LEN);

    if((DataInBuff == NULL) || (Ecc_SignBuff == NULL))
    {
        ret =-1;
        ECC_TEST_PRINT("ecc_hal_efficiency_test malloc fail...\r\n");
        goto exit;
    }

    /*************************** function **********************************/

    ECC_TEST_PRINT("ecc_hal efficiency test start...\r\n");

    for(int k = 1; k <= ECC_DATA_LEN/ECC_UINT_LEN; k++)
    {
        in_len = k*ECC_UINT_LEN;
        memset(DataInBuff, 0x55, k*ECC_UINT_LEN);

        ECC_TEST_PRINT("cnt[%d]\r\n", k);

        /*************************** ecc sign&verify step all **********************************/
        ret1 = 0;
        ret2 = 0;

        mbedtls_ecdsa_init(&ecdsa);

        ret1 = mbedtls_ecdsa_genkey( &ecdsa, MBEDTLS_ECP_DP_SECP256R1, ecc_myrand, NULL);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_ecdsa_genkey return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ECC_TEST_PRINT("ecc_hal_efficiency_test mbedtls_ecdsa_init ok %d \n", ecdsa.grp.id);

        tick1 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_ecdsa_write_signature( &ecdsa, MBEDTLS_MD_SHA256, sha256sum, SHA256_SUM_LEN, Ecc_SignBuff, (size_t *)&Ecc_SignBuff_len, ecc_myrand, NULL);

        tick2 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_ecdsa_write_signature return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret2 = 0;
        tick3 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_ecdsa_read_signature( &ecdsa, sha256sum, SHA256_SUM_LEN, Ecc_SignBuff, Ecc_SignBuff_len );

        mbedtls_ecdsa_free( &ecdsa );

        tick4 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_ecdsa_read_signature return %x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ECC_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick1, tick2, tick3, tick4);
        ECC_TEST_PRINT("mbedtls_eccdsa sign %d, mbedtls_eccdsa verify %d\r\n", tick2-tick1,  tick4-tick3);
        ECC_TEST_PRINT("mbedtls_eccdsa sign step all %d K bytes time:---------> %f ms,", k, (float)(tick2-tick1)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa sign step all 1 K bytes time:---------> %f ms,", (float)(tick2-tick1)/k/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa verify step all %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa verify step all 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);

        ret1 = 0;
        ret2 = 0;

        ret1 = asr_ecpki_genkeypair(rndContext_ptr,
                        pDomain,
                        &UserPrivKey,
                        &UserPubKey,
                        &TempECCKGBuff,
                        &FipsBuff);

        tick5 = lega_rtos_get_time();

        ActualSignatureSize = SIG_BUF_LEN;

        ret2 = asr_ecdsa_sign(rndContext_ptr,
                        &SignUserContext,
                        &UserPrivKey,
                        ECPKI_HASH_SHA256_mode,
                        DataInBuff,
                        in_len,
                        Ecc_SignBuff,
                        (uint32_t *)&ActualSignatureSize);

        tick6 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecpki_genkeypair return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }
        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecdsa_sign return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret1 = 0;
        ret2 = 0;
        tick7 = lega_rtos_get_time();

        ret2 = asr_ecdsa_verify(&VerifyUserContext,
                        &UserPubKey,
                        ECPKI_HASH_SHA256_mode,
                        Ecc_SignBuff,
                        ActualSignatureSize,
                        DataInBuff,
                        in_len);

        tick8 = lega_rtos_get_time();

        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecdsa_verify return %x], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ECC_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick5, tick6, tick7, tick8);
        ECC_TEST_PRINT("hal_ecdsa sign %d, hal_ecdsa verify %d\r\n", tick6-tick5,  tick8-tick7);
        ECC_TEST_PRINT("hal_ecdsa sign step all %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        ECC_TEST_PRINT("hal_ecdsa sign step all 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        ECC_TEST_PRINT("hal_ecdsa verify step all %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        ECC_TEST_PRINT("hal_ecdsa verify step all 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        /*************************** ecdsa sign&verify step by step **********************************/
        ret1 = 0;
        tick1 = lega_rtos_get_time();

        mbedtls_ecdsa_init(&ecdsa);

        ret1 = mbedtls_ecdsa_genkey( &ecdsa, MBEDTLS_ECP_DP_SECP256R1, ecc_myrand, NULL);

        tick2 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_ecdsa_genkey return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick3 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_ecdsa_write_signature( &ecdsa, MBEDTLS_MD_SHA256, sha256sum, SHA256_SUM_LEN, Ecc_SignBuff, (size_t *)&Ecc_SignBuff_len, ecc_myrand, NULL);

        tick4 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_hw_ecdsa_write_signature return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret2 = 0;
        tick5 = lega_rtos_get_time();

        mbedtls_sha256(DataInBuff, in_len, sha256sum, USE_SHA256);

        ret2 = mbedtls_ecdsa_read_signature( &ecdsa, sha256sum, SHA256_SUM_LEN, Ecc_SignBuff, Ecc_SignBuff_len );

        tick6 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[mbedtls_hw_ecdsa_read_signature return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        tick7 = lega_rtos_get_time();

        mbedtls_ecdsa_free( &ecdsa );

        tick8 = lega_rtos_get_time();

        ECC_TEST_PRINT("\r\n-%d %d %d %d %d %d %d %d-\r\n", tick1, tick2, tick3, tick4, tick5, tick6, tick7, tick8);
        ECC_TEST_PRINT("mbedtls_eccdsa init %d, mbedtls_eccdsa sign %d, mbedtls_eccdsa verify %d, mbedtls_eccdsa finalize %d\r\n", tick2-tick1,  tick4-tick3, tick6-tick5,  tick8-tick7);
        ECC_TEST_PRINT("mbedtls_eccdsa step init time:---------> %f ms,", (float)(tick1-tick1)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step sign %d K bytes time:---------> %f ms,", k, (float)(tick4-tick3)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step sign 1 K bytes time:---------> %f ms,", (float)(tick4-tick3)/k/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step verify %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step verify 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step finalize %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        ECC_TEST_PRINT("mbedtls_eccdsa step finalize 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);

        ret1 = 0;

        tick1 = lega_rtos_get_time();

        ret1 = asr_ecpki_genkeypair(rndContext_ptr,
                        pDomain,
                        &UserPrivKey,
                        &UserPubKey,
                        &TempECCKGBuff,
                        &FipsBuff);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecpki_genkeypair return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        tick4 = lega_rtos_get_time();
        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecpki_build_pubkey_fullcheck return %d], test failed!\r\n", ret1);
            ret = ret1;
            goto exit;
        }

        ret2 = 0;
        tick5 = lega_rtos_get_time();

        ActualSignatureSize = SIG_BUF_LEN;

        ret2 = asr_ecdsa_sign(rndContext_ptr,
                        &SignUserContext,
                        &UserPrivKey,
                        ECPKI_HASH_SHA256_mode,
                        DataInBuff,
                        in_len,
                        Ecc_SignBuff,
                        (uint32_t *)&ActualSignatureSize);

        tick6 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecdsa_sign return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ret2 = 0;
        tick7 = lega_rtos_get_time();

        ret2 = asr_ecdsa_verify(&VerifyUserContext,
                        &UserPubKey,
                        ECPKI_HASH_SHA256_mode,
                        Ecc_SignBuff,
                        ActualSignatureSize,
                        DataInBuff,
                        in_len);

        tick8 = lega_rtos_get_time();
        if(ret2 != 0)
        {
            ECC_TEST_PRINT("ecc_hal_efficiency_test function[asr_ecdsa_verify return %d], test failed!\r\n", ret2);
            ret = ret2;
            goto exit;
        }

        ECC_TEST_PRINT("\r\n-%d %d %d %d %d %d-\r\n", tick1, tick4, tick5, tick6, tick7, tick8);
        ECC_TEST_PRINT("hal_ecdsa init %d, hal_ecdsa sign %d, hal_ecdsa verify %d\r\n", tick4-tick1,  tick6-tick5, tick8-tick7);
        ECC_TEST_PRINT("hal_ecdsa step init time:---------> %f ms,", (float)(tick4-tick1)/1.0);
        ECC_TEST_PRINT("hal_ecdsa step sign %d K bytes time:---------> %f ms,", k, (float)(tick6-tick5)/1.0);
        ECC_TEST_PRINT("hal_ecdsa step sign 1 K bytes time:---------> %f ms,", (float)(tick6-tick5)/k/1.0);
        ECC_TEST_PRINT("hal_ecdsa step verify %d K bytes time:---------> %f ms,", k, (float)(tick8-tick7)/1.0);
        ECC_TEST_PRINT("hal_ecdsa step verify 1 K bytes time:---------> %f ms,", (float)(tick8-tick7)/k/1.0);
    }
    ret = 0;
    ECC_TEST_PRINT("\r\necc_hal efficiency test complete.\r\n");

exit:
    if(DataInBuff != NULL)
        lega_rtos_free(DataInBuff);

    if(Ecc_SignBuff != NULL)
        lega_rtos_free(Ecc_SignBuff);
    return ret;
}

int ecdh_hal_efficiency_test(void)
{
    int ret = 0, ret1 = 0;
    int tick1 = 0, tick4 = 0;
    int tick5 = 0, tick6 = 0;

    ECPKI_UserPrivKey_t UserPrivKey1;
    ECPKI_UserPublKey_t UserPublKey1;
    ECPKI_UserPrivKey_t UserPrivKey2;
    ECPKI_UserPublKey_t UserPublKey2;

    ECDH_TempData_t TempDHBuffptr;
    ECPKI_KG_TempData_t TempECCKGBuffptr;
    ECPKI_KG_FipsContext_t FipsBuff;
    uint8_t sharedSecret1ptr[SHARED_SECRET_MAX_LENGHT];
    uint8_t sharedSecret2ptr[SHARED_SECRET_MAX_LENGHT];
    const ECPKI_Domain_t *pDomain;

    uint32_t sharedSecret1Size = SHARED_SECRET_MAX_LENGHT;
    uint32_t sharedSecret2Size = SHARED_SECRET_MAX_LENGHT;

    pDomain = asr_ecpki_getEcDomain(ECPKI_DomainID_secp256r1);

    /*************************** function **********************************/

    ECC_TEST_PRINT("ecdh_hal efficiency test start...\r\n");

    {
        tick1 = lega_rtos_get_time();

        ret1 = asr_ecpki_genkeypair(rndContext_ptr,
                        pDomain,
                        &UserPrivKey1,
                        &UserPublKey1,
                        &TempECCKGBuffptr,
                        &FipsBuff);

        if(ret1 != ASR_SILIB_RET_OK)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test asr_ecpki_genkeypair for key pair 1 failed with 0x%x \n",ret1);
            return ret;
        }

        /*Generate second key pair*/
        ret1 = asr_ecpki_genkeypair(rndContext_ptr,
                        pDomain,
                        &UserPrivKey2,
                        &UserPublKey2,
                        &TempECCKGBuffptr,
                        &FipsBuff);

        if(ret1 != ASR_SILIB_RET_OK)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test asr_ecpki_genkeypair key pair 2 failed with 0x%x \n",ret1);
            return ret;
        }

        tick4 = lega_rtos_get_time();

        tick5 = lega_rtos_get_time();

        /* Generating the Secret for user 1*/
        /*---------------------------------*/
        ret1 = asr_ecdh_svdp_dh(&UserPublKey2,
                        &UserPrivKey1,
                        sharedSecret1ptr,
                        &sharedSecret1Size,
                        &TempDHBuffptr);

        if(ret1 != ASR_SILIB_RET_OK)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test asr_ecdh_svdp_dh for secret 1 failed with 0x%x \n",ret1);
            return ret;
        }

        /* Generating the Secret for user 2*/
        /*---------------------------------*/
        ret1 = asr_ecdh_svdp_dh(&UserPublKey1,
                        &UserPrivKey2,
                        sharedSecret2ptr,
                        &sharedSecret2Size,
                        &TempDHBuffptr);

        if(ret1 != ASR_SILIB_RET_OK)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test asr_ecdh_svdp_dh for secret 2 failed with 0x%x \n",ret1);
            return ret;
        }

        tick6 = lega_rtos_get_time();

        /* Verifying we have the same Secret Key both for user1 & user2   */
        /*----------------------------------------------------------------*/
        ret1 = memcmp(sharedSecret1ptr, sharedSecret2ptr, sharedSecret2Size);

        if(ret1 != ASR_SILIB_RET_OK)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test memcmp of shared secrets failed");
            return ret;
        }

        ECC_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick1, tick4, tick5, tick6);
        ECC_TEST_PRINT("hal_ecdh init %d, hal_ecdh generate secret %d\r\n", tick4-tick1,  tick6-tick5);
        ECC_TEST_PRINT("hal_ecdh step init time:---------> %f ms,", (float)(tick4-tick1)/1.0);
        ECC_TEST_PRINT("hal_ecdh user generate secret:---------> %f ms,", (float)(tick6-tick5)/1.0);

        /*************************** ecc sign&verify step all **********************************/
        ret1 = 0;

        mbedtls_ecp_group grp;
        mbedtls_mpi cli_secret, srv_secret;
        mbedtls_mpi cli_pri, srv_pri;
        mbedtls_ecp_point cli_pub, srv_pub;

        tick1 = lega_rtos_get_time();

        mbedtls_mpi_init(&cli_pri);
        mbedtls_mpi_init(&srv_pri);
        mbedtls_mpi_init(&cli_secret);
        mbedtls_mpi_init(&srv_secret);
        mbedtls_ecp_group_init(&grp);
        mbedtls_ecp_point_init(&cli_pub);
        mbedtls_ecp_point_init(&srv_pub);

        mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);

        ret1 = mbedtls_ecdh_gen_public(&grp,
                        &cli_pri,
                        &cli_pub,
                        ecc_myrand,
                        NULL);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test mbedtls_ecdh_gen_public failed with 0x%x \n",ret1);
            return ret;
        }

        ret1 = mbedtls_ecdh_gen_public(&grp,
                        &srv_pri,
                        &srv_pub,
                        ecc_myrand,
                        NULL);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test mbedtls_ecdh_gen_public failed with 0x%x \n",ret1);
            return ret;
        }

        tick4 = lega_rtos_get_time();

        tick5 = lega_rtos_get_time();

        ret1 = mbedtls_ecdh_compute_shared(&grp,
                        &cli_secret,
                        &srv_pub,
                        &cli_pri,
                        ecc_myrand,
                        NULL);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test mbedtls_ecdh_compute_shared failed with 0x%x \n",ret1);
            return ret;
        }

        ret1 = mbedtls_ecdh_compute_shared(&grp,
                        &srv_secret,
                        &cli_pub,
                        &srv_pri,
                        ecc_myrand,
                        NULL);

        if(ret1 != 0)
        {
            ECC_TEST_PRINT("ecdh_hal_efficiency_test mbedtls_ecdh_compute_shared failed with 0x%x \n",ret1);
            return ret;
        }

        tick6 = lega_rtos_get_time();

        ret = mbedtls_mpi_cmp_mpi(&cli_secret, &srv_secret);

        if(ret == 0)
        {
            ECC_TEST_PRINT("mbedtls_ecdh ok");
        }

        mbedtls_mpi_free(&cli_pri);
        mbedtls_mpi_free(&srv_pri);
        mbedtls_mpi_free(&cli_secret);
        mbedtls_mpi_free(&srv_secret);
        mbedtls_ecp_group_free(&grp);
        mbedtls_ecp_point_free(&cli_pub);
        mbedtls_ecp_point_free(&srv_pub);

        ECC_TEST_PRINT("\r\n-%d %d %d %d-\r\n", tick1, tick4, tick5, tick6);
        ECC_TEST_PRINT("mbedtls_ecdh init %d, mbedtls_ecdh generate secret %d\r\n", tick4-tick1,  tick6-tick5);
        ECC_TEST_PRINT("mbedtls_ecdh step init time:---------> %f ms,", (float)(tick4-tick1)/1.0);
        ECC_TEST_PRINT("mbedtls_ecdh user generate secret:---------> %f ms,", (float)(tick6-tick5)/1.0);
    }
    ret = 0;
    ECC_TEST_PRINT("\r\necc_hal efficiency test complete.\r\n");

    return ret;
}

#endif /* MBEDTLS_ECC_TEST */
