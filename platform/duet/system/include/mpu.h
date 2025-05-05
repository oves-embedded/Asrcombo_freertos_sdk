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


#ifndef _MPU_H_
#define _MPU_H_

#include "duet_cm4.h"

typedef enum {
    MEM_ATTR_NON_CACHEABLE,
    MEM_ATTR_MEMORY_WT,
    MEM_ATTR_MEMORY_WBWA,
    MEM_ATTR_DEVICE_nGnRE,
    MEM_ATTR_DEVICE_nGnRnE
} MEM_ATTR;

void mpu_init(void);
void mpu_deinit(void);

#endif // _MPU_H_

