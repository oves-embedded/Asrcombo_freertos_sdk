
/**
 ****************************************************************************************
 *
 * @file (app_utils.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */

#ifndef _APP_UTILS_H
#define _APP_UTILS_H

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <stddef.h>    // standard definitions
#include <stdint.h>    // standard integer definition
#include <stdbool.h>   // boolean definition
#include <stdio.h>
#include <string.h>
/*
 * MACRO DEFINES
 ****************************************************************************************
 */


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

uint8_t char2HexValue(char ch);

uint32_t string2HexValue(uint8_t * inStr, uint8_t len);

void hexValue2Char(uint8_t value, uint8_t *chars);

uint8_t char2Caption(uint8_t ch);

bool isHexChar(uint8_t ch);

bool isHexValue(const uint8_t * value);

bool isDecChar(uint8_t ch);

uint8_t findChar(const uint8_t *source, uint8_t target, uint8_t start, uint8_t end);

uint8_t macValue2Char(uint8_t *mac, uint8_t *chars, bool colon);

void macChar2Value(uint8_t *chars, uint8_t *mac);

void macReverse(uint8_t * mac);

bool addrCompare(uint8_t * addr1, bool  littleEnding1, uint8_t * addr2, bool littleEnding2);

void uuidChar2Value(uint8_t *chars, uint8_t *uuid, bool len16);

void keyChar2Value(uint8_t *chars, uint8_t * uuid, uint8_t keyLen);

void addrCopy(uint8_t * target, uint8_t * source);

void gapKeyCopy(uint8_t * target, uint8_t * source);

bool addrValid(uint8_t* addr);
bool searchChar(uint8_t* string, uint8_t length, uint8_t target);
int16_t string2Value(uint8_t * valueStr, uint8_t length);
int32_t string2ValueLong(uint8_t * valueStr, uint8_t length);
uint32_t string2ValueU32(uint8_t * valueStr, uint8_t length);

void app_util_set_bt_address(uint8_t * bd_addr,uint8_t length);

uint8_t * app_util_get_bt_address();

uint32_t regRead(uint32_t addr, bool log);

void util_reg_read(uint32_t ul_reg);

void util_reg_write(uint32_t ul_reg, uint32_t value);

#endif //BLE_CODE_APP_UTILS_H
