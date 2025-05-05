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

#ifdef CLOUD_TEST

#ifdef CLOUD_AWS_TEST
#include "cloud_common.h"
extern int8_t aws_connected_subscribe_ready;
static int aws_test_cnt = 0;
extern int aws_reconnect_cnt;
void aws_test(void)
{
    if (!aws_connected_subscribe_ready) return;

    if (semaphore_flag != SEMAPHORE_FLAG_NONE) return;

    if (light_flag == LIGHT_OFF)
        light_flag = LIGHT_ON;
    else
        light_flag = LIGHT_OFF;

    printf("----------------aws test cnt %d <%d>------------------\n",aws_test_cnt++,aws_reconnect_cnt);
    light_control();
}
#endif
#ifdef CLOUD_TENCENT_TEST
#include "cloud_common.h"
extern int8_t tencent_connected_subscribe_ready;
static int tencent_test_cnt = 0;
extern int tencent_reconnect_cnt;
void tencent_test(void)
{
    if (!tencent_connected_subscribe_ready) return;

    if (semaphore_flag != SEMAPHORE_FLAG_NONE) return;

    if (light_flag == LIGHT_OFF)
        light_flag = LIGHT_ON;
    else
        light_flag = LIGHT_OFF;

    printf("----------------tencent test cnt %d <%d>------------------\n",tencent_test_cnt++,tencent_reconnect_cnt);
    light_control();
}
#endif
#ifdef CLOUD_BAIDU_TEST
#include "cloud_common.h"
extern int8_t baidu_connected_subscribe_ready;
static int baidu_test_cnt = 0;
extern int baidu_reconnect_cnt;
void baidu_test(void)
{
    if (!baidu_connected_subscribe_ready) return;

    if (semaphore_flag != SEMAPHORE_FLAG_NONE) return;

    if (light_flag == LIGHT_OFF)
        light_flag = LIGHT_ON;
    else
        light_flag = LIGHT_OFF;

    printf("----------------baidu test cnt %d <%d>------------------\n",baidu_test_cnt++,baidu_reconnect_cnt);
    light_control();
}
#endif
#ifdef CLOUD_HUAWEI_TEST
#include "cloud_common.h"
extern int8_t huawei_connected_subscribe_ready;
static int huawei_test_cnt = 0;
extern int huawei_reconnect_cnt;
void huawei_test(void)
{
    if (!huawei_connected_subscribe_ready) return;

    if (semaphore_flag != SEMAPHORE_FLAG_NONE) return;

    if (light_flag == LIGHT_OFF)
        light_flag = LIGHT_ON;
    else
        light_flag = LIGHT_OFF;

    printf("----------------huawei test cnt %d <%d>------------------\n",huawei_test_cnt++,huawei_reconnect_cnt);
    light_control();
}
#endif
void cloud_test(void)
{
#ifdef CLOUD_AWS_TEST
    aws_test();
#elif CLOUD_TENCENT_TEST
    tencent_test();
#elif CLOUD_BAIDU_TEST
    baidu_test();
#elif CLOUD_HUAWEI_TEST
    huawei_test();
#endif
}
#endif
