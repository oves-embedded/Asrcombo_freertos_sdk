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

#ifdef CLOUD_TENCENT_SUPPORT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define     TENCENT_PRODUCT_ID_LEN      64
#define     TENCENT_DEVICE_NAME_LEN     64
#define     TENCENT_HOSTNAME_MAXLEN    128

#define     TENCENT_TOPIC_PUBLISH               0
#define     TENCENT_TOPIC_SUBSCRIBE_DATA        2
#define     TENCENT_TOPIC_SUBSCRIBE_CONTROL     1
#define     TENCENT_TOPIC_SUBSCRIBE_RESULT      3

#define     PAYLOAD_LIGHT_OFF           0
#define     PAYLOAD_LIGHT_ON            1
#define     PAYLOAD_GET_SHADOW          2
#define     TENCENT_GET_SHADOW          (PAYLOAD_GET_SHADOW)


extern const char tencent_ca_crt[];
extern const size_t tencent_ca_crt_len;
extern const char tencent_client_cert[];
extern const size_t tencent_client_cert_len;
extern const char tencent_client_private_key[];
extern const size_t tencent_client_private_key_len;
extern int tencent_shadowVersion;



extern int get_tencent_value();
extern int init_tencent_info();
extern int tencent_mqtt_get_topic(char *topic_out, int action_in);
extern int tencent_mqtt_get_publish_payload(char *payload_out, int action_in);

#endif