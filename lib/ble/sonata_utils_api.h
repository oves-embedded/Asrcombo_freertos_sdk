/*
 * Copyright © 2023 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
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
/**
 ****************************************************************************************
 *
 * @file sonata_utils_api.h
 *
 * @brief header file - asr utilities
 *
 ****************************************************************************************
 */


#ifndef _SONATA_UTILS_API_H_
#define _SONATA_UTILS_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include "sonata_error_api.h"
#include "sonata_ble_hook.h"
#ifdef CFG_PLF_SONATA
#include "sonata_flash.h"
#endif
/**
 * @defgroup SONATA_UTILS_API UTILS_API
 * @{
 */

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
#if (CORE_BLUETOOTH_RESOURCE)
#define SIZE_U32 (sizeof(uint32_t))
#endif
#define __SONATA_INLINE static __attribute__((__always_inline__)) inline
#define __SONATA_ARRAY_EMPTY

/// maxium supported messages from application
#define SONATA_API_MAX_APP_MSG                  32
#define SONATA_TIMER_DELAY_MAX                 (41940000) // (about 11.65 hour)


/// debug trace
#ifdef SONATA_UTILS_API_DBG
#define SONATA_UTILS_API_TRC    printf
#else
#define SONATA_UTILS_API_TRC(...)
#endif //SONATA_API_TASK_DBG

/// file system first tag id for application
#define APP_DATA_SAVE_TAG_FIRST              (0x90)
/// file system last tag id for application
#define APP_DATA_SAVE_TAG_LAST               (0xAF)
/// file system bluetooth device address tag id
#define SONATA_FS_TAG_BD_ADDR                (0x01)
/// file system bluetooth device address tag length
#define SONATA_FS_TAG_BD_ADDR_LEN            6

#define PWR_ON_RST               0x00
#define HARDWARE_PIN_RST         0x01
#define SOFTWARE_RST             0x02
#define UNKNOWN_RST              0xFF

/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */


/*!
* @brief app msg operation type
*/
typedef enum
{
    /// at command operation
    APP_MSG_AT_CMD             = (1 << 0),
    APP_MSG_UART_CMD           = (5),

    APP_MSG_HID_TIMER          = (10),

    APP_MSG_LAST,
}app_msg_op_t;

/*!
* @brief file system possible return status
*/
typedef enum
{
    /// SONATA FILE SYSTEM status OK
    SONATA_FS_OK,
    /// generic SONATA FILE SYSTEM status KO
    SONATA_FS_FAIL,
    /// SONATA FILE SYSTEM TAG unrecognized
    SONATA_FS_TAG_NOT_DEFINED,
    /// No space for SONATA FILE SYSTEM
    SONATA_FS_NO_SPACE_AVAILABLE,
    /// Length violation
    SONATA_FS_LENGTH_OUT_OF_RANGE,
    /// SONATA FILE SYSTEM parameter locked
    SONATA_FS_PARAM_LOCKED,
    /// SONATA FILE SYSTEM corrupted
    SONATA_FS_CORRUPT,
    /// SONATA FILE TYPE OUT RANGE
    SONATA_FS_OT_RANGE,
} SONATA_FS_STATUS;
typedef uint8_t sonata_fs_len_t;

typedef uint8_t sonata_fs_tag_t;

/*!
* @brief ble addr priority return result
*/
typedef enum
{
    ///efuse > nvds
    SONATA_MAC_USE_EFUSE,
    ///nvds > efuse
    SONATA_MAC_USE_NVDS,
}SONATA_MAC_PRIORITY;



/*
 * Type Definition
 ****************************************************************************************
 */
/// application message handler
typedef uint8_t (*PF_SONATA_API_APP_MSG_HANDLER)(void *p_param);

/// asr api task messages
typedef struct sonata_api_app_msg
{
    uint8_t operation;
    PF_SONATA_API_APP_MSG_HANDLER function;
} sonata_api_app_msg_t;

/// asr api task message array
typedef struct sonata_api_app_msg_array
{
    int msg_num;
    sonata_api_app_msg_t *msg[SONATA_API_MAX_APP_MSG];
} sonata_api_app_msg_array_t;

/// asr api task message array
typedef struct sonata_api_app_ke_msg
{
    uint8_t operation;
    void *p_param;
} sonata_api_app_ke_msg_t;

/// Ble bt addr priority callback
typedef struct
{
    SONATA_MAC_PRIORITY (*ble_addr_priority)(void);

} ble_addr_callback_t;

typedef enum
{
    AES_PADDING_NONE = 0, /*!< No padding. */
    AES_PADDING_PKCS7 = 1, /*!< PKCS7 padding. */
} AesPaddingType_t;

typedef enum
{
    AES_MODE_ECB = 0, /*!< ECB mode. */
    AES_MODE_CBC = 1, /*!< CBC mode. */
} AesOperationMode_t;

/// the aes context-type definition
typedef struct
{
    uint8_t key[16];
    uint8_t iv[16];
    uint8_t temp[16];
    uint8_t *input;
    uint32_t input_len;
    uint8_t *output;
    uint32_t output_len;
    void *res_cb;
    uint8_t is_enc;
    uint8_t is_ongoing;
    uint8_t op_mode;
    uint32_t op_offset;
    uint8_t paddingtype;
    int8_t res_status; //Execution status
} sonata_aes_context;

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of AES cbc algorithm
 *
 * @param[in] status       Execution status
 * @param[in] ctx          aes-cbc context
 ****************************************************************************************
 */
typedef void (*aes_app_result_cb) (sonata_aes_context *ctx);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of an AES based algorithm
 *
 * @param[in] status       Execution status
 * @param[in] aes_res      16 bytes block result
 * @param[in] src_info     Information provided by requester
 ****************************************************************************************
 */
typedef void (*aes_func_result_cb) (uint8_t status, const uint8_t* aes_res, uint32_t src_info);


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
***************************************************************************************
* @brief asr api send msg from app
* @param[in] uint8_t op : operation code
*       [in] void *p_param
*
* @return uint16_t : api result
***************************************************************************************
*/
uint16_t sonata_api_send_app_msg(uint8_t op, void *p_param);

/**
***************************************************************************************
* @brief asr api register message
* @param[in] const sonata_api_msg_t *cmd
*
*
* @return  uint16_t : api result
***************************************************************************************
*/
uint16_t sonata_api_app_msg_register(const sonata_api_app_msg_t *msg);

///@hide
uint8_t sonata_api_app_ke_msg_handler(void *p_param);

///@hide
void sonata_api_util_gap_status_print(uint8_t status);

///@hide
void sonata_api_util_gatt_status_print(uint8_t operation,uint8_t status);


/**
***************************************************************************************
* @brief asr api create timer
* @param[in] uint8_t timer_id : timer id, should be uniquely in global
* @param[in] uint32_t delay : delay time, if bigger than @SONATA_TIMER_DELAY_MAX, delay will be set to SONATA_TIMER_DELAY_MAX
*
* @return  uint16_t : api result
***************************************************************************************
*/
uint16_t sonata_api_app_timer_set(uint8_t timer_id,  uint32_t delay);

/**
***************************************************************************************
* @brief asr api clear timer
* @param[in] uint8_t timer_id : timer id, should be uniquely in global
*
*
* @return  uint16_t : api result
***************************************************************************************
*/
void sonata_api_app_timer_clear(uint8_t timer_id);

/**
***************************************************************************************
* @brief asr api to judge time active status
* @param[in] uint8_t timer_id : timer id, should be uniquely in global
*
*
* @return  bool : true(active) or false(inactive)
***************************************************************************************
*/
bool sonata_api_app_timer_active(uint8_t timer_id);

/*****************************************************************************************

* @brief sonata malloc dynamic memory
* @param[in] uint16_t size : memory size
*
*
* @return void * : NULL or memory addr
****************************************************************************************
*/
void *sonata_api_malloc(uint16_t size);

/**
***************************************************************************************
* @brief free memory alloced by sonata_api_malloc
* @param[in] void *p_ptr : memory addr
*
*
* @return void
****************************************************************************************
*/
void sonata_api_free(void *p_ptr);

/*****************************************************************************************
* @brief get the free heap size
*
*
* @return uint32_t the free heap size
****************************************************************************************/
uint32_t sonata_api_free_heap_size(void);

/**
***************************************************************************************
* @brief sonata file system write to flash
* @param[in] sonata_fs_tag_t tag : tag
* @param[in] sonata_fs_len_t length : write len
* @param[in] uint8_t *buf           : write buf
* @note      for example: write bluetooth device address
* @note      sonata_fs_write(SONATA_FS_TAG_BD_ADDR,SONATA_FS_TAG_BD_ADDR_LEN,buf);
*
* @return uint8_t @see SONATA_FS_STATUS
****************************************************************************************
*/
uint8_t sonata_fs_write(sonata_fs_tag_t tag, sonata_fs_len_t length, uint8_t *buf);

/**
***************************************************************************************
* @brief sonata file system read flash
* @param[in] sonata_fs_tag_t tag : tag
* @param[in] sonata_fs_len_t * lengthPtr: read length
* @param[in] uint8_t *buf           : read buf
* @note      for example: read bluetooth device address
* @note      sonata_fs_len_t length = SONATA_FS_TAG_BD_ADDR_LEN;
* @note      sonata_fs_read(SONATA_FS_TAG_BD_ADDR,&length,buf);
*
* @return uint8_t @see SONATA_FS_STATUS
****************************************************************************************
*/
uint8_t sonata_fs_read(sonata_fs_tag_t tag, sonata_fs_len_t * lengthPtr, uint8_t *buf);

/**
***************************************************************************************
* @brief sonata file system erase flash
* @param[in] sonata_fs_tag_t tag : tag
*
*
* @return uint8_t @see SONATA_FS_STATUS
****************************************************************************************
*/
uint8_t sonata_fs_erase(sonata_fs_tag_t tag);

/*!
 * @brief get BT address
 * @return address
 */
uint8_t * sonata_get_bt_address();


/**
***************************************************************************************
* @brief sonata set bd addr
* @param[in] uint8_t* bd_addr : bd_addr value
*             uint8_t  length: addr length
* @return bool : true(success) or false(fail)
****************************************************************************************
*/
bool sonata_set_bt_address(uint8_t * bd_addr,uint8_t length);

/**
***************************************************************************************
* @brief sonata set bd addr but no save
* @param[in] uint8_t* bd_addr : bd_addr value
*             uint8_t  length: addr length
* @return void
****************************************************************************************
*/
void sonata_set_bt_address_no_save(uint8_t * bd_addr,uint8_t length);

/**
 ****************************************************************************************
 * @brief Perform an AES encryption form app - result within callback
 * @param[in] key      Key used for the encryption
 * @param[in] val      Value to encrypt using AES
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 * @param[in] src_info Information used retrieve requester
 *
 * @return  bool : true for encryption is ongoing, false for some error
 ****************************************************************************************
 */
bool sonata_aes_app_encrypt(uint8_t* key, uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);

/**
 ****************************************************************************************
 * @brief Perform an AES decryption form app - result within callback
 * @param[in] key      Key used for the decryption
 * @param[in] val      Value to decrypt using AES
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 * @param[in] src_info Information used retrieve requester
 *
 * @return  bool : true for decryption is ongoing, false for some error
 ****************************************************************************************
 */
bool sonata_aes_app_decrypt(uint8_t* key, uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);

/**
 ****************************************************************************************
 * @brief Perform an AES-CBC encryption form app - result within callback
 * @param[in] ctx      aes context
 * @param[in] key      Key used for the encryption
 * @param[in] input    The input data to be encrypted
 * @param[in] input_len    The len of input
 * @param[in] output   The buffer of output data encrypted
 * @param[in] output_buf_len The address of output size, which should be large enough to hold the output. A number larger than 'sizeof(input) + 16' is suggested
 * @param[in] iv       The Initialization vector (updated after use).It must be a readable and writeable buffer of 16 Bytes.
 * @param[in] res_cb   Function that will handle the AES result
 * @param[in] paddingtype the padding type of data
 *
 * @return  int : 0 for encryption is ongoing.
 *                -1 point parameter is NULL.
 *                -2 op_mode is not AES_MODE_ECB/AES_MODE_CBC.
 *                -3 paddingtype is not AES_PADDING_NONE/AES_PADDING_PKCS7.
 *                -4 input_len is invalid.
 *                -5 output_len is invalid.
 ****************************************************************************************
 */
int sonata_aes_cbc_app_encrypt(sonata_aes_context *ctx, uint8_t key[16], uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t output_buf_len, uint8_t iv[16], aes_app_result_cb res_cb, AesPaddingType_t paddingtype);

/**
 ****************************************************************************************
 * @brief Perform an AES-CBC decryption form app - result within callback
 * @param[in] ctx      aes context
 * @param[in] key      Key used for the decryption
 * @param[in] input    The input data to be decrypted
 * @param[in] input_len    The len of input
 * @param[in] output   The buffer of output data decrypted
 * @param[in] output_buf_len The address of output size, which should be large enough to hold the output. A number larger than 'sizeof(input) + 16' is suggested
 * @param[in] iv    The Initialization vector (updated after use).It must be a readable and writeable buffer of 16 Bytes.
 * @param[in] res_cb   Function that will handle the AES result
 * @param[in] paddingtype the padding type of data
 *
 * @return  int : 0 for decryption is ongoing.
 *                -1 point parameter is NULL.
 *                -2 op_mode is not AES_MODE_ECB/AES_MODE_CBC.
 *                -3 paddingtype is not AES_PADDING_NONE/AES_PADDING_PKCS7.
 *                -4 input_len is invalid.
 *                -5 output_len is invalid.
 ****************************************************************************************
 */
int sonata_aes_cbc_app_decrypt(sonata_aes_context *ctx, uint8_t key[16], uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t output_buf_len, uint8_t iv[16], aes_app_result_cb res_cb, AesPaddingType_t paddingtype);

/**
 ****************************************************************************************
 * @brief Perform an AES-ECB encryption form app - result within callback
 * @param[in] ctx      aes context
 * @param[in] key      Key used for the encryption
 * @param[in] input    The input data to be encrypted
 * @param[in] input_len    The len of input
 * @param[in] output   The buffer of output data encrypted
 * @param[in] output_buf_len The address of output size, which should be large enough to hold the output. A number larger than 'sizeof(input) + 16' is suggested
 * @param[in] res_cb   Function that will handle the AES result
 * @param[in] paddingtype the padding type of data
 *
 * @return  int : 0 for encryption is ongoing.
 *                -1 point parameter is NULL.
 *                -2 op_mode is not AES_MODE_ECB/AES_MODE_CBC.
 *                -3 paddingtype is not AES_PADDING_NONE/AES_PADDING_PKCS7.
 *                -4 input_len is invalid.
 *                -5 output_len is invalid.
 ****************************************************************************************
 */
int sonata_aes_ecb_app_encrypt(sonata_aes_context *ctx, uint8_t key[16], uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t output_buf_len, aes_app_result_cb res_cb, AesPaddingType_t paddingtype);

/**
 ****************************************************************************************
 * @brief Perform an AES-ECB decryption form app - result within callback
 * @param[in] ctx      aes context
 * @param[in] key      Key used for the decryption
 * @param[in] input    The input data to be decrypted
 * @param[in] input_len    The len of input
 * @param[in] output   The buffer of output data decrypted
 * @param[in] output_buf_len The address of output size, which should be large enough to hold the output. A number larger than 'sizeof(input) + 16' is suggested
 * @param[in] res_cb   Function that will handle the AES result
 * @param[in] paddingtype the padding type of data
 *
 * @return  int : 0 for decryption is ongoing.
 *                -1 point parameter is NULL.
 *                -2 op_mode is not AES_MODE_ECB/AES_MODE_CBC.
 *                -3 paddingtype is not AES_PADDING_NONE/AES_PADDING_PKCS7.
 *                -4 input_len is invalid.
 *                -5 output_len is invalid.
 ****************************************************************************************
 */
int sonata_aes_ecb_app_decrypt(sonata_aes_context *ctx, uint8_t key[16], uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t output_buf_len, aes_app_result_cb res_cb, AesPaddingType_t paddingtype);

/**
 ****************************************************************************************
 * @brief Perform aes status query form app
 * @param[in] ctx      aes context
 *
 * @return  int : 1 for encryption/decryption is ongoing, 0 is idle. -1 for context point is NULL.
 ****************************************************************************************
 */
int sonata_aes_app_is_onging(sonata_aes_context *ctx);

/*!
 * @brief change pin code to byte
 * @param pin_code
 * @param bytes
 * @param byte_len
 */
void sonata_passkey_pincode_to_byte(uint32_t pin_code, uint8_t * bytes, uint8_t byte_len);

/*!
 * @brief change byte to pin code
 * @param bytes
 * @return
 */
uint32_t sonata_passkey_byte_to_pincode(uint8_t * bytes);

/*!
 * @brief Get information form advertising report
 * @param type [in] GAP advertising flags, for example GAP_AD_TYPE_COMPLETE_NAME is the device's name in advertising report.
 * @param info [in]  save return value
 * @param info_length [in]  length of the info buffer
 * @param report  [in] advertising report data
 * @param report_length  [in] advertising report data length
 * @param rel_length  [out] for target information
 * @return TRUE for target fond, FALSE for not found
 */
bool sonata_get_adv_report_info(uint8_t type, uint8_t *info, uint16_t info_length, uint8_t *report, uint16_t report_length, uint16_t *rel_length);

/*!
 * @brief Get system tick value
 * @return System tick
 */
uint32_t sonata_get_sys_time();

/*!
 * @brief convert millisecond to half slot
 * @return half slot
 */
uint32_t sonata_ms_2_hs(uint32_t ms);

/*!
 * @brief convert half slot to millisecond
 * @return millisecond
 */
uint32_t sonata_hs_2_ms(uint32_t hs);


/*!
 * @brief Reset BLE stack
 * @return
 */
void sonata_ble_stack_reset();

/*!
 * @brief  Get boot type value
 * @note   in low power mode, if use this API should enable the peri_clk of RET_HCLK_EN
 * @return 0x00: PWR_ON_RST, 0x01: HARDWARE_PIN_RST, 0x02: SOFTWARE_RST, 0xFF: UNKNOWN_RST
 */
uint32_t sonata_get_boot_rst_type(void);

/*!
 * @brief  Get sonata rom version
 * @return rom version
 */
const char *sonata_get_rom_version(void);

/*!
 * @brief Ble addr priority callback
 */
void sonata_ble_register_bt_addr_callback(ble_addr_callback_t *cb);

extern int rand (void);

__SONATA_INLINE uint8_t util_rand_byte(void)
{
    return (uint8_t)(rand() & 0xFF);
}
__SONATA_INLINE uint32_t util_rand_word(void)
{
    return (uint32_t)rand();
}

__SONATA_INLINE uint32_t util_min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

__SONATA_INLINE void util_write16p(void const *ptr16, uint16_t value)
{
    uint8_t *ptr=(uint8_t*)ptr16;

    *ptr++ = value&0xff;
    *ptr = (value&0xff00)>>8;
}

__SONATA_INLINE uint16_t util_read16p(void const *ptr16)
{
    uint16_t value = ((uint8_t *)ptr16)[0] | ((uint8_t *)ptr16)[1] << 8;
    return value;
}

__SONATA_INLINE uint32_t util_read32p(void const *ptr32)
{
    uint16_t addr_l, addr_h;
    addr_l = util_read16p((uint16_t *)ptr32);
    addr_h = util_read16p((uint16_t *)ptr32 + 1);
    return ((uint32_t)addr_l | (uint32_t)addr_h << 16);
}

void sonata_platform_reset();

/*!
 * @brief  check if the addrss is equal with 00:00:00:00:00:00
 * @return false:if equal with 00:00:00:00:00:00, means the address is invalid;
 */
bool sonata_api_addr_valid(uint8_t* addr);

/*!
 * @brief Initialize the LE Coded PHY 500 Kbps selection , 125 Kbps by default
 * @param usePhy500  false: 125 Kbps, true: 500 Kbps
 * @return true: set success, false: set fail.
 */
bool sonata_ble_set_phy_500_flag(bool usePhy500);

/*!
 * @brief  Get the LE Coded PHY 500 Kbps selection flag
 * @return false: 125 Kbps(default), true: 500 Kbps
 */
bool sonata_ble_get_phy_500_flag();
#ifdef CFG_PLF_SONATA
/**
 * Write data to an area on a flash logical partition without erase. Deal with interrupt before and after
 *
 * @note in sonata platform, this function will disable interrupt before write flash and enable it after.

 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t sonata_ble_flash_write(sonata_partition_t in_partition, uint32_t *off_set,
                               const void *in_buf, uint32_t in_buf_len);


/**
 * Erase an area on a Flash logical partition . Deal with interrupt before and after
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 * @note in sonata platform, this function will disable interrupt before write flash and enable it after.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t sonata_ble_flash_erase(sonata_partition_t in_partition, uint32_t off_set, uint32_t size);
#endif
#if (APP_BLUETOOTH_RESOURCE || CORE_BLUETOOTH_RESOURCE )
#include "sonata_mem_config.h"


uint16_t sonata_get_stack_heap_size(void);

uint16_t sonata_get_stack_msg_size(void);

uint16_t sonata_get_stack_db_size(void);

uint16_t sonata_get_llcp_tx_pool_size(void);

uint16_t sonata_get_acl_tx_pool_size(void);

uint16_t sonata_get_adv_tx_pool_size(void);

uint16_t sonata_get_rx_pool_size(void);

uint16_t sonata_get_llm_ral_size(void);

uint16_t sonata_get_llm_act_info_size(void);

uint16_t sonata_get_dev_list_size(void);

uint16_t sonata_get_sch_size(void);

uint16_t sonata_dynamic_memory_buffer_init(pf_mem_config_t *config);

void sonata_print_dynamic_memory_info(void);

void sonata_dynamic_memory_buffer_free(void);

#endif


/** @}*/
#endif //_SONATA_UTILS_API_H_

