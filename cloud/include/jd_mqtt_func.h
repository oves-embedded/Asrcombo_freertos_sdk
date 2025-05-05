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

#ifndef _JD_MQTT_FUNC_H_
#define _JD_MQTT_FUNC_H_

#ifdef CLOUD_JD_SUPPORT
#include "jd_iot_defs.h"
#include "cJSON.h"


/* Max MQTT msgid */
#define IOT_MQTT_MSG_ID_MAX                     (65535)

/* Default max length of message, in bytes */
#define CONFIG_MQTT_MESSAGE_MAXLEN              (2048)

/* Default max length of topic, in bytes */
#define CONFIG_MQTT_TOPIC_MAXLEN                (128)

/* Default keepalive interval of MQTT request, in seconds */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL          (60)

/* Default timeout value of MQTT request, in milliseconds */
#define CONFIG_MQTT_REQUEST_TIMEOUT             (2000)

/* Min timeout value of MQTT request, in milliseconds */
#define CONFIG_MQTT_REQ_TIMEOUT_MIN             (500)

/* Max timeout value of MQTT request, in milliseconds */
#define CONFIG_MQTT_REQ_TIMEOUT_MAX             (5000)

/* Min keepalive interval of MQTT request, in seconds */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN      (30)

/* Max keepalive interval of MQTT request, in seconds */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX      (180)

/* json property name, the value contains message identifier */
#define IOT_MSG_PROPERTY_MSGID                  "msgId"

/* json property name, the value contains timestamp of the message */
#define IOT_MSG_PROPERTY_TS                     "ts"

/* json property name, the value contains the playload of the message */
#define IOT_MSG_PROPERTY_DATA                   "data"

/* json property name, the value contains the version of the message */
#define IOT_MSG_PROPERTY_VERSION                "version"

/* json property name, the value contains the error message when failure happens, can be empty if success */
#define IOT_MSG_PROPERTY_MESSAGE                "message"

/* json property name, the value contains the status code, the valid values can be found in iot_mqtt_reply_code_t */
#define IOT_MSG_PROPERTY_CODE                   "code"

/* json property name, the value contains the name of request, can be an event name or service name */
#define IOT_MSG_PROPERTY_NAME                   "name"

/* json property name, the value contains the desired configs from user */
#define IOT_MSG_PROPERTY_SHADOW_DESIRED         "desired"

/* json property name, the value contains the current configs from device */
#define IOT_MSG_PROPERTY_SHADOW_REPORTED        "reported"

/* json property name, the value contains the shadow version */
#define IOT_MSG_PROPERTY_SHADOW_VERSION         "version"

/* json property name, the value contains the shadow state */
#define IOT_MSG_PROPERTY_SHADOW_STATE           "state"

/* the timestamp of shadow json */
#define IOT_MSG_PROPERTY_SHADOW_TIMESTAMP       "timestamp"

#define UP_SIDE             1
#define DOWN_SIDE           0
#define POST_REPLY          2
#define GET_SHADOW          3
#define GET_SHADOW_RPY      4
#define UPDATE_SHADOW       5
#define UPDATE_SHADOW_RPY   6
#define UPDATE_PROPERTY     7
#define UP_PROPERTY         8
#define JD_TOPIC_LEN        100
#define JD_PAYLAD           256

typedef enum {
    MQTT_SUB_TYPE_PROPERTY_REPLY = 0,
    MQTT_SUB_TYPE_EVENT_REPLY,
    MQTT_SUB_TYPE_SERVICE,
    MQTT_SUB_TYPE_SHADOW_UPDATE,
    MQTT_SUB_TYPE_DEVICE_UPDATE_SHADOW_REPLY,
    MQTT_SUB_TYPE_GET_SHADOW_REPLY,
    MQTT_SUB_DYNAMIC_AUTH,
} iot_mqtt_sub_t;

typedef enum {
    MQTT_PUB_TYPE_PROPERTY = 0,
    MQTT_PUB_TYPE_EVENT,
    MQTT_PUB_TYPE_SERVICE_REPLY,
    MQTT_PUB_TYPE_SHADOW_REPLY,
    MQTT_PUB_TYPE_DEVICE_UPDATE_SHADOW,
    MQTT_PUB_TYPE_GET_SHADOW,
} iot_mqtt_pub_t;

typedef struct{
    char pub_topic_buf[JD_TOPIC_LEN];
    char sub_topic_buf[JD_TOPIC_LEN];
    //char payload_buf[JD_PAYLAD];
}jd_mqtt_buf_t;

extern jd_mqtt_buf_t *p_jd_mqtt_buf;
extern uint32_t _shadowVersion;
extern int jd_mqtt_get_topic(char *topic_out, int side);
extern cJSON * iot_mqtt_build_device_shadow_update_json(cJSON * current, uint64_t ts);
extern char * jd_get_pub_payload(int light_switch, int action);

#endif
#endif