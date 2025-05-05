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

#ifdef CLOUD_AWS_SUPPORT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define     AWS_PRODUCT_ID_LEN      64
#define     AWS_CLIENT_ID_LEN       64
#define     AWS_HOSTNAME_MAXLEN     128

#define     AWS_TOPIC_PUBLISH               0
#define     AWS_TOPIC_SUBSCRIBE             1

#define     AWS_PAYLOAD_LIGHT_OFF           0
#define     AWS_PAYLOAD_LIGHT_ON            1



extern const char aws_ca_crt[];
extern const size_t aws_ca_crt_len;
extern const char aws_client_cert[];
extern const size_t aws_client_cert_len;
extern const char aws_client_private_key[];
extern const size_t aws_client_private_key_len;
extern int aws_shadowVersion;



extern int get_aws_value();
extern int init_aws_info();
extern int aws_mqtt_get_topic(char *topic_out, int action_in);
extern int aws_mqtt_get_publish_payload(char *payload_out, int action_in);

#endif