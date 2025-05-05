
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


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_utils.h"
#include "stdlib.h"
#include "sonata_utils_api.h"
#include "sonata_gap.h"
#include "sonata_config.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern struct bd_addr co_default_bdaddr;
const uint8_t null_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
static void swap(uint8_t *p1, uint8_t *p2)
{
    uint8_t temp;
    temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

//Exp:3=3; F=16
uint8_t char2HexValue(char ch)
{
    uint8_t result = 0;

    if (ch >= '0' && ch <= '9')
    {
        result = ch - '0';
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        result = (ch - 'a') + 10;
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        result = (ch - 'A') + 10;
    }
    else
    {
        result = -1;
    }
    return result;
}
//convert 0XFFFF or FFFF string to value
uint32_t string2HexValue(uint8_t * inStr, uint8_t len)
{
    if ((len % 2 != 0))
    {
        return 0;
    }
    uint32_t value = 0;
    uint8_t baseIndex = 0;
    uint8_t valueLen = len;
    if (isHexValue(inStr)) //inSrt start with 0X
    {
        baseIndex = 2;
        valueLen -= 2;
    }
    for (int i = 0; i < valueLen / 2; ++i)
    {
        uint8_t tempValue = char2HexValue((char) inStr[i * 2 + baseIndex]) * 16 + char2HexValue((char) inStr[i * 2 + baseIndex + 1]);
        value = (value << (8 * i)) + tempValue;
    }
    return value;
}

uint16_t string2HexValue_2Byte(uint8_t * inStr)
{
    uint16_t value = 0;
    uint8_t d0, d1;
    if (isHexValue(inStr)) //Convert "0xFFFF" string
    {
        d0 = char2HexValue((char)inStr[2]) * 16 + char2HexValue((char)inStr[3]);
        d1 = char2HexValue((char)inStr[4]) * 16 + char2HexValue((char)inStr[5]);
        value = (d0 << 8) + d1;
    }
    else  //Convert "FFFF" string
    {
        d0 = char2HexValue((char)inStr[0]) * 16 + char2HexValue((char)inStr[1]);
        d1 = char2HexValue((char)inStr[2]) * 16 + char2HexValue((char)inStr[3]);
        value = (d0 << 8) + d1;
    }
    return value;
}

//Exp: 0X1F==> '1','F'
void hexValue2Char(uint8_t value, uint8_t *chars)
{
    uint8_t d1 = value & 0xF;
    uint8_t d2 = value >> 4;
    chars[0] = d2 > 9 ? 'A' + d2 - 10 : '0' + d2;
    chars[1] = d1 > 9 ? 'A' + d1 - 10 : '0' + d1;
}

uint8_t char2Caption(uint8_t ch)
{
    if (ch >= 'a' && ch <= 'z')
    {
        return ch - 32;
    }
    return ch;

}

bool isHexChar(uint8_t ch)
{
    if ((ch >= '0' && ch <= '9')
        || (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f'))
    {
        return true;
    }
    return false;
}

bool isHexValue(const uint8_t * value)
{
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
    {
        return true;
    }
    return false;
}

bool isDecChar(uint8_t ch)
{
    if ((ch >= '0' && ch <= '9'))
    {
        return true;
    }
    return false;
}

//Find a char in a string , return position
uint8_t findChar(const uint8_t *source, uint8_t target, uint8_t start, uint8_t end)
{
    for (int i = start; i < end; ++i)
    {
        if (source[i] == target)
        {
            return i;
        }
    }
    return 0;
}

//Find a char in a string , return exist or not
bool searchChar(uint8_t* string, uint8_t length, uint8_t target)
{
    if (string == NULL)
    {
        return false;
    }
    for (int i = 0; i < length; ++i)
    {
        if (string[i] == target)
        {
            return true;
        }
    }
    return false;
}

int16_t string2Value(uint8_t * valueStr, uint8_t length)
{
    for (int i = 1; i < length; ++i)
    {
        if(isDecChar(valueStr[i]) == false)
        {
            return 0;
        }
    }
    return (int16_t)atoi((const char *)valueStr);
}

int32_t string2ValueLong(uint8_t * valueStr, uint8_t length)
{
    for (int i = 1; i < length; ++i)
    {
        if(isDecChar(valueStr[i]) == false)
        {
            return 0;
        }
    }
    return (int32_t)atol((const char *)valueStr);
}

uint32_t string2ValueU32(uint8_t * valueStr, uint8_t length)
{
    for (int i = 1; i < length; ++i)
    {
        if(isDecChar(valueStr[i]) == false && isHexChar(valueStr[i]) == false)
        {
            return 0;
        }
    }
    return strtoul((char *)valueStr, 0, 16);
}


uint8_t macValue2Char(uint8_t *mac, uint8_t *chars, bool colon)
{
    if (colon)
    {
        hexValue2Char(mac[5], &chars[0]);
        chars[2] = ':';
        hexValue2Char(mac[4], &chars[3]);
        chars[5] = ':';
        hexValue2Char(mac[3], &chars[6]);
        chars[8] = ':';
        hexValue2Char(mac[2], &chars[9]);
        chars[11] = ':';
        hexValue2Char(mac[1], &chars[12]);
        chars[14] = ':';
        hexValue2Char(mac[0], &chars[15]);
        return 17;
    }
    else
    {
        hexValue2Char(mac[5], &chars[0]);
        hexValue2Char(mac[4], &chars[2]);
        hexValue2Char(mac[3], &chars[4]);
        hexValue2Char(mac[2], &chars[6]);
        hexValue2Char(mac[1], &chars[8]);
        hexValue2Char(mac[0], &chars[10]);
        return 12;
    }
}

void macChar2Value(uint8_t *chars, uint8_t *mac)
{
    bool colon = searchChar(chars, 9, ':');
    if (colon)
    {
        mac[5] = char2HexValue(chars[0]) * 16 + char2HexValue(chars[1]);
        mac[4] = char2HexValue(chars[3]) * 16 + char2HexValue(chars[4]);
        mac[3] = char2HexValue(chars[6]) * 16 + char2HexValue(chars[7]);
        mac[2] = char2HexValue(chars[9]) * 16 + char2HexValue(chars[10]);
        mac[1] = char2HexValue(chars[12]) * 16 + char2HexValue(chars[13]);
        mac[0] = char2HexValue(chars[15]) * 16 + char2HexValue(chars[16]);
    }
    else
    {
        mac[5] = char2HexValue(chars[0]) * 16 + char2HexValue(chars[1]);
        mac[4] = char2HexValue(chars[2]) * 16 + char2HexValue(chars[3]);
        mac[3] = char2HexValue(chars[4]) * 16 + char2HexValue(chars[5]);
        mac[2] = char2HexValue(chars[6]) * 16 + char2HexValue(chars[7]);
        mac[1] = char2HexValue(chars[8]) * 16 + char2HexValue(chars[9]);
        mac[0] = char2HexValue(chars[10]) * 16 + char2HexValue(chars[11]);
    }
}

bool addrCompare(uint8_t * addr1, bool  littleEnding1, uint8_t * addr2, bool littleEnding2)
{
    if (addr1 == NULL || addr2 == NULL)
    {
        return false;
    }
    if (littleEnding1 == littleEnding2)
    {
        return (strncmp((char *)addr1, (char *)addr2, 6) == 0);
    }
    else
    {
        uint8_t temp[6] = {0};
        memcpy(temp, addr1, 6);
        macReverse(temp);
        return (strncmp((char *)addr1, (char *)addr2, 6) == 0);
    }
}

//The mac shows in phones,  should reverse.
void macReverse(uint8_t * mac)
{
    swap(&mac[0], &mac[5]);
    swap(&mac[1], &mac[4]);
    swap(&mac[2], &mac[3]);
}

void uuidChar2Value(uint8_t *chars, uint8_t * uuid, bool len16)
{
    uint8_t d0, d1;
    if (len16)
    {
        d0 = char2HexValue(chars[0]) * 16 + char2HexValue(chars[1]);
        d1 = char2HexValue(chars[2]) * 16 + char2HexValue(chars[3]);
        uuid[0] = d1;
        uuid[1] = d0;
    }
}

void keyChar2Value(uint8_t *chars, uint8_t * uuid, uint8_t keyLen)
{
    uint8_t point = 0;
    for (int i = 0; i < keyLen; ++i)
    {
        uuid[i] = char2HexValue(chars[point ]) * 16 + char2HexValue(chars[point  + 1]);
        point += 2;
    }
}


void addrCopy(uint8_t * target, uint8_t * source)
{
    if (target == NULL || source == NULL)
    {
        return;
    }
    memcpy(target, source, SONATA_GAP_BD_ADDR_LEN);
}

void gapKeyCopy(uint8_t * target, uint8_t * source)
{
    if (target == NULL || source == NULL)
    {
        return;
    }
    memcpy(target, source, SONATA_GAP_KEY_LEN);
}
bool addrValid(uint8_t* addr)
{
    if (memcmp(addr, null_addr, sizeof(null_addr)) == 0)
    {
        return false;
    }
    return true;
}



uint32_t regRead(uint32_t addr, bool log)
{
    #define APP_REG_RD(addr)              (*(volatile uint32_t *)(addr))
    uint32_t value = 0;
    value = APP_REG_RD(addr);
    if (log)
    {
        printf("   :%08X = 0x%08X \r\n", (unsigned int)(addr), (unsigned int)value);
    }
    return value;
}

void util_reg_read(uint32_t ul_reg)
{
    uint32_t value = *(uint32_t *) (ul_reg);
    printf("Reg=0x%08X, Read Value= 0x%08X\r\n", (unsigned int) ul_reg, (unsigned int) value);
}

void util_reg_write(uint32_t ul_reg, uint32_t value)
{
    *(uint32_t *) (ul_reg) = value;
    printf("Reg=0x%08X, Set  Value= 0x%08X\r\n", (unsigned int) ul_reg, (unsigned int) value);
    util_reg_read(ul_reg);
    printf("\r\n");
}
//void gapAddrCopy(struct sonata_gap_bdaddr *target, uint8_t * source)
//{
//    if (source == NULL)
//    {
//        return;
//    }
//    addrCopy(target.addr.addr, source);
//    target.addr_type = (source[5] & 0xC0) ? SONATA_ADDR_RAND : SONATA_ADDR_PUBLIC;
//
//}





