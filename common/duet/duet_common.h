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

#ifndef _DUET_COMM_H_
#define _DUET_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DUET_STR_TO_INT_ERR  0xFFFFFFFF
#define LEGA_STR_TO_INT_ERR  DUET_STR_TO_INT_ERR

void jumpToApp(int addr);
void duet_memset(char *buf,int value, int size);
void duet_memcpy(char *dst, char *src, int size);
void delay(unsigned int cycles);
void udelay_pl(unsigned int us);
int convert_str_to_int(char *str);
void convert_int_to_str(unsigned int val, unsigned int type, char *ch);

#define OTA_MAX_SIZE_SEG __attribute__((section("ota_max_size_sec")))
#define APP_CERT_SEG __attribute__((section("app_cert_sec")))

#ifdef CFG_HIGHFREQ_VER
#define FLASH_COMMON_SEG __attribute__((section("CODE_DTCM")))
#else
#define FLASH_COMMON_SEG
#endif

#ifdef CFG_DUET_FREERTOS
#define FLASH_COMMON2_SEG __attribute__((section("seg_wf_flash_driver")))
#else
#define FLASH_COMMON2_SEG
#endif
/// Macro to read a register
#define REG_RD(addr)              (*(volatile uint32_t *)(addr))
/// Macro to write a register
#define REG_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

/// Macro to read a register
#define REG_BLE_RD(addr)              (*(volatile uint32_t *)(addr))
/// Macro to write a register
#define REG_BLE_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

void duet_write32_bit(uint32_t reg, uint8_t start_bit, uint8_t len, uint32_t src_val);
uint32_t duet_read32_bit(uint32_t reg, uint8_t start_bit, uint8_t len);

#ifdef CFG_DUET_FREERTOS
void lega_enter_critical_expble(void);
void lega_exit_critical_expble(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //_DUET_COMM_H_
