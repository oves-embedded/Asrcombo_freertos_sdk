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


#ifdef AT_USER_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include "lega_at_api.h"
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#else
#include "lega_common.h"
#endif
#ifdef CLOUD_SUPPORT
#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif
#include "cloud.h"
#include "lwip/err.h"
#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include "private_fun.h"
#endif
#endif
#include <string.h>

//joylink
#ifdef CLOUD_JOYLINK_SUPPORT
#include "joylink_softap_start.h"
#include "joylink_softap.h"
#include "joylink_thread.h"
#endif
/*
 ************************************************************
 *                    USER AT CMD START
 *
 ************************************************************
 */
#ifdef CLOUD_SUPPORT
int at_cloud_cmd(int argc, char **argv)
{
#ifdef CLOUD_PRIVATE_SUPPORT
    int temperature = 0;
    char send_buf[50] = {0};
#endif
    if(argc == 3 && (!strcmp(argv[1], "light")))
    {
        if(!strcmp(argv[2], "on"))
        {
            light_flag = LIGHT_ON;
            light_control();
        }
        else if(!strcmp(argv[2], "off"))
        {
            light_flag = LIGHT_OFF;
            light_control();
        }
    }
    else if(argc == 3 && (!strcmp(argv[1], "temperature")))
    {
#ifdef CLOUD_PRIVATE_SUPPORT
        if(private_client == NULL)
        {
            dbg_at("private cloud not connected!\n");
        }
        temperature = convert_str_to_int(argv[2]);
        if(temperature < 1000 && temperature > 0)
        {
            sprintf(send_buf, "{\"temperature\":%s}", argv[2]);
            mqtt_publish(private_client, "v1/devices/me/telemetry", send_buf, strlen(send_buf), 0, 0, NULL,  NULL);
        }
#else
        dbg_at("not support private cloud, please open macro CLOUD_PRIVATE_SUPPORT\n");
#endif
    }
    return 0;
}
#ifdef CLOUD_PRIVATE_SUPPORT
int at_cloud_private(int argc, char **argv)
{
    int32_t ret;
    char private_host[129] = {0};
    int32_t private_host_len = 127;
    char private_protocol[16] = {0};
    int32_t private_protocol_len = 15;
    int private_port = 8883;
    int32_t private_port_len = 4;
    char private_tls_switch[16] = {0};
    int32_t private_tls_switch_len = 15;
    char private_username[64] = {0};
    int32_t private_username_len = 63;
    int private_timeout = 100;
    int32_t private_timeout_len = 4;
    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("private_host", private_host, &private_host_len);
        if(ret != 0)
        {
            dbg_at("private host get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private host = %s\n", private_host);
        }

        ret = lega_flash_kv_get("private_tls_switch", private_tls_switch, &private_tls_switch_len);
        if(ret != 0)
        {
            dbg_at("private_tls_switch get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private_tls_switch = %s\n", private_tls_switch);
        }

        ret = lega_flash_kv_get("private_username", private_username, &private_username_len);
        if(ret != 0)
        {
            dbg_at("private_username get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private_username = %s\n", private_username);
        }

        ret = lega_flash_kv_get("private_protocol", private_protocol, &private_protocol_len);
        if(ret != 0)
        {
            dbg_at("private_protocol get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private_protocol = %s\n", private_protocol);
        }

        ret = lega_flash_kv_get("private_port", &private_port, &private_port_len);
        if(ret != 0)
        {
            dbg_at("private port get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private port = %d\n", private_port);
        }

        ret = lega_flash_kv_get("private_timeout", &private_timeout, &private_timeout_len);
        if(ret != 0)
        {
            dbg_at("private timeout get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("private timeout = %d\n", private_timeout);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&Private_Cloud_Task_Handler, PRIVATE_CLOUD_TASK_PRIORITY, PRIVATE_CLOUD_TASK_NAME, private_cloud_task, PRIVATE_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))//baidu write username xxxxxxxx
    {
        if(!strcmp(argv[2], "host"))
        {
            strcpy(private_host, argv[3]);
            private_host_len = strlen(private_host);
            ret = lega_flash_kv_set("private_host", private_host, private_host_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_host = %s\n", private_host);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "tls_switch"))
        {
            strcpy(private_tls_switch, argv[3]);
            private_tls_switch_len = strlen(private_tls_switch);
            ret = lega_flash_kv_set("private_tls_switch", private_tls_switch, private_tls_switch_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_tls_switch = %s\n", private_tls_switch);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "username"))
        {
            strcpy(private_username, argv[3]);
            private_username_len = strlen(private_username);
            ret = lega_flash_kv_set("private_username", private_username, private_username_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_username = %s\n", private_username);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "protocol"))
        {
            strcpy(private_protocol, argv[3]);
            private_protocol_len = strlen(private_protocol);
            ret = lega_flash_kv_set("private_protocol", private_protocol, private_protocol_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_protocol = %s\n", private_protocol);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
//        else if(!strcmp(argv[2], "host"))
//        {
//            strcpy(private_host, argv[3]);
//            private_host_len = strlen(private_host);
//            ret = lega_flash_kv_set("private_host", private_host, private_host_len, 1);
//            if(ret == 0)
//            {
//                dbg_at("write ok\n, private_host = %s\n", private_host);
//            }
//            else
//            {
//                dbg_at("write error\n");
//                return CONFIG_FAIL;
//            }
//        }
        else if(!strcmp(argv[2], "port"))
        {
            private_port = convert_str_to_int(argv[3]);
            if(private_port == LEGA_STR_TO_INT_ERR || private_port <= 0)
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
            ret = lega_flash_kv_set("private_port", &private_port, private_port_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_port = %d\n", private_port);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }

        else if(!strcmp(argv[2], "timeout"))
        {
            private_timeout = convert_str_to_int(argv[3]);
            if(private_timeout == LEGA_STR_TO_INT_ERR || private_timeout <= 0)
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
            ret = lega_flash_kv_set("private_timeout", &private_timeout, private_timeout_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, private_timeout = %d\n", private_timeout);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop private cloud\n");
        if(private_Semaphore == NULL)
            return CONFIG_FAIL;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&private_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_MQTT_BROKER_SUPPORT
int at_cloud_mqtt_broker(int argc, char **argv)
{
    int32_t ret;
    char mqtt_broker_host[129] = {0};
    int32_t mqtt_broker_host_len = 127;
    int mqtt_broker_port = 1883;
    int32_t mqtt_broker_port_len = 4;
    char mqtt_broker_tls_switch[16] = {0};
    int32_t mqtt_broker_tls_switch_len = 15;
    char mqtt_broker_username[64] = {0};
    int32_t mqtt_broker_username_len = 63;
    char mqtt_broker_userpass[64] = {0};
    int32_t mqtt_broker_userpass_len = 63;
    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("mqtt_broker_host", mqtt_broker_host, &mqtt_broker_host_len);
        if(ret != 0)
        {
            dbg_at("mqtt_broker host get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("mqtt_broker host = %s\n", mqtt_broker_host);
        }

        ret = lega_flash_kv_get("mqtt_broker_tls_switch", mqtt_broker_tls_switch, &mqtt_broker_tls_switch_len);
        if(ret != 0)
        {
            dbg_at("mqtt_broker_tls_switch get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("mqtt_broker_tls_switch = %s\n", mqtt_broker_tls_switch);
        }

        ret = lega_flash_kv_get("mqtt_broker_username", mqtt_broker_username, &mqtt_broker_username_len);
        if(ret != 0)
        {
            dbg_at("mqtt_broker_username get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("mqtt_broker_username = %s\n", mqtt_broker_username);
        }

        ret = lega_flash_kv_get("mqtt_broker_userpass", mqtt_broker_userpass, &mqtt_broker_userpass_len);
        if(ret != 0)
        {
            dbg_at("mqtt_broker_userpass get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("mqtt_broker_userpass = %s\n", mqtt_broker_userpass);
        }

        ret = lega_flash_kv_get("mqtt_broker_port", &mqtt_broker_port, &mqtt_broker_port_len);
        if(ret != 0)
        {
            dbg_at("mqtt_broker port get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("mqtt_broker port = %d\n", mqtt_broker_port);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&Mqtt_broker_Cloud_Task_Handler, MQTT_BROKER_CLOUD_TASK_PRIORITY, MQTT_BROKER_CLOUD_TASK_NAME, mqtt_broker_cloud_task, MQTT_BROKER_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))//mqtt write username xxxxxxxx
    {
        if(!strcmp(argv[2], "host"))
        {
            strcpy(mqtt_broker_host, argv[3]);
            mqtt_broker_host_len = strlen(mqtt_broker_host);
            ret = lega_flash_kv_set("mqtt_broker_host", mqtt_broker_host, mqtt_broker_host_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, mqtt_broker_host = %s\n", mqtt_broker_host);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "tls_switch"))
        {
            strcpy(mqtt_broker_tls_switch, argv[3]);
            mqtt_broker_tls_switch_len = strlen(mqtt_broker_tls_switch);
            ret = lega_flash_kv_set("mqtt_broker_tls_switch", mqtt_broker_tls_switch, mqtt_broker_tls_switch_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, mqtt_broker_tls_switch = %s\n", mqtt_broker_tls_switch);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "username"))
        {
            strcpy(mqtt_broker_username, argv[3]);
            mqtt_broker_username_len = strlen(mqtt_broker_username);
            ret = lega_flash_kv_set("mqtt_broker_username", mqtt_broker_username, mqtt_broker_username_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, mqtt_broker_username = %s\n", mqtt_broker_username);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "userpass"))
        {
            strcpy(mqtt_broker_userpass, argv[3]);
            mqtt_broker_userpass_len = strlen(mqtt_broker_userpass);
            ret = lega_flash_kv_set("mqtt_broker_userpass", mqtt_broker_userpass, mqtt_broker_userpass_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, mqtt_broker_userpass = %s\n", mqtt_broker_userpass);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
//        else if(!strcmp(argv[2], "host"))
//        {
//            strcpy(mqtt_broker_host, argv[3]);
//            mqtt_broker_host_len = strlen(mqtt_broker_host);
//            ret = lega_flash_kv_set("mqtt_broker_host", mqtt_broker_host, mqtt_broker_host_len, 1);
//            if(ret == 0)
//            {
//                dbg_at("write ok\n, mqtt_broker_host = %s\n", mqtt_broker_host);
//            }
//            else
//            {
//                dbg_at("write error\n");
//                return CONFIG_FAIL;
//            }
//        }
        else if(!strcmp(argv[2], "port"))
        {
            mqtt_broker_port = convert_str_to_int(argv[3]);
            if(mqtt_broker_port == LEGA_STR_TO_INT_ERR || mqtt_broker_port <= 0)
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
            ret = lega_flash_kv_set("mqtt_broker_port", &mqtt_broker_port, mqtt_broker_port_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, mqtt_broker_port = %d\n", mqtt_broker_port);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop mqtt_broker cloud\n");
        if(mqtt_broker_Semaphore == NULL)
            return CONFIG_FAIL;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&mqtt_broker_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_BAIDU_SUPPORT
int at_cloud_baidu(int argc, char **argv)
{
    //xTaskCreate(cloud_task,CLOUD_TASK_NAME,CLOUD_STACK_SIZE>>2,0,CLOUD_TASK_PRIORITY,&cloud_task_Handler);
    int32_t ret;
    char baidu_username[32] = {0};
    int32_t baidu_username_len = 32;
    char baidu_userpass[64] = {0};
    int32_t baidu_userpass_len = 64;
    char baidu_clientid[32] = {0};
    int32_t baidu_clientid_len = 32;
    char baidu_url[64] = {0};
    int32_t baidu_url_len = 64;
    int baidu_timeout = 100;
    int32_t baidu_timeout_len = 4;
    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("baidu_username", baidu_username, &baidu_username_len);
        if(ret != 0)
        {
            dbg_at("baidu username get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("baidu username = %s\n", baidu_username);
        }

        ret = lega_flash_kv_get("baidu_userpass", baidu_userpass, &baidu_userpass_len);
        if(ret != 0)
        {
            dbg_at("baidu userpass get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("baidu userpass = %s\n", baidu_userpass);
        }

        ret = lega_flash_kv_get("baidu_clientid", baidu_clientid, &baidu_clientid_len);
        if(ret != 0)
        {
            dbg_at("baidu clientid get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("baidu clientid = %s\n", baidu_clientid);
        }

        ret = lega_flash_kv_get("baidu_url", baidu_url, &baidu_url_len);
        if(ret != 0)
        {
            dbg_at("baidu url get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("baidu url = %s\n", baidu_url);
        }

        ret = lega_flash_kv_get("baidu_timeout", &baidu_timeout, &baidu_timeout_len);
        if(ret != 0)
        {
            //dbg_at("baidu timeout get error\n");
        }
        else
        {
            dbg_at("baidu timeout = %d\n", baidu_timeout);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&Baidu_Cloud_Task_Handler, BAIDU_CLOUD_TASK_PRIORITY, BAIDU_CLOUD_TASK_NAME, baidu_cloud_task, BAIDU_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))//baidu write username xxxxxxxx
    {
        if(!strcmp(argv[2], "userName"))
        {
            strcpy(baidu_username, argv[3]);
            baidu_username_len = strlen(baidu_username);
            ret = lega_flash_kv_set("baidu_username", baidu_username, baidu_username_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, baidu username = %s\n", baidu_username);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "userPass"))
        {
            strcpy(baidu_userpass, argv[3]);
            baidu_userpass_len = strlen(baidu_userpass);
            ret = lega_flash_kv_set("baidu_userpass", baidu_userpass, baidu_userpass_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, baidu userpass = %s\n", baidu_userpass);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "clientID"))
        {
            strcpy(baidu_clientid, argv[3]);
            baidu_clientid_len = strlen(baidu_clientid);
            ret = lega_flash_kv_set("baidu_clientid", baidu_clientid, baidu_clientid_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, baidu clientid = %s\n", baidu_clientid);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "hostName"))
        {
            strcpy(baidu_url, argv[3]);
            baidu_url_len = strlen(baidu_url);
            ret = lega_flash_kv_set("baidu_url", baidu_url, baidu_url_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, baidu url = %s\n", baidu_url);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "timeout"))
        {
            baidu_timeout = convert_str_to_int(argv[3]);
            if(baidu_timeout == LEGA_STR_TO_INT_ERR || baidu_timeout <= 0)
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
            ret = lega_flash_kv_set("baidu_timeout", &baidu_timeout, baidu_timeout_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, baidu timeout = %d\n", baidu_timeout);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop baidu cloud\n");
        if(baidu_Semaphore == NULL)
            return CONFIG_OK;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&baidu_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_TENCENT_SUPPORT
int at_cloud_tencent(int argc, char **argv)
{
    int32_t ret;
    char productID[TENCENT_PRODUCT_ID_LEN + 1] = {0};
    int32_t productID_len = TENCENT_PRODUCT_ID_LEN;
    char deviceName[TENCENT_DEVICE_NAME_LEN + 1] = {0};
    int32_t deviceName_len = TENCENT_DEVICE_NAME_LEN;
    char hostName[TENCENT_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = TENCENT_HOSTNAME_MAXLEN;

    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("tencent_productID", productID, &productID_len);
        if(ret != 0)
        {
            dbg_at("tencent productID get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("tencent productID = %s\n", productID);
        }

        ret = lega_flash_kv_get("tencent_deviceName", deviceName, &deviceName_len);
        if(ret != 0)
        {
            dbg_at("tencent deviceName get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("tencent deviceName = %s\n", deviceName);
        }

        ret = lega_flash_kv_get("tencent_hostName", hostName, &hostName_len);
        if(ret != 0)
        {
            dbg_at("tencent hostName get error\n");
        }
        else
        {
            dbg_at("tencent hostName = %s\n", hostName);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&TENCENT_Cloud_Task_Handler, TENCENT_CLOUD_TASK_PRIORITY, TENCENT_CLOUD_TASK_NAME, tencent_cloud_task, TENCENT_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))   //tencent write username xxxxxxxx
    {
        if(!strcmp(argv[2], "productID"))
        {
            strcpy(productID, argv[3]);
            productID_len = strlen(productID);
            ret = lega_flash_kv_set("tencent_productID", productID, productID_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, tencent productID = %s\n", productID);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "deviceName"))
        {
            strcpy(deviceName, argv[3]);
            deviceName_len = strlen(deviceName);
            ret = lega_flash_kv_set("tencent_deviceName", deviceName, deviceName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, tencent deviceName = %s\n", deviceName);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "hostName"))
        {
            strcpy(hostName, argv[3]);
            hostName_len = strlen(hostName);
            ret = lega_flash_kv_set("tencent_hostName", hostName, hostName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, tencent hostName = %s\n", hostName);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop tencent cloud\n");
        if(tencent_Semaphore == NULL)
            return CONFIG_OK;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&tencent_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_AWS_SUPPORT
int at_cloud_aws(int argc, char **argv)
{
    int32_t ret;
    char productID[AWS_PRODUCT_ID_LEN + 1] = {0};
    int32_t productID_len = AWS_PRODUCT_ID_LEN;
    char clientId[AWS_CLIENT_ID_LEN + 1] = {0};
    int32_t clientid_len = AWS_CLIENT_ID_LEN;
    char hostName[AWS_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = AWS_HOSTNAME_MAXLEN;

    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("aws_productID", productID, &productID_len);
        if(ret != 0)
        {
            dbg_at("aws productID get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("aws productID = %s\n", productID);
        }

        ret = lega_flash_kv_get("aws_clientId", clientId, &clientid_len);
        if(ret != 0)
        {
            dbg_at("aws clientID get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("aws clientId = %s\n", clientId);
        }

        ret = lega_flash_kv_get("aws_hostName", hostName, &hostName_len);
        if(ret != 0)
        {
            dbg_at("aws hostName get none\n");
        }
        else
        {
            dbg_at("aws hostName = %s\n", hostName);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&AWS_Cloud_Task_Handler, AWS_CLOUD_TASK_PRIORITY, AWS_CLOUD_TASK_NAME, aws_cloud_task, AWS_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))     //e.g. aws write productID xxxxxxxx
    {
        if(!strcmp(argv[2], "productID"))
        {
            strcpy(productID, argv[3]);
            productID_len = strlen(productID);
            ret = lega_flash_kv_set("aws_productID", productID, productID_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, aws productID = %s\n", productID);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "clientID"))
        {
            strcpy(clientId, argv[3]);
            clientid_len = strlen(clientId);
            ret = lega_flash_kv_set("aws_clientId", clientId, clientid_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, aws clientId = %s\n", clientId);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "hostName"))
        {
            strcpy(hostName, argv[3]);
            hostName_len = strlen(hostName);
            ret = lega_flash_kv_set("aws_hostName", hostName, hostName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, aws hostName = %s\n", hostName);
            }
            else
            {
                dbg_at("write error\n");
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop aws cloud\n");
        if(aws_Semaphore == NULL)
            return CONFIG_OK;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&aws_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_JD_SUPPORT
int at_cloud_jd(int argc, char **argv)
{
    int32_t ret;
    char productKey[IOT_PRODUCT_KEY_LEN + 1] = {0};
    int32_t productKey_len = IOT_PRODUCT_KEY_LEN;
    char productSecret[IOT_PRODUCT_SECRET_LEN +1] = {0};
    int32_t productSecret_len = IOT_PRODUCT_SECRET_LEN;
    char identifer[IOT_IDENTIFIER_LEN + 1] = {0};
    int32_t identifer_len = IOT_IDENTIFIER_LEN;
    char deviceName[IOT_DEVICE_NAME_LEN + 1] = {0};
    int32_t deviceName_len = IOT_DEVICE_NAME_LEN;
    char deviceSecret[IOT_DEVICE_SECRET_LEN + 1] = {0};
    int32_t deviceSecret_len = IOT_DEVICE_SECRET_LEN;
    char hostName[DEV_AUTH_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = DEV_AUTH_HOSTNAME_MAXLEN;

    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("jd_productKey", productKey, &productKey_len);
        if(ret != 0)
        {
            dbg_at("jd productKey get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd productKey = %s\n", productKey);
        }

        ret = lega_flash_kv_get("jd_productSecret", productSecret, &productSecret_len);
        if(ret != 0)
        {
            dbg_at("jd productSecret get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd productSecret = %s\n", productSecret);
        }

        ret = lega_flash_kv_get("jd_identifer", identifer, &identifer_len);
        if(ret != 0)
        {
            dbg_at("jd identifer get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd identifer = %s\n", identifer);
        }

        ret = lega_flash_kv_get("jd_deviceName", deviceName, &deviceName_len);
        if(ret != 0)
        {
            dbg_at("jd deviceName get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd deviceName = %s\n", deviceName);
        }

        ret = lega_flash_kv_get("jd_deviceSecret", deviceSecret, &deviceSecret_len);
        if(ret != 0)
        {
            dbg_at("jd deviceSecret get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd deviceSecret = %s\n", deviceSecret);
        }

        ret = lega_flash_kv_get("jd_hostName", hostName, &hostName_len);
        if(ret != 0)
        {
            dbg_at("jd hostName get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("jd hostName = %s\n", hostName);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&JD_Cloud_Task_Handler, JD_CLOUD_TASK_PRIORITY, JD_CLOUD_TASK_NAME, jd_cloud_task, JD_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write")) //jd write username xxxxxxxx
    {
        if(!strcmp(argv[2], "productKey"))
        {
            strcpy(productKey, argv[3]);
            productKey_len = strlen(productKey);
            ret = lega_flash_kv_set("jd_productKey", productKey, productKey_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd productKey = %s\n", productKey);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "productSecret"))
        {
            strcpy(productSecret, argv[3]);
            productSecret_len = strlen(productSecret);
            ret = lega_flash_kv_set("jd_productSecret", productSecret, productSecret_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd productSecret = %s\n", productSecret);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "identifer"))
        {
            strcpy(identifer, argv[3]);
            identifer_len = strlen(identifer);
            ret = lega_flash_kv_set("jd_identifer", identifer, identifer_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd identifer = %s\n", identifer);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "deviceName"))
        {
            strcpy(deviceName, argv[3]);
            deviceName_len = strlen(deviceName);
            ret = lega_flash_kv_set("jd_deviceName", deviceName, deviceName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd deviceName = %s\n", deviceName);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "deviceSecret"))
        {
            strcpy(deviceSecret, argv[3]);
           deviceSecret_len = strlen(deviceSecret);
            ret = lega_flash_kv_set("jd_deviceSecret", deviceSecret, deviceSecret_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd deviceSecret = %s\n", deviceSecret);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "hostName"))
        {
            strcpy(hostName, argv[3]);
            hostName_len = strlen(hostName);
            ret = lega_flash_kv_set("jd_hostName", hostName, hostName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, jd hostName = %s\n", hostName);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop jd cloud\n");
        if(jd_Semaphore == NULL)
            return CONFIG_OK;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(&jd_Semaphore);
    }
    return CONFIG_OK;
}
#endif
#ifdef CLOUD_HUAWEI_SUPPORT
int at_cloud_huawei(int argc, char **argv)
{
    int32_t ret;
    char deviceID[HUAWEI_DEVICE_ID_LEN + 1] = {0};
    int32_t deviceID_len = HUAWEI_DEVICE_ID_LEN;
    char deviceSecret[HUAWEI_DEVICE_SECRET_LEN + 1] = {0};
    int32_t deviceSecret_len = HUAWEI_DEVICE_SECRET_LEN;
    char hostName[HUAWEI_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = HUAWEI_HOSTNAME_MAXLEN;

    if(argc == 1 || (argc == 2 && (!strcmp(argv[1], "start") || !strcmp(argv[1], "show"))))
    {
        ret = lega_flash_kv_get("huawei_deviceID", deviceID, &deviceID_len);
        if(ret != 0)
        {
            dbg_at("huawei deviceID get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("huawei deviceID = %s\n", deviceID);
        }

        ret = lega_flash_kv_get("huawei_deviceSecret", deviceSecret, &deviceSecret_len);
        if(ret != 0)
        {
            dbg_at("huawei deviceSecret get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("huawei deviceSecret = %s\n", deviceSecret);
        }

        ret = lega_flash_kv_get("huawei_hostName", hostName, &hostName_len);
        if(ret != 0)
        {
            dbg_at("huawei hostName get error\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("huawei hostName = %s\n", hostName);
        }

        if(!strcmp(argv[1], "start"))
        {
            if(is_cloud_connected() != 0)
            {
                dbg_at("cloud task working already...\n");
                return CONFIG_OK;
            }
            lega_rtos_create_thread(&HUAWEI_Cloud_Task_Handler, HUAWEI_CLOUD_TASK_PRIORITY, HUAWEI_CLOUD_TASK_NAME, huawei_cloud_task, HUAWEI_CLOUD_STACK_SIZE, 0);
        }
    }
    else if(argc == 4 && !strcmp(argv[1], "write"))//huawei write username xxxxxxxx
    {
        if(!strcmp(argv[2], "deviceID"))
        {
            strcpy(deviceID, argv[3]);
            deviceID_len = strlen(deviceID);
            ret = lega_flash_kv_set("huawei_deviceID", deviceID, deviceID_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, huawei deviceID = %s\n", deviceID);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "deviceSecret"))
        {
            strcpy(deviceSecret, argv[3]);
            deviceSecret_len = strlen(deviceSecret);
            ret = lega_flash_kv_set("huawei_deviceSecret", deviceSecret, deviceSecret_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, huawei deviceSecret = %s\n", deviceSecret);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
        else if(!strcmp(argv[2], "hostName"))
        {
            strcpy(hostName, argv[3]);
            hostName_len = strlen(hostName);
            ret = lega_flash_kv_set("huawei_hostName", hostName, hostName_len, 1);
            if(ret == 0)
            {
                dbg_at("write ok\n, huawei hostName = %s\n", hostName);
            }
            else
            {
                dbg_at("write error\n");
                return CONFIG_FAIL;
            }
        }
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop huawei cloud\n");
        if(huawei_Semaphore == NULL)
            return CONFIG_OK;
        semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
        lega_rtos_set_semaphore(huawei_Semaphore);
    }
    return CONFIG_OK;

}
#endif
#ifdef CLOUD_CONNECT_TEST
int at_cloud_connect_test(int argc, char **argv)
{
    if(argc != 2)
        return PARAM_MISS;

    if(argc == 2 && !strcmp(argv[1], "start"))
    {
        if (_connect_test_semaphore != NULL)
            return CONFIG_FAIL;
        lega_rtos_create_thread(&Cloud_Connect_Task_Handler, CLOUD_CONNECT_TASK_PRIORITY, CLOUD_CONNECT_TASK_NAME, cloud_connect_test, CLOUD_CONNECT_STACK_SIZE, 0);
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        if (_connect_test_semaphore == NULL)
            return CONFIG_FAIL;
        lega_rtos_set_semaphore(_connect_test_semaphore);
    }
    else
        return CONFIG_FAIL;

    return CONFIG_OK;
}
#endif
#endif
//joylink
#ifdef CLOUD_JOYLINK_SUPPORT
#define     CLOUD_JOYLINK_SETUP_NET_TASK_PRIORITY          3
#define     CLOUD_JOYLINK_SETUP_NET_TASK_NAME              "Joylink_Softap"
#define     CLOUD_JOYLINK_SETUP_NET_STACK_SIZE             2048*3

void cloud_joylink_setup_net_task(lega_thread_arg_t arg)
{
#if 1
    //1. setup net
    jl_softap_enter_ap_mode();
    joylink_softap_start(300000);//5 minutes
#else //1. ble adapter.

#endif

    //2. delete thread
    lega_rtos_delete_thread(NULL);
}

int at_cloud_joylink(int argc, char **argv)
{
    if(argc == 2 && !strcmp(argv[1], "start"))
    {
        dbg_at("cmd: star joylink cloud\n");

        lega_thread_t Cloud_JOYLINK_SETUP_NET_Task_Handler;
        lega_rtos_create_thread(&Cloud_JOYLINK_SETUP_NET_Task_Handler,
                                 CLOUD_JOYLINK_SETUP_NET_TASK_PRIORITY,
                                 CLOUD_JOYLINK_SETUP_NET_TASK_NAME,
                                 cloud_joylink_setup_net_task,
                                 CLOUD_JOYLINK_SETUP_NET_STACK_SIZE,
                                 0);
    }
    else if(argc == 2 && !strcmp(argv[1], "stop"))
    {
        dbg_at("cmd: stop joylink cloud\n");
        //close wifi
//        lega_wlan_close();
        jl_platform_thread_delete(NULL);
    }
    return CONFIG_OK;
}
#endif
/*
 ************************************************************
 *                    USER AT CMD END
 *
 ************************************************************
 */

#ifdef CLOUD_SUPPORT
cmd_entry comm_cloud_cmd = {
    .name = "cloud",
    .help = "cloud cmd",
    .function = at_cloud_cmd,
};
#ifdef CLOUD_PRIVATE_SUPPORT
cmd_entry comm_cloud_private = {
    .name = "private",
    .help = "connect private iot platform",
    .function = at_cloud_private,
};
#endif
#ifdef CLOUD_MQTT_BROKER_SUPPORT
cmd_entry comm_cloud_mqtt_broker = {
    .name = "mqtt",
    .help = "connect mqtt_broker iot platform",
    .function = at_cloud_mqtt_broker,
};
#endif
#ifdef CLOUD_BAIDU_SUPPORT
cmd_entry comm_cloud_baidu = {
    .name = "baidu",
    .help = "connect baidu iot platform",
    .function = at_cloud_baidu,
};
#endif
#ifdef CLOUD_TENCENT_SUPPORT
cmd_entry comm_cloud_tencent = {
    .name = "tencent",
    .help = "connect tencent iot platform",
    .function = at_cloud_tencent,
};
#endif
#ifdef CLOUD_AWS_SUPPORT
cmd_entry comm_cloud_aws = {
    .name = "aws",
    .help = "connect aws iot platform",
    .function = at_cloud_aws,
};
#endif
#ifdef CLOUD_JD_SUPPORT
cmd_entry comm_cloud_jd = {
    .name = "jd",
    .help = "connect jd iot platform",
    .function = at_cloud_jd,
};
#endif
#ifdef CLOUD_HUAWEI_SUPPORT
cmd_entry comm_cloud_huawei = {
    .name = "huawei",
    .help = "connect huawei iot platform",
    .function = at_cloud_huawei,
};
#endif
#ifdef CLOUD_CONNECT_TEST
cmd_entry comm_cloud_connect_test= {
    .name = "connecttest",
    .help = "connect test",
    .function = at_cloud_connect_test,
};
#endif
#endif
//joulink
#ifdef CLOUD_JOYLINK_SUPPORT
cmd_entry comm_cloud_joylink = {
    .name = "joylink",
    .help = "connect joylink iot platform",
    .function = at_cloud_joylink,
};
#endif

void lega_at_cloud_cmd_register(void)
{
#ifdef CLOUD_SUPPORT
    lega_at_cmd_register(&comm_cloud_cmd);
#ifdef CLOUD_BAIDU_SUPPORT
    lega_at_cmd_register(&comm_cloud_baidu);
#endif
#ifdef CLOUD_TENCENT_SUPPORT
    lega_at_cmd_register(&comm_cloud_tencent);
#endif
#ifdef CLOUD_AWS_SUPPORT
    lega_at_cmd_register(&comm_cloud_aws);
#endif
#ifdef CLOUD_JD_SUPPORT
    lega_at_cmd_register(&comm_cloud_jd);
#endif
#ifdef CLOUD_HUAWEI_SUPPORT
    lega_at_cmd_register(&comm_cloud_huawei);
#endif
#ifdef CLOUD_PRIVATE_SUPPORT
    lega_at_cmd_register(&comm_cloud_private);
#endif
#ifdef CLOUD_MQTT_BROKER_SUPPORT
    lega_at_cmd_register(&comm_cloud_mqtt_broker);
#endif
#ifdef CLOUD_CONNECT_TEST
    lega_at_cmd_register(&comm_cloud_connect_test);
#endif
#endif
//joylink
#ifdef CLOUD_JOYLINK_SUPPORT
    lega_at_cmd_register(&comm_cloud_joylink);
#endif
}
#endif
