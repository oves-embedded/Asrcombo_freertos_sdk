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

#ifndef __CLOUD_H__
#define __CLOUD_H__
#ifdef CLOUD_SUPPORT
#include "FreeRTOS.h"
#include "task.h"
#include "cloud_common.h"
#include "lwip/apps/mqtt.h"
#include "jd_auth.h"
#include "jd_mqtt_func.h"
#include "tencent_func.h"
#include "huawei_func.h"
#include "aws_func.h"
/******************test_unity_var********************** */
#ifdef TEST_UNITY_CLOUD
extern int test_cloud_connected_flag;
extern int test_cloud_upload_flag;
extern int test_cloud_download_flag;
#endif

/************************** private *************************** */
#ifdef CLOUD_PRIVATE_SUPPORT
#define     PRIVATE_CLOUD_TASK_NAME              "private_cloud_task"
#define     PRIVATE_CLOUD_STACK_SIZE             3096
#define     PRIVATE_CLOUD_TIMEOUT_MS             20000
#define     PRIVATE_CONNECT_TIMES                   5
#define     PRIVATE_CLOUD_TASK_PRIORITY                   3
typedef struct private_para_struct
{
    char private_host[129];
    int32_t private_host_len;
    char private_protocol[16];
    int32_t private_protocol_len;
    int private_port;
    int32_t private_port_len;
    char private_tls_switch[16];
    int32_t private_tls_switch_len;
    char private_username[64];
    int32_t private_username_len;
    int private_timeout;
    int32_t private_timeout_len;
}private_para_t;

//TaskHandle_t cloud_task_Handler;
extern lega_thread_t Private_Cloud_Task_Handler;
extern lega_semaphore_t private_Semaphore;
extern private_para_t *private_para;
extern mqtt_client_t *private_client;
extern int private_get_value(private_para_t *para);
extern void private_cloud_task(lega_thread_arg_t arg);
#endif
/************************** mqtt_broker *************************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include "private_fun.h"
extern lega_thread_t Mqtt_broker_Cloud_Task_Handler;
extern lega_semaphore_t mqtt_broker_Semaphore;
extern mqtt_broker_para_t *mqtt_broker_para;
extern int mqtt_broker_get_value(mqtt_broker_para_t *para);
extern void mqtt_broker_cloud_task(lega_thread_arg_t arg);
#endif
/************************** baidu *************************** */
#ifdef CLOUD_BAIDU_SUPPORT
#define     BAIDU_CLOUD_TASK_NAME              "baidu_cloud_task"
#define     BAIDU_CLOUD_STACK_SIZE             3096
#define     BAIDU_CLOUD_TIMEOUT_MS             20000
#define     BAIDU_CONNECT_TIMES                5
#define     BAIDU_CLOUD_TASK_PRIORITY          3
struct baidu_para_struct
{
    char baidu_username[32];
    int32_t baidu_username_len;
    char baidu_userpass[64];
    int32_t baidu_userpass_len;
    char baidu_clientid[32];
    int32_t baidu_clientid_len;
    char baidu_url[64];
    int32_t baidu_url_len;
    int baidu_timeout;
    int32_t baidu_timeout_len;
};

//TaskHandle_t cloud_task_Handler;
extern lega_thread_t Baidu_Cloud_Task_Handler;
extern lega_semaphore_t baidu_Semaphore;
extern uint8_t baidu_get_value(struct baidu_para_struct *baidupara);
extern void baidu_cloud_task(lega_thread_arg_t arg);
#endif
/************************** jd *************************** */
#ifdef CLOUD_JD_SUPPORT
#define     JD_CLOUD_TASK_NAME              "jd_cloud_task"
#define     JD_CLOUD_TASK_PRIORITY          3
#define     JD_CLOUD_STACK_SIZE             3096
#define     JD_CLOUD_TIMEOUT_MS             20000
#define     JD_CONNECT_TIMES                   5
struct jd_para_struct
{
    char jd_username[32];
    int32_t jd_username_len;
    char jd_userpass[64];
    int32_t jd_userpass_len;
    char jd_clientid[32];
    int32_t jd_clientid_len;
    char jd_host_name[64];
    int32_t jd_host_name_len;
    int port;
};
extern lega_thread_t JD_Cloud_Task_Handler;
extern lega_semaphore_t jd_Semaphore;
extern uint8_t jd_get_value(jd_mqtt_auth_info_t *jd_para);
extern void jd_cloud_task(lega_thread_arg_t arg);
#endif
/************************** tencent *************************** */
#ifdef CLOUD_TENCENT_SUPPORT
#define     TENCENT_CLOUD_TASK_NAME              "tencent_cloud_task"
#define     TENCENT_CLOUD_TASK_PRIORITY          3
#define     TENCENT_CLOUD_STACK_SIZE             3096
#define     TENCNET_CLOUD_TIMEOUT_MS             20000
#define     TENCENT_CONNECT_TIMES                5
extern lega_thread_t TENCENT_Cloud_Task_Handler;
extern lega_semaphore_t tencent_Semaphore;
extern uint8_t tencent_get_value(mqtt_auth_info_t *tencent_para);
extern void tencent_cloud_task(lega_thread_arg_t arg);
#endif

/************************** huawei *************************** */
#ifdef CLOUD_HUAWEI_SUPPORT
#define     HUAWEI_CLOUD_TASK_NAME              "huawei_cloud_task"
#define     HUAWEI_CLOUD_TASK_PRIORITY          3
#define     HUAWEI_CLOUD_STACK_SIZE             3096
#define     HUAWEI_CLOUD_TIMEOUT_MS             20000
#define     HUAWEI_CONNECT_TIMES                5
extern lega_thread_t HUAWEI_Cloud_Task_Handler;
extern lega_semaphore_t huawei_Semaphore;
extern int huawei_get_value(mqtt_connect_para_t *huawei_para);
extern void huawei_cloud_task(lega_thread_arg_t arg);
#endif
/************************** aws *************************** */
#ifdef CLOUD_AWS_SUPPORT
#define     AWS_CLOUD_TASK_NAME              "aws_cloud_task"
#define     AWS_CLOUD_TASK_PRIORITY          3
#define     AWS_CLOUD_STACK_SIZE             3096
#define     AWS_CLOUD_TIMEOUT_MS             20000
#define     AWS_CONNECT_TIMES                5
extern lega_thread_t AWS_Cloud_Task_Handler;
extern lega_semaphore_t aws_Semaphore;
extern uint8_t aws_get_value(mqtt_auth_info_t *aws_para);
extern void aws_cloud_task(lega_thread_arg_t arg);
#endif
#ifdef CLOUD_CONNECT_TEST
#define     CLOUD_CONNECT_TASK_NAME              "cloud_connect_task"
#define     CLOUD_CONNECT_TASK_PRIORITY          3
#define     CLOUD_CONNECT_STACK_SIZE             4096
extern lega_thread_t Cloud_Connect_Task_Handler;
extern void cloud_connect_test(lega_thread_arg_t arg);
extern lega_semaphore_t _connect_test_semaphore;
#endif
#endif
#endif //__CLOUD_H__