#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "joylink.h"

//example.
#include "joylink_extern.h"
#include "joylink_extern_ota.h"
#include "joylink_extern_json.h"

//joylink.
#include "joylink_ret_code.h"

//pal.
#include "joylink_flash.h"
#include "joylink_log.h"
#include "joylink_memory.h"
#include "joylink_socket.h"
#include "joylink_softap.h"
#include "joylink_stdint.h"
#include "joylink_stdio.h"
#include "joylink_string.h"
#include "joylink_log.h"
#include "joylink_thread.h"
#include "joylink_time.h"

//lega board.
#include "lega_wlan_api.h"
#include "duet_flash_kv.h"
#define lega_flash_kv_set duet_flash_kv_set
#define lega_flash_kv_get duet_flash_kv_get
#define lega_flash_kv_del duet_flash_kv_del
#define lega_flash_erase  duet_flash_erase
#define lega_flash_write  duet_flash_write
#define lega_flash_read   duet_flash_read

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/

/****************************************************************************
 * Public Data
****************************************************************************/

/****************************************************************************
 * Private Data
****************************************************************************/

/****************************************************************************
 * Public Function
****************************************************************************/
/**
 * @brief:set data in jl_flash
 *
 * @param: key, value
 *
 * @returns: success 0, failed -1
 */
int32_t jl_flash_SetInt8(const char* key, int8_t value)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "wb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "wb+");
    }
    fwrite(&value, sizeof(int8_t), 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
#else
    jl_platform_printf("jl_flash_SetInt8 ret:0\n");
    return 0;
#endif
}

int32_t jl_flash_SetInt32(const char* key, int32_t value)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "wb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "wb+");
    }
    fwrite(&value, sizeof(int32_t), 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
#else
    jl_platform_printf("jl_flash_SetInt32 ret:0\n");
    return 0;
#endif
}

int32_t jl_flash_SetInt64(const char* key, int64_t value)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "wb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "wb+");
    }
    fwrite(&value, sizeof(int64_t), 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
#else
    jl_platform_printf("jl_flash_SetInt64 ret:0\n");
    return 0;
#endif
}

int32_t jl_flash_SetString(const char* key, const char* value, int32_t len)
{
#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_SetString key:%s ,value:%s ,len:%d\n", key, value, len);
#endif

#ifdef __FREERTOS_PAL__
    int32_t ret=0;

    if (strcmp(key, KEY_FAC) == 0) {
        ret = lega_flash_kv_set(KEY_FAC, (void *)value, len, 1);//写KV值数组进flash
    } else if (strcmp(key, KEY_SDK) == 0) {
        ret = lega_flash_kv_set(KEY_SDK, (void *)value, len, 1);//写KV值数组进flash
    }
    return ret;
#else
    return 0;
#endif
}

int32_t jl_flash_GetString(const char* key, char* buff, int32_t len)
{
#ifdef __FREERTOS_PAL__
    int32_t read_len;

    if (strcmp(key, KEY_FAC) == 0) {
        lega_flash_kv_get(KEY_FAC, buff, &len);//读取KV值数组
    } else if (strcmp(key, KEY_SDK) == 0) {
        lega_flash_kv_get(KEY_SDK, buff, &len);//读取KV值数组
    }

    read_len = jl_platform_strlen(buff);

#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_GetString key:%s ,value:%s ,len:%d\n", key, buff, read_len);
#endif

    return read_len;
#else
    return 0;
#endif
}

int32_t jl_flash_SetBool(const char* key, int8_t value)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "wb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "wb+");
    }
    fwrite(&value, sizeof(int8_t), 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
#else
    jl_platform_printf("jl_flash_SetBool ret:0\n");
    return 0;
#endif
}


/**
 * @brief:get data from jl_flash
 *
 * @param: key, value
 *
 * @returns: 读取到的value值或value长度len, 如果读取失败则返回-1
 */
int8_t     jl_flash_GetInt8(const char* key, int8_t defaultValue)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    int8_t value = 0;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "rb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "rb+");
    }
    if (fread(&value, sizeof(int8_t), 1, fp) == 0) {
        value = defaultValue;
        printf("jl_flash_GetInt8, fread failed, value = defaultValue\n");
    }
    fclose(fp);
    fp = NULL;
    printf("jl_flash_GetInt8, value = %d\n", value);
    return value;
#else
    jl_platform_printf("jl_flash_GetInt8 ret:0\n");
    return 0;
#endif
}

int32_t jl_flash_GetInt32(const char* key, int32_t defaultValue)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    int32_t value = 0;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "rb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "rb+");
    }
    if (fread(&value, sizeof(int32_t), 1, fp) == 0) {
        value = defaultValue;
        printf("jl_flash_GetInt32, fread failed, value = defaultValue\n");
    }
    fclose(fp);
    fp = NULL;
    printf("jl_flash_GetInt32, value = %d\n", value);
    return value;
#else
    jl_platform_printf("jl_flash_GetInt32 ret:0\n");
    return 0;
#endif
}

int64_t jl_flash_GetInt64(const char* key, int64_t defaultValue)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    int64_t value = 0;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "rb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "rb+");
    }
    if (fread(&value, sizeof(int64_t), 1, fp) == 0) {
        value = defaultValue;
        printf("jl_flash_GetInt64, fread failed, value = defaultValue\n");
    }
    fclose(fp);
    fp = NULL;
    printf("jl_flash_GetInt64, value = %ld\n", value);
    return value;
#else
    jl_platform_printf("jl_flash_GetInt64 ret:0\n");
    return 0;
#endif
}

int8_t  jl_flash_GetBool(const char* key, int8_t defaultValue)
{
#ifdef __LINUX_PAL__
    FILE *fp = NULL;
    int8_t value = 0;
    if (strcmp(key, KEY_FAC) == 0) {
        fp = fopen("/tmp/flashfac.txt", "rb+");
    } else if (strcmp(key, KEY_SDK) == 0) {
        fp = fopen("/tmp/flashsdk.txt", "rb+");
    }
    if (fread(&value, sizeof(int8_t), 1, fp) == 0) {
        value = defaultValue;
        printf("jl_flash_GetBool, fread failed, value = defaultValue\n");
    }
    fclose(fp);
    fp = NULL;
    printf("jl_flash_GetBool, value = %d\n", value);
    return value;
#else
    jl_platform_printf("jl_flash_GetBool ret:0\n");
    return 0;
#endif
}

/**
 * @brief:clear jl_flash data
 *
 * @param: key
 *
 * @returns: none
 */
void jl_flash_ClearKey(const char *key)
{
#ifdef __LINUX_PAL__
    if (strcmp(key, KEY_FAC) == 0) {
        system("> /tmp/flashfac.txt");
    } else if (strcmp(key, KEY_SDK) == 0) {
        system("> /tmp/flashsdk.txt");
    }
#else
    jl_platform_printf("jl_flash_ClearKey ret:0\n");
    return;
#endif
}

/**
 * @brief: 二进制数据写入flash
 *
 * @param[in] offset: flash地址偏移
 *
 * @param[in] buf: 写入flash的数据
 *
 * @param[in] len: 要写入数据的长度
 *
 * @return 返回实际写入的数据长度，写入失败则返回-1
 *
 */

int32_t jl_flash_SetBuffer(int32_t offset, const void* buf, int32_t len)
{
#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_SetBuffer ret:0\n");
#endif

    return 0;
}

/**
 * @brief: SDK申请预留flash空间
 *
 * @param[in] size: 申请空间的大小，字节为单位
 *
 * @return： 申请flash空间的偏移，如果申请失败则返回-1
 *
 */
int32_t jl_flash_request(int32_t size)
{
#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_request ret:0\n");
#endif

    return 0;
}

/**
 * @brief: 读取指定偏移地址的数据
 *
 * @param[in] offset: flash偏移地址
 *
 * @param[out] buf: 存储读取数据的buffer
 *
 * @param[in] len: 预期读取的数据长度
 *
 * @return int32_t: 实际读取的数据长度，如果读取失败则返回-1
 *
 */

int32_t jl_flash_GetBuffer(int32_t offset, const void* buf, int32_t len)
{
#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_GetBuffer ret:0\n");
#endif

    return 0;
}

/**
 * @brief: 清除指定偏移地址的数据
 *
 * @param[in] offset: flash偏移地址
 *
 * @param[in] len: 预期读取的数据长度
 *
 * @return int32_t: 实际读取的数据长度，如果读取失败则返回-1
 *
 */
int32_t jl_flash_ClearBuffer(int32_t offset, int32_t len)
{
#ifdef JOYLINK_FLASH_DEBUG_INFO
    jl_platform_printf("jl_flash_ClearBuffer ret:0\n");
#endif

    return 0;
}