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

#ifdef CLOUD_HUAWEI_SUPPORT
#include "huawei_func.h"
#include "lega_rtos_api.h"
// #include "lega_flash.h"
#include "cJSON.h"
#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif


#include <string.h>
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/platform.h"

#include "mbedtls/md_internal.h"
//#include "dtls_interface.h"
//********************************************************************************************************/
//make a byte to 2 ascii hex
int byte2hexstr(uint8_t *bufin, int len, char *bufout)
{
    int i = 0;
    uint8_t tmp_l = 0x0;
    uint8_t tmp_h = 0;
    if ((NULL == bufin )|| (len <= 0 )||( NULL == bufout))
    {
        return -1;
    }
    for(i = 0; i < len; i++)
    {
        tmp_h = (bufin[i]>>4)&0X0F;
        tmp_l = bufin[i] &0x0F;
        bufout[2*i] = (tmp_h > 9)? (tmp_h - 10 + 'a'):(tmp_h +'0');
        bufout[2*i + 1] = (tmp_l > 9)? (tmp_l - 10 + 'a'):(tmp_l +'0');
    }
    bufout[2*len] = '\0';

    return 0;
}

int osal_strdup(char *out, const char *ch)
{
    size_t length;
    if(NULL == ch)
        return -1;
    length = strlen(ch);
    strncpy(out, ch, length);
    return 0;
}

int clientid_fmt(char *out, char *fmt, char *id, char *salt_time)
{
    int len =  0;
    if(out == NULL || fmt == NULL || id == NULL || salt_time == NULL)
        return -1;
    len = strlen(fmt) + strlen(id) + strlen(salt_time) + 1;
    snprintf(out,len,fmt,id,salt_time);
    return 0;
}

int mbedtls_hmac_calc(mbedtls_hmac_t *hmac_info)
{
    int ret;
    mbedtls_md_context_t mbedtls_md_ctx;
    const mbedtls_md_info_t *md_info;

    if (hmac_info == NULL || hmac_info->secret == NULL || hmac_info->input == NULL
        || hmac_info->secret_len <= 0 || hmac_info->input_len <= 0 || hmac_info->digest_len <= 0)
    {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    md_info = mbedtls_md_info_from_type(hmac_info->hmac_type);
    if (md_info == NULL || md_info->size > hmac_info->digest_len)
    {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    mbedtls_md_init(&mbedtls_md_ctx);
    memset(hmac_info->digest, 0x00, hmac_info->digest_len);

    ret = mbedtls_md_setup(&mbedtls_md_ctx, md_info, 1);
    if (ret != 0)
    {
        printf("mbedtls_md_setup() returned -0x%04x\n", -ret);
        goto exit;
    }

    (void)mbedtls_md_hmac_starts(&mbedtls_md_ctx, hmac_info->secret, hmac_info->secret_len);
    (void)mbedtls_md_hmac_update(&mbedtls_md_ctx, hmac_info->input, hmac_info->input_len);
    (void)mbedtls_md_hmac_finish(&mbedtls_md_ctx, hmac_info->digest);

    exit:
    mbedtls_md_free(&mbedtls_md_ctx);

    return ret;
}

int hmac_generate_passwd(char *content, int contentlen,char *key,int keylen, unsigned char *buf,int buflen)
{
    int ret = -1;
    mbedtls_hmac_t hmac;
    hmac.secret = (uint8_t *)key;
    hmac.secret_len = keylen;
    hmac.input = (unsigned char *)content;
    hmac.input_len = contentlen;
    hmac.digest =(unsigned char *) buf;
    hmac.digest_len = buflen;
    hmac.hmac_type = MBEDTLS_MD_SHA256;

    ret = mbedtls_hmac_calc(&hmac);

    return ret;
}
int mqtt_connect_para_gernerate(mqtt_connect_para_t *para, huawei_tuple_t *tuple)
{
    uint8_t hmac[CN_HMAC_LEN] = {0};
    char salt_time[16] = {0};
    snprintf(salt_time, 11, "%04d%02d%02d%02d", 2020, 03, 10, 20);

    if(0 != clientid_fmt(para->clientid, CN_CLIENT_ID_FMT, tuple->deviceID, salt_time))
    {
        return -1;
    }

    if(0 != osal_strdup(para->username, tuple->deviceID))
    {
        return -1;
    }

    hmac_generate_passwd(tuple->deviceSecret, strlen(tuple->deviceSecret), salt_time, strlen(salt_time), (unsigned char *)hmac, sizeof(hmac));
    if(NULL != para->password)
    {
        byte2hexstr(hmac, CN_HMAC_LEN, para->password);
    }
    else
    {
        return -1;
    }
    return 0;
}
//********************************************************************************************************/
const char huawei_ca_crt[] = \
{
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIID4DCCAsigAwIBAgIJAK97nNS67HRvMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV\r\n" \
    "BAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UEBxMCU1oxDzANBgNVBAoTBkh1YXdl\r\n" \
    "aTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lPVDAeFw0xNjA1MDQxMjE3MjdaFw0y\r\n" \
    "NjA1MDIxMjE3MjdaMFMxCzAJBgNVBAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UE\r\n" \
    "BxMCU1oxDzANBgNVBAoTBkh1YXdlaTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lP\r\n" \
    "VDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJxM9fwkwvxeILpkvoAM\r\n" \
    "Gdqq3x0G9o445F6Shg3I0xmmzu9Of8wYuW3c4jtQ/6zscuIGyWf06ke1z//AVZ/o\r\n" \
    "dp8LkuFbBbDXR5swjUJ6z15b6yaYH614Ty/d6DrCM+RaU+FWmxmOon9W/VELu2BB\r\n" \
    "NXDQHJBSbWrLNGnZA2erk4JSMp7RhHrZ0QaNtT4HhIczFYtQ2lYF+sQJpQMrjoRn\r\n" \
    "dSV9WB872Ja4DgcISU1+wuWLmS/NKjIvOWW1upS79yu2I4Rxos2mFy9xxz311rGC\r\n" \
    "Z3X65ejFNzCUrNgf6NEP1N7wB9hUu7u50aA+/56D7EgjeI0gpFytC+a4f6JCPVWI\r\n" \
    "Lr0CAwEAAaOBtjCBszAdBgNVHQ4EFgQUcGqy59oawLEgMl21//7F5RyABpwwgYMG\r\n" \
    "A1UdIwR8MHqAFHBqsufaGsCxIDJdtf/+xeUcgAacoVekVTBTMQswCQYDVQQGEwJD\r\n" \
    "TjELMAkGA1UECBMCR0QxCzAJBgNVBAcTAlNaMQ8wDQYDVQQKEwZIdWF3ZWkxCzAJ\r\n" \
    "BgNVBAsTAkNOMQwwCgYDVQQDEwNJT1SCCQCve5zUuux0bzAMBgNVHRMEBTADAQH/\r\n" \
    "MA0GCSqGSIb3DQEBCwUAA4IBAQBgv2PQn66gRMbGJMSYS48GIFqpCo783TUTePNS\r\n" \
    "tV8G1MIiQCpYNdk2wNw/iFjoLRkdx4va6jgceht5iX6SdjpoQF7y5qVDVrScQmsP\r\n" \
    "U95IFcOkZJCNtOpUXdT+a3N+NlpxiScyIOtSrQnDFixWMCJQwEfg8j74qO96UvDA\r\n" \
    "FuTCocOouER3ZZjQ8MEsMMquNEvMHJkMRX11L5Rxo1pc6J/EMWW5scK2rC0Hg91a\r\n" \
    "Lod6aezh2K7KleC0V5ZlIuEvFoBc7bCwcBSAKA3BnQveJ8nEu9pbuBsVAjHOroVb\r\n" \
    "8/bL5retJigmAN2GIyFv39TFXIySw+lW0wlp+iSPxO9s9J+t\r\n" \
    "-----END CERTIFICATE-----"
};

int huawei_shadowVersion = 0;
huawei_tuple_t tuple;
int init_huawei_info()
{
    int ret = -1;
    char deviceID[HUAWEI_DEVICE_ID_LEN + 1] = {0};
    int32_t deviceID_len = HUAWEI_DEVICE_ID_LEN;
    char deviceSecret[HUAWEI_DEVICE_SECRET_LEN + 1] = {0};
    int32_t deviceSecret_len = HUAWEI_DEVICE_SECRET_LEN;
#ifdef CFG_PLF_DUET
    ret = duet_flash_kv_get("huawei_deviceID", deviceID, &deviceID_len);
#else
    ret = lega_flash_kv_get("huawei_deviceID", deviceID, &deviceID_len);
#endif
    if(ret != 0)
    {
        printf("huawei deviceID get error\n");
        return ret;
    }
    else
    {
        printf("huawei deviceID = %s\n", deviceID);
    }
#ifdef CFG_PLF_DUET
    ret = duet_flash_kv_get("huawei_deviceSecret", deviceSecret, &deviceSecret_len);
#else
    ret = lega_flash_kv_get("huawei_deviceSecret", deviceSecret, &deviceSecret_len);
#endif
    if(ret != 0)
    {
        printf("huawei deviceSecret get error\n");
        return ret;
    }
    else
    {
        printf("huawei deviceSecret = %s\n", deviceSecret);
    }
    memset(&tuple, 0, sizeof(huawei_tuple_t));
    strncpy(tuple.deviceID, deviceID, deviceID_len);
    strncpy(tuple.deviceSecret, deviceSecret, deviceSecret_len);
    return 0;
}

int huawei_mqtt_get_topic(char *topic_out, int action_in)
{
    char *ptmp_buf = NULL;
    int topic_len = 0;
    if(topic_out == NULL)
    {
        printf("topic_out is NULL\n");
        return -1;
    }
    if(action_in == HUAWEI_TOPIC_PUBLISH)
    {
        ptmp_buf = "/huawei/v1/devices/%s/data/json";
    }
    else if(action_in == HUAWEI_TOPIC_SUBSCRIBE_RESULT)
    {
        ptmp_buf = "$shadow/operation/result/%s/%s";
    }
    else if(action_in == HUAWEI_TOPIC_SUBSCRIBE_CONTROL)
    {
        ptmp_buf = "/huawei/v1/devices/%s/command/json";
    }
    else if(action_in == HUAWEI_TOPIC_SUBSCRIBE_DATA)
    {
        ptmp_buf = "%s/%s/data";
    }
    else
    {
        printf("topic input error\n");
        return -1;
    }
    topic_len = strlen(ptmp_buf) + strlen(tuple.deviceID) + 1;
    snprintf(topic_out, topic_len, ptmp_buf, tuple.deviceID);
    printf("topic:%s\n", topic_out);
    return 0;
}

int huawei_mqtt_get_publish_payload(char *payload_out, int action_in)
{
    char *ptmp = NULL;
    cJSON * pJsonData = cJSON_CreateObject();
    cJSON * pJsonArray = cJSON_CreateArray();
    cJSON * pJsonArrayData = cJSON_CreateObject();
    cJSON * pJsonServiceData = cJSON_CreateObject();
    if(pJsonData == NULL || pJsonArray == NULL || pJsonArrayData == NULL || pJsonServiceData == NULL)
    {
        printf("json creat error\n");
        return -1;
    }
    if(payload_out == NULL)
    {
        printf("payload_out is NULL\n");
        return -1;
    }
    if(action_in == HUAWEI_PAYLOAD_GET_SHADOW)
    {
    }
    else if(action_in == HUAWEI_PAYLOAD_LIGHT_OFF)
    {
        cJSON_AddStringToObject(pJsonData, "msgType", "deviceReq");
        cJSON_AddItemToObject(pJsonData, "data", pJsonArray);
        cJSON_AddItemToArray(pJsonArray, pJsonArrayData);
        cJSON_AddStringToObject(pJsonArrayData, "serviceId", "Turn");
        cJSON_AddItemToObject(pJsonArrayData, "serviceData", pJsonServiceData);
        cJSON_AddStringToObject(pJsonServiceData, "switchone", "False");
    }
    else if(action_in == HUAWEI_PAYLOAD_LIGHT_ON)
    {
        cJSON_AddStringToObject(pJsonData, "msgType", "deviceReq");
        cJSON_AddItemToObject(pJsonData, "data", pJsonArray);
        cJSON_AddItemToArray(pJsonArray, pJsonArrayData);
        cJSON_AddStringToObject(pJsonArrayData, "serviceId", "Turn");
        cJSON_AddItemToObject(pJsonArrayData, "serviceData", pJsonServiceData);
        cJSON_AddStringToObject(pJsonServiceData, "switchone", "True");
    }
    else
    {
        printf("get publish payload input error\n");
        cJSON_Delete(pJsonData);
        cJSON_Delete(pJsonArray);
        cJSON_Delete(pJsonArrayData);
        cJSON_Delete(pJsonServiceData);
        return -1;
    }
    ptmp = cJSON_PrintUnformatted(pJsonData);
    strcpy(payload_out, ptmp);
    printf("payload:%s\n", payload_out);
    lega_rtos_free(ptmp);
    if(pJsonData != NULL)
        cJSON_Delete(pJsonData);
    return 0;
}
#endif