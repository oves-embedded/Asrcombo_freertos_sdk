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

#ifndef _PRIVATE_FUNC_H_
#define _PRIVATE_FUNC_H_

#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define     MQTT_BROKER_CLOUD_TASK_NAME              "mqtt_broker_cloud_task"
#define     MQTT_BROKER_CLOUD_STACK_SIZE             3096*2
#define     MQTT_BROKER_CLOUD_TIMEOUT_MS             20000
#define     MQTT_BROKER_CONNECT_TIMES                   5
#define     MQTT_BROKER_CLOUD_TASK_PRIORITY             3

typedef struct mqtt_broker_para_struct
{
    char mqtt_broker_host[129];
    int32_t mqtt_broker_host_len;
    int mqtt_broker_port;
    int32_t mqtt_broker_port_len;
    char mqtt_broker_tls_switch[16];
    int32_t mqtt_broker_tls_switch_len;
    char mqtt_broker_username[64];
    int32_t mqtt_broker_username_len;
    char mqtt_broker_userpass[64];
    int32_t mqtt_broker_userpass_len;
}mqtt_broker_para_t;

#define     MQTT_BROKER_TOPIC_PUBLISH               0
#define     MQTT_BROKER_TOPIC_SUBSCRIBE_DATA        1
#define     MQTT_BROKER_TOPIC_SUBSCRIBE_CONTROL     2
#define     MQTT_BROKER_TOPIC_SUBSCRIBE_RESULT      3
#define     MQTT_BROKER_TOPIC_SUBSCRIBE_WILL        4
#define     MQTT_BROKER_TOPIC_SUBSCRIBE_OTA         5
#define     MQTT_BROKER_TOPIC_PUBLISH_PROGERSS      6

#define     Current_Version    1

int mqtt_broker_get_topic(char *topic_out, int action_in, char *device_id);
int mqtt_broker_get_value(mqtt_broker_para_t *para);
int parse_mqtt_Command_json(char *data);
int parse_mqtt_OTA_json(char *data);

#endif

#endif