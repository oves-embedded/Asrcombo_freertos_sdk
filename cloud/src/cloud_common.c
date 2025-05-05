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

#ifdef CLOUD_SUPPORT
#include "cloud_common.h"
#include "lega_rtos_api.h"
/******************baidu var********************** */
#ifdef CLOUD_BAIDU_SUPPORT
lega_semaphore_t baidu_Semaphore = NULL;
#endif
/******************private var********************** */
#ifdef CLOUD_PRIVATE_SUPPORT
lega_semaphore_t private_Semaphore = NULL;
#endif

/******************mqtt_broker var********************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT
lega_semaphore_t mqtt_broker_Semaphore = NULL;
#endif

/******************jd var********************** */
#ifdef CLOUD_JD_SUPPORT
lega_semaphore_t jd_Semaphore = NULL;
#endif

/******************tencent var********************** */
#ifdef CLOUD_TENCENT_SUPPORT
lega_semaphore_t tencent_Semaphore = NULL;
#endif

/******************huawei var********************** */
#ifdef CLOUD_HUAWEI_SUPPORT
lega_semaphore_t huawei_Semaphore = NULL;
#endif

/******************aws var********************** */
#ifdef CLOUD_AWS_SUPPORT
lega_semaphore_t aws_Semaphore = NULL;
#endif


/******************common var********************** */
#ifdef CFG_PLF_DUET
duet_gpio_dev_t g_lega_gpio7, g_lega_gpio6;
#else
lega_gpio_dev_t g_lega_gpio7, g_lega_gpio6;
#endif
uint8_t semaphore_flag = SEMAPHORE_FLAG_NONE;
uint8_t light_flag = LIGHT_OFF;
uint8_t light_gpio_init_flag = 0;
int light_status = 0;
OSStatus ret;

void light_control()
{
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_MESSAGE;   //test found, when incoming data, will occurring lega_gpio7_irq_handler by lega_gpio_output_low(&g_lega_gpio6);
    if(light_flag == LIGHT_ON)
        lega_gpio_output_low(&g_lega_gpio6);
    else if(light_flag == LIGHT_OFF)
        lega_gpio_output_high(&g_lega_gpio6);
    else
    {
        printf("light control status error\n");
        semaphore_flag = SEMAPHORE_FLAG_NONE;
        return;
    }
    #ifdef CLOUD_BAIDU_SUPPORT
    if(baidu_Semaphore != NULL)
        lega_rtos_set_semaphore(&baidu_Semaphore);
    #endif
    #ifdef CLOUD_MQTT_BROKER_SUPPORT
    if(mqtt_broker_Semaphore != NULL)
        lega_rtos_set_semaphore(&mqtt_broker_Semaphore);
    #endif
    #ifdef CLOUD_JD_SUPPORT
    if(jd_Semaphore != NULL)
        lega_rtos_set_semaphore(&jd_Semaphore);
    #endif
    #ifdef CLOUD_TENCENT_SUPPORT
    if(tencent_Semaphore != NULL)
        lega_rtos_set_semaphore(&tencent_Semaphore);
    #endif
    #ifdef CLOUD_HUAWEI_SUPPORT
    if(huawei_Semaphore != NULL)
        lega_rtos_set_semaphore(&huawei_Semaphore);
    #endif
    #ifdef CLOUD_AWS_SUPPORT
    if(aws_Semaphore != NULL)
    {
        ret = lega_rtos_set_semaphore(&aws_Semaphore);
        if (ret == kGeneralErr)
        {
            printf("aws set semaphore fail %d\n",ret);
        }

    }
    #endif
}

void lega_gpio7_irq_handler(void *arg) //for gpio7
{
    if(semaphore_flag != SEMAPHORE_FLAG_NONE)
    {
        return; //flutter free
    }
    if(light_flag == LIGHT_OFF)
        light_flag = LIGHT_ON;
    else
    {
        light_flag = LIGHT_OFF;
    }
    printf("light kick\r\n");
    light_control();
}

//note: before test, please short GPIO0 and GPIO6. GPIO0 functions as input, while GPIO6 output
void light_gpio_init(void)
{
    light_gpio_init_flag = 1;
    //gpio0 input
    g_lega_gpio7.port = GPIO7_INDEX;
    g_lega_gpio7.config = LEGA_INPUT_PULL_DOWN;
    g_lega_gpio7.priv = NULL;
    lega_gpio_init(&g_lega_gpio7);
    lega_gpio_enable_irq(&g_lega_gpio7, LEGA_IRQ_TRIGGER_FALLING_EDGE, lega_gpio7_irq_handler, NULL);

    //gpio1 output
    g_lega_gpio6.port = GPIO6_INDEX;
    g_lega_gpio6.config = LEGA_OUTPUT_PUSH_PULL;
    g_lega_gpio6.priv = NULL;
    lega_gpio_init(&g_lega_gpio6);
    lega_gpio_output_high(&g_lega_gpio6);
}
int is_cloud_connected()
{
/******************private var********************** */
#ifdef CLOUD_PRIVATE_SUPPORT
    if(private_Semaphore != NULL)
        return 1;
#endif

/******************mqtt_broker var********************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT
    if(mqtt_broker_Semaphore != NULL)
        return 1;
#endif
/******************baidu var********************** */
#ifdef CLOUD_BAIDU_SUPPORT
    if(baidu_Semaphore != NULL)
        return 1;
#endif
/******************jd var********************** */
#ifdef CLOUD_JD_SUPPORT
    if(jd_Semaphore != NULL)
        return 1;
#endif
/******************tencent var********************** */
#ifdef CLOUD_TENCENT_SUPPORT
    if(tencent_Semaphore != NULL)
        return 1;
#endif
/******************huawei var********************** */
#ifdef CLOUD_HUAWEI_SUPPORT
    if(huawei_Semaphore != NULL)
        return 1;
#endif
/******************aws var********************** */
#ifdef CLOUD_AWS_SUPPORT
    if(aws_Semaphore != NULL)
        return 1;
#endif
    return 0;
}
#endif