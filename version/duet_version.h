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
#ifndef _DUET_VERSION_H_
#define _DUET_VERSION_H_

//ASR release version
#if defined(CFG_DUET_5822T)
#define LEGA_VERSION_PREIFX     "5822T-APP-"
#elif defined(CFG_DUET_5822S)
#define LEGA_VERSION_PREIFX     "5822S-APP-"
#else
#define LEGA_VERSION_PREIFX     "5822NC-APP-"
#endif

#define LEGA_VERSION_STR        LEGA_VERSION_PREIFX"V1.9.3"

#endif  //_DUET_VERSION_H_
