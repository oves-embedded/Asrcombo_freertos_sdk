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

#ifdef CLOUD_JD_SUPPORT
#include "jd_mqtt_func.h"
#include "cloud_common.h"
#include <stdio.h>
#include <string.h>
#include "lega_rtos_api.h"
#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif
uint32_t _shadowVersion = 0;
static double msg_id = 1;
#define KV_OK 0

jd_mqtt_buf_t *p_jd_mqtt_buf = NULL;


cJSON * jd_get_update_json_data(int light_switch)
{
    cJSON * pJsonData  = NULL;
    pJsonData = cJSON_CreateObject();
    if(pJsonData == NULL)
    {
        printf("json creat error\n");
        return NULL;
    }
    if(light_switch == LIGHT_ON)
        cJSON_AddStringToObject(pJsonData, "switch", "True");
    else if(light_switch == LIGHT_OFF)
        cJSON_AddStringToObject(pJsonData, "switch", "False");
    return pJsonData;
}

cJSON * jd_get_update_json_data_property(int light_switch)
{
    cJSON * pJsonData  = NULL;
    pJsonData = cJSON_CreateObject();
    if(pJsonData == NULL)
    {
        printf("json creat error\n");
        return NULL;
    }
    if(light_switch == LIGHT_ON)
        cJSON_AddStringToObject(pJsonData, "turn", "True");
    else if(light_switch == LIGHT_OFF)
        cJSON_AddStringToObject(pJsonData, "turn", "False");
    return pJsonData;
}

static void add_base_json(cJSON * pJsonRoot)
{
    printf("func: add_base_json\n");
    cJSON_AddNumberToObject(pJsonRoot, IOT_MSG_PROPERTY_MSGID, ++msg_id);
    cJSON_AddStringToObject(pJsonRoot, IOT_MSG_PROPERTY_VERSION, "1.0");
}

cJSON * iot_mqtt_build_device_shadow_update_json(cJSON * current, uint64_t ts)
{
    cJSON * pJsonRoot = NULL;
    cJSON * pJsonData = NULL;

    pJsonRoot = cJSON_CreateObject();
    pJsonData = cJSON_CreateObject();

    if(NULL == current || NULL == pJsonRoot || NULL == pJsonData){
        printf("json_create error\n");
        return NULL;
    }

    add_base_json(pJsonRoot);

    cJSON_AddItemToObject(pJsonData, IOT_MSG_PROPERTY_SHADOW_REPORTED, current);
    cJSON_AddNumberToObject(pJsonData, IOT_MSG_PROPERTY_SHADOW_VERSION, _shadowVersion);
    cJSON_AddNumberToObject(pJsonData, IOT_MSG_PROPERTY_SHADOW_TIMESTAMP, ts);

    cJSON_AddItemToObject(pJsonRoot, IOT_MSG_PROPERTY_DATA, pJsonData);
    cJSON_AddNumberToObject(pJsonRoot, IOT_MSG_PROPERTY_TS, ts);

    return pJsonRoot;
}

cJSON * iot_mqtt_build_property_json(cJSON * data, int64_t ts)
{
    cJSON * pJsonRoot = NULL;
    pJsonRoot = cJSON_CreateObject();

    if(NULL == pJsonRoot || NULL == data){
        return NULL;
    }

    add_base_json(pJsonRoot);
    cJSON_AddItemToObject(pJsonRoot, IOT_MSG_PROPERTY_DATA, data);
    cJSON_AddNumberToObject(pJsonRoot, IOT_MSG_PROPERTY_TS, ts);

    return pJsonRoot;
}

cJSON * iot_mqtt_build_shadow_acquiring_json(cJSON * data, uint64_t ts)
{
    cJSON * pJsonRoot = NULL;
    pJsonRoot = cJSON_CreateObject();

    if(NULL == pJsonRoot || NULL == data){
        return NULL;
    }

    add_base_json(pJsonRoot);
    cJSON_AddItemToObject(pJsonRoot, IOT_MSG_PROPERTY_DATA, data);
    cJSON_AddNumberToObject(pJsonRoot, IOT_MSG_PROPERTY_TS, ts);

    return pJsonRoot;
}

char * jd_get_pub_payload(int light_switch, int action)
{
    cJSON * p_data;
    cJSON * p_payload;
    uint64_t timeofday;
    char * pbuf = NULL;
    timeofday = 1579142881000 + (uint64_t)lega_rtos_get_time();
    if(action == GET_SHADOW)
    {
        p_data = cJSON_CreateObject();
        p_payload = iot_mqtt_build_shadow_acquiring_json(p_data, timeofday);
    }
    else if(action == UPDATE_SHADOW && (light_switch == LIGHT_OFF || light_switch == LIGHT_ON))
    {
        p_data = jd_get_update_json_data(light_switch);
        p_payload = iot_mqtt_build_device_shadow_update_json(p_data, timeofday);
    }
    else if(action == UPDATE_PROPERTY && (light_switch == LIGHT_OFF || light_switch == LIGHT_ON))
    {
        p_data = jd_get_update_json_data_property(light_switch);
        p_payload = iot_mqtt_build_property_json(p_data, timeofday);
    }
    else
    {
        printf("jd_get_put_payload para un recognize\n");
        return NULL;
    }


    if(p_payload == NULL)
    {
        printf("payload is null\n");
        return NULL;
    }
    pbuf = cJSON_PrintUnformatted(p_payload);
    printf("publish payload: %s\n", pbuf);
    return pbuf;
}

static char * get_mqtt_pub_format(iot_mqtt_pub_t pub_type)
{
    char *fmt = NULL;

    switch (pub_type)
    {
        case MQTT_PUB_TYPE_PROPERTY:
            fmt = "/sys/%s/%s/thing/property/post";
            break;
        case MQTT_PUB_TYPE_EVENT:
            fmt = "/sys/%s/%s/thing/event/post";
            break;
        case MQTT_PUB_TYPE_SERVICE_REPLY:
            fmt = "/sys/%s/%s/thing/service/post_reply";
            break;
        case MQTT_PUB_TYPE_SHADOW_REPLY:
            fmt = "/sys/%s/%s/thing/shadow/down_reply";
            break;
        case MQTT_PUB_TYPE_DEVICE_UPDATE_SHADOW:
            fmt = "/sys/%s/%s/thing/shadow/post";
            break;
        case MQTT_PUB_TYPE_GET_SHADOW:
            fmt = "/sys/%s/%s/thing/shadow/get";
            break;
        default:
            break;
    }
    return fmt;
}

static int32_t iot_mqtt_get_pub_topic(char *topic_out,
                            iot_mqtt_pub_t topic_type,
                            const char * product_key,
                            const char * identifier)
{
    int res = 0;
    int topic_len = 0;
    if (product_key == NULL || identifier == NULL || topic_out == NULL) {
        return -1;
    }

    //char * buf = cJSON_PrintUnformatted(payload);
    char *fmt = get_mqtt_pub_format(topic_type);
    topic_len = strlen(fmt) + strlen(product_key) + strlen(identifier) + 1;
    snprintf(topic_out, topic_len, fmt, product_key, identifier);
    return res;
}

static char * get_mqtt_sub_format(iot_mqtt_sub_t sub_type)
{
    char *fmt = NULL;

    switch (sub_type)
    {
        case MQTT_SUB_TYPE_PROPERTY_REPLY:
            fmt = "/sys/%s/%s/thing/property/post_reply";
            break;
        case MQTT_SUB_TYPE_EVENT_REPLY:
            fmt = "/sys/%s/%s/thing/event/post_reply";
            break;
        case MQTT_SUB_TYPE_SERVICE:
            fmt = "/sys/%s/%s/thing/service/post";
            break;
        case MQTT_SUB_TYPE_SHADOW_UPDATE:
            fmt = "/sys/%s/%s/thing/shadow/down";
            break;
        case MQTT_SUB_TYPE_DEVICE_UPDATE_SHADOW_REPLY:
            fmt = "/sys/%s/%s/thing/shadow/post_reply";
            break;
        case MQTT_SUB_TYPE_GET_SHADOW_REPLY:
            fmt = "/sys/%s/%s/thing/shadow/get_reply";
            break;
        case MQTT_SUB_DYNAMIC_AUTH:
            fmt = "/sys/%s/deviceNo_%s/thing/deviceAct/post";
            break;
        default:
            break;
    }
    return fmt;
}

static int iot_mqtt_get_sub_topic(char *topic_out,
                    iot_mqtt_sub_t topic_type,
                    const char * product_key,
                    const char * identifier)
{
    int res = 0;
    int topic_len = 0;
    if (product_key == NULL || identifier == NULL || topic_out == NULL) {
        return -1;
    }
    char *fmt = get_mqtt_sub_format(topic_type);
    topic_len = strlen(fmt) + strlen(product_key) + strlen(identifier) + 1;
    snprintf(topic_out, topic_len, fmt, product_key, identifier);
    return res;
}

int jd_mqtt_get_topic(char *topic_out, int side)
{
    int ret = 0;
    char productKey[IOT_PRODUCT_KEY_LEN + 1] = {0};
    int32_t productKey_len = IOT_PRODUCT_KEY_LEN;
    char identifer[IOT_IDENTIFIER_LEN + 1] = {0};
    int32_t identifer_len = IOT_IDENTIFIER_LEN;
    if(topic_out == NULL)
        return -1;
    memset(topic_out, 0, JD_TOPIC_LEN);
    ret = lega_flash_kv_get("jd_productKey", productKey, &productKey_len);
    if(ret != KV_OK)
    {
        printf("jd productKey get error\n");
        return -1;
    }
    else
    {
        // printf("jd productKey = %s\n", productKey);
    }
    ret = lega_flash_kv_get("jd_identifer", identifer, &identifer_len);
    if(ret != KV_OK)
    {
        printf("jd identifer get error\n");
        return -1;
    }
    else
    {
        // printf("jd identifer = %s\n", identifer);
    }
    if(side == UP_SIDE)
    {
        ret = iot_mqtt_get_pub_topic(topic_out, MQTT_PUB_TYPE_DEVICE_UPDATE_SHADOW, productKey, identifer);
    }
    else if(side == DOWN_SIDE)
    {
        ret = iot_mqtt_get_sub_topic(topic_out, MQTT_SUB_TYPE_SHADOW_UPDATE, productKey, identifer);
    }
    else if(side == POST_REPLY)
    {
        ret = iot_mqtt_get_sub_topic(topic_out, MQTT_SUB_TYPE_DEVICE_UPDATE_SHADOW_REPLY, productKey, identifer);
    }
    else if(side == GET_SHADOW_RPY)
    {
        ret = iot_mqtt_get_sub_topic(topic_out, MQTT_SUB_TYPE_GET_SHADOW_REPLY, productKey, identifer);
    }
    else if(side == GET_SHADOW)
    {
        ret = iot_mqtt_get_pub_topic(topic_out, MQTT_PUB_TYPE_GET_SHADOW, productKey, identifer);
    }
    else if(side == UP_PROPERTY)
    {
        ret = iot_mqtt_get_pub_topic(topic_out, MQTT_PUB_TYPE_PROPERTY, productKey, identifer);
    }
    return ret;
}
#endif