/*******************************************************************************
 *   Copyright 2019 JDCLOUD.COM
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *******************************************************************************/

#ifdef CLOUD_JD_SUPPORT
#include <stdio.h>
#include <string.h>

#include "jd_auth.h"
#include "mbedtls/md.h"
//#include "iot_log.h"
//#include "hal_wrappers.h"

#define TCP_PORT 1883
#define TLS_PORT 8883

/*MQTT connect hostname  */
#ifndef INDEPENDENT_MODE
#define IOT_MQTT_HOSTNAME  "iothub-cn1-778891-master-nlb-FI.jvessel-open-hb.jdcloud.com"
#endif

static void _hex2str(uint8_t *input, uint16_t input_len, char *output)
{
    char *zEncode = "0123456789ABCDEF";
    int i = 0, j = 0;

    for (i = 0; i < input_len; i++) {
        output[j++] = zEncode[(input[i] >> 4) & 0xf];
        output[j++] = zEncode[(input[i]) & 0xf];
    }
}

static int32_t _iot_get_username(const char * productKey, const char * device_id, char * username)
{
    memset(username, 0, DEV_AUTH_USERNAME_MAXLEN);
    memcpy(username, productKey, strlen(productKey));
    memcpy(username + strlen(username), "_", strlen("_"));
    memcpy(username + strlen(username), device_id, strlen(device_id));

    return SUCCESS_RETURN;
}

#ifdef INDEPENDENT_MODE
static int32_t _iot_get_host_name(char * hostname)
{
    int len = hal_get_mqtt_host(hostname);

    if(len == 0)
    {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#else
static int32_t _iot_get_host_name(char * hostname)
{
    memset(hostname, 0, DEV_AUTH_HOSTNAME_MAXLEN);
    memcpy(hostname, IOT_MQTT_HOSTNAME, strlen(IOT_MQTT_HOSTNAME));

    return SUCCESS_RETURN;
}
#endif

#if defined(DYNAMIC_REGISTER) || defined(DEVICE_GATEWAY)
static int32_t _iot_get_dynamic_reg_username(const char * productKey, const char * deviceName, char * username)
{
    memset(username, 0, DEV_AUTH_USERNAME_MAXLEN);
    memcpy(username, productKey, strlen(productKey));
    memcpy(username + strlen(username), "_deviceNo_", strlen("_deviceNo_"));
    memcpy(username + strlen(username), deviceName, strlen(deviceName));

    return SUCCESS_RETURN;
}
#endif // DYNAMIC_REGISTER

static int32_t _iot_get_clientid(const char * username, const char * devicename, char * clientid)
{
    memset(clientid, 0, DEV_AUTH_CLIENT_ID_MAXLEN);
    memcpy(clientid, username, strlen(username));
    memcpy(clientid + strlen(clientid), "_", strlen("_"));
    memcpy(clientid + strlen(clientid), devicename, strlen(devicename));

    return SUCCESS_RETURN;
}

static int32_t _iot_hmac_sha1(const char * key, const char * content, char * output)
{
    const mbedtls_md_info_t * pInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_hmac(pInfo, (const unsigned char *)key, 32, (const unsigned char *)content, strlen(content), (unsigned char *)output);

    return SUCCESS_RETURN;
}

static int32_t _iot_get_pwd(const char * key, const char * content, char * pwd)
{
    char msg[DEV_AUTH_DEVICE_NAME_MAXLEN] = {"deviceName"};
    strcat(msg, content);

    char sign_hex[DEV_AUTH_PASSWORD_BYTE_LEN] = {0};
    _iot_hmac_sha1(key, msg, sign_hex);
    _hex2str((uint8_t*)sign_hex, DEV_AUTH_PASSWORD_BYTE_LEN, pwd);

    return SUCCESS_RETURN;
}

int32_t iot_get_auth_info(jd_dev_meta_info_t *meta, jd_mqtt_auth_info_t *auth)
{
    if(NULL == meta || NULL == auth) {
        printf("invalid meta info point or auth info point");
        return FAIL_RETURN;
    }

    uint16_t length = 0;
    int res;

    memset(auth, 0, sizeof(jd_mqtt_auth_info_t));

    /* setup username */
    length = strlen(meta->device_name) + strlen(meta->product_key) + 2;
    if (length >= DEV_AUTH_USERNAME_MAXLEN) {
        return ERROR_DEV_AUTH_USERNAME_TOO_SHORT;
    }

    if (_iot_get_username(meta->product_key, meta->device_id, auth->username) != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* setup clientid */
    if (_iot_get_clientid(auth->username, meta->device_name, auth->clientid) != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* setup password */
    res = _iot_get_pwd(meta->device_secret, auth->username, auth->password);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    /* setup port */
#ifdef SUPPORT_TLS
    auth->port = TLS_PORT;
#else
    auth->port = TCP_PORT;
#endif /* SUPPORT_TLS */

    /* setup host */
    if(_iot_get_host_name(auth->hostname) == FAIL_RETURN)
    {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

// #if defined(DYNAMIC_REGISTER) || defined(DEVICE_GATEWAY)
// int32_t iot_get_dynamic_auth_info(jd_dev_meta_info_t *meta, jd_mqtt_auth_info_t *auth)
// {
//     if(NULL == meta || NULL == auth) {
//         LOG_ERROR("invalid meta info point or auth info point");
//         return FAIL_RETURN;
//     }

//     uint16_t length = 0;
//     int res;

//     memset(auth, 0, sizeof(iot_mqtt_auth_info_t));

//     /* setup username */
//     length = strlen(meta->device_name) + strlen(meta->product_key) + 2;
//     if (length >= DEV_AUTH_USERNAME_MAXLEN) {
//         return ERROR_DEV_AUTH_USERNAME_TOO_SHORT;
//     }

//     /* setup user name */
//     if (_iot_get_dynamic_reg_username(meta->product_key, meta->device_name, auth->username) != SUCCESS_RETURN) {
//         return FAIL_RETURN;
//     }

//     /* setup clientid */
//     if (_iot_get_clientid(auth->username, meta->device_name, auth->clientid) != SUCCESS_RETURN) {
//         return FAIL_RETURN;
//     }

//     /* setup password */
//     res = _iot_get_pwd(meta->product_secret, auth->username, auth->password);
//     if (res < SUCCESS_RETURN) {
//         return res;
//     }

//     /* setup port */
// #ifdef SUPPORT_TLS
//     auth->port = TLS_PORT;
// #else
//     auth->port = TCP_PORT;
// #endif /* SUPPORT_TLS */

//     /* setup host */
//     if(_iot_get_host_name(auth->hostname) == FAIL_RETURN)
//     {
//         return FAIL_RETURN;
//     }

//     return SUCCESS_RETURN;
// }

// #endif // DYNAMIC_REGISTER
#endif