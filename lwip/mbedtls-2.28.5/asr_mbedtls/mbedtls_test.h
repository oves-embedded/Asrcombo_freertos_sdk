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

#ifndef MBEDTLS_TEST_H
#define MBEDTLS_TEST_H

int ecdh_hal_efficiency_test(void);
int ecc_hal_efficiency_test(void);
int rsa_hal_efficiency_test(int pcks_type);
int aes_hal_efficiency_test(void);

#endif /*MBEDTLS_TEST_H*/