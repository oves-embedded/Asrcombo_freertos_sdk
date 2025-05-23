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

#ifndef __PANIC_C_H__
#define __PANIC_C_H__

#include <stdio.h>

#ifdef DEBUG_BACKTRACE

#define BACKTRACE_JUMP    3
#define BACKTRACE_DEPTH   256
void debug_backtrace_now();
void debug_backtrace_exception();

#endif /* DEBUG_BACKTRACE */

#endif //__PANIC_C_H__
