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
#include "mbedtls/md.h"
#include "cloud_common.h"

#define     HUAWEI_DEVICE_ID_LEN      64
#define     HUAWEI_DEVICE_SECRET_LEN     64
#define     HUAWEI_HOSTNAME_MAXLEN    128

#define     HUAWEI_TOPIC_PUBLISH               0
#define     HUAWEI_TOPIC_SUBSCRIBE_DATA        2
#define     HUAWEI_TOPIC_SUBSCRIBE_CONTROL     1
#define     HUAWEI_TOPIC_SUBSCRIBE_RESULT      3

#define     HUAWEI_PAYLOAD_LIGHT_OFF           0
#define     HUAWEI_PAYLOAD_LIGHT_ON            1
#define     HUAWEI_PAYLOAD_GET_SHADOW          2
#define     HUAWEI_GET_SHADOW                  (HUAWEI_PAYLOAD_GET_SHADOW)

#define CN_CON_BACKOFF_TIME     (1000)   ///< UNIT:ms
#define CN_CON_BACKOFF_MAXTIMES  20      ///< WHEN WAIT OR GET ADDRESS ,WE COULD TRY MAX TIMES
#define CN_HMAC_LEN              32
#define CN_STRING_MAXLEN         127
#define CN_CLIENT_ID_FMT                    "%s_0_0_%s"

typedef struct _mbedtls_hmac_t
{
    const unsigned char *secret;
    const unsigned char *input;
    unsigned char *digest;
    size_t secret_len;
    size_t input_len;
    size_t digest_len;
    mbedtls_md_type_t hmac_type;
}mbedtls_hmac_t;

typedef struct _huawei_tuple_t
{
    char deviceID[64];
    char deviceSecret[64];
}huawei_tuple_t;

extern const char huawei_ca_crt[];
extern const size_t huawei_ca_crt_len;
extern const char huawei_client_cert[];
extern const size_t huawei_client_cert_len;
extern const char huawei_client_private_key[];
extern const size_t huawei_client_private_key_len;
extern int huawei_shadowVersion;
extern huawei_tuple_t tuple;

extern int init_huawei_info();
extern int get_huawei_value();
extern int huawei_mqtt_get_topic(char *topic_out, int action_in);
extern int huawei_mqtt_get_publish_payload(char *payload_out, int action_in);
extern int mqtt_connect_para_gernerate(mqtt_connect_para_t *para, huawei_tuple_t *tuple);

#endif