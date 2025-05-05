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

#ifndef __CLOUD_COMMON_H__
#define __CLOUD_COMMON_H__
#ifdef CLOUD_SUPPORT

// #include "lega_at_api.h"
#ifdef CFG_PLF_DUET
#include "duet_gpio.h"
#define lega_gpio_output_low duet_gpio_output_low
#define lega_gpio_output_high duet_gpio_output_high
#define lega_gpio_init duet_gpio_init
#define lega_gpio_enable_irq duet_gpio_enable_irq
#define lega_flash_kv_get duet_flash_kv_get 
#define lega_flash_kv_set duet_flash_kv_set
#define lega_gpio_dev_t duet_gpio_dev_t
#define LEGA_OUTPUT_PUSH_PULL DUET_OUTPUT_PUSH_PULL
#define LEGA_INPUT_PULL_DOWN DUET_INPUT_PULL_DOWN
#define LEGA_IRQ_TRIGGER_FALLING_EDGE DUET_IRQ_TRIGGER_FALLING_EDGE
#else
#include "lega_gpio.h" 
#endif
#include "FreeRTOS.h"



#define LIGHT_ON            1
#define LIGHT_OFF           0
#define     KV_OK                                   0
#define     SEMAPHORE_FLAG_NONE                     0
#define     SEMAPHORE_FLAG_CLOUD_MESSAGE            1
#define     SEMAPHORE_FLAG_CLOUD_TASK_KILL          2
#define     SEMAPHORE_FLAG_CLOUD_SYNC               3
#define     SEMAPHORE_FLAG_CLOUD_RECONNECTING       4

#define     CLOUD_TIMEOUT_MS             20000
#define DEV_AUTH_HOSTNAME_MAXLEN    (128)
#define DEV_AUTH_CLIENT_ID_MAXLEN   (128)
#define DEV_AUTH_DEVICE_NAME_MAXLEN (128)
#define DEV_AUTH_USERNAME_MAXLEN    (64)
#define DEV_AUTH_PASSWORD_MAXLEN    (64)
#define DEV_AUTH_PASSWORD_BYTE_LEN  (20)
typedef struct _mqtt_connect_para_t
{
    char clientid[64];
    char username[64];
    char password[128];
    char hostname[128];
    int port;
}mqtt_connect_para_t;
typedef struct {
    uint16_t port;
    char hostname[DEV_AUTH_HOSTNAME_MAXLEN];
    char clientid[DEV_AUTH_CLIENT_ID_MAXLEN];
    char username[DEV_AUTH_USERNAME_MAXLEN];
    char password[DEV_AUTH_PASSWORD_MAXLEN];
} mqtt_auth_info_t;

extern lega_gpio_dev_t g_lega_gpio7, g_lega_gpio6;
extern uint8_t semaphore_flag;
extern uint8_t light_flag;
extern uint8_t light_gpio_init_flag;
extern int light_status;
extern void light_control();
extern void lega_gpio7_irq_handler(void *arg);
extern void light_gpio_init(void);
extern int is_cloud_connected();
/******************baidu var********************** */
#ifdef CLOUD_BAIDU_SUPPORT
extern lega_semaphore_t baidu_Semaphore;
#endif
/******************private var********************** */
#ifdef CLOUD_PRIVATE_SUPPORT
extern lega_semaphore_t private_Semaphore;
#endif

/******************mqtt_broker var********************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT
extern lega_semaphore_t mqtt_broker_Semaphore;
#endif

/******************jd var********************** */
#ifdef CLOUD_JD_SUPPORT
extern lega_semaphore_t jd_Semaphore;
#endif

/******************tencent var********************** */
#ifdef CLOUD_TENCENT_SUPPORT
extern lega_semaphore_t tencent_Semaphore;
#endif

/******************huawei var********************** */
#ifdef CLOUD_HUAWEI_SUPPORT
extern lega_semaphore_t huawei_Semaphore;
#endif

/******************aws var********************** */
#ifdef CLOUD_AWS_SUPPORT
extern lega_semaphore_t aws_Semaphore;
#endif


#endif
#endif