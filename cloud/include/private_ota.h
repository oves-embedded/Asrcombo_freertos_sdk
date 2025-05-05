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

#ifndef _PRIVATE_OTA_H_
#define _PRIVATE_OTA_H_

#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define     OTA_TASK_NAME              "ota_task"
#define     OTA_TASK_PRIORITY          3
#define     OTA_STACK_SIZE             4096

typedef enum {
    PRIVATE_OTA_INIT = 0,
    PRIVATE_OTA_START,
    PRIVATE_OTA_LEN_PASS,
    PRIVATE_OTA_MD5_PASS,
    PRIVATE_OTA_FAILED
} private_ota_step_t;

void private_ota_entry();

#endif

#endif