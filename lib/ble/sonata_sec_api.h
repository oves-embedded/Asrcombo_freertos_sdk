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
 * @file sonata_sec_api.h
 *
 * @brief header file - security application interface
 *
 ****************************************************************************************
 */


#ifndef _SONATA_SEC_API_H_
#define _SONATA_SEC_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_utils_api.h"



/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */





/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */
/** security level **/
typedef enum{
    /** security mode 1 level 1: no security **/
    SONATA_SEC_M1_L1 = 0,
    /** security mode 1 level 2: unauthenticated pairing with encryption , no mitm protection **/
    SONATA_SEC_M1_L2,
    /** security mode 1 level 3: authenticated pairing with encryption, mitm protection **/
    SONATA_SEC_M1_L3,
    /** security mode 1 level 4: authenticated le secure connections pairing with encryption using a 128-bit stength encryption key **/
    SONATA_SEC_M1_L4,
    /** unauthenticated pairing with data signing **/
    SONATA_SEC_M2_L1,
    /** authenticated pairing with data signing **/
    SONATA_SEC_M2_L2,
}sonata_sec_lvl_t;

/**IO capabilities**/
typedef enum{
    /** display only **/
    SONATA_IO_CAP_DISPLAY_ONLY = 0,
    /** display yes no **/
    SONATA_IO_CAP_DISPLAY_YES_NO,
    /** keyboard only **/
    SONATA_IO_CAP_KB_ONLY,
    /** no input no output **/
    SONATA_IO_CAP_NO_INPUT_NO_OUTPUT,
    /** keyboard display **/
    SONATA_IO_CAP_KB_DISPLAY,
}sonata_iocap_t;

/** OOB data present flag values **/
typedef enum{
    /** OOB data not present **/
    SONATA_OOB_AUTH_DATA_NOT_PRESENT = 0,
    /** OOB data present **/
    SONATA_OOB_AUTH_DATA_PRESENT,
}sonata_oob_t;

/** authentication requirements **/




/*
 * Type Definition
 ****************************************************************************************
 */
typedef struct sonata_sec_api_cb{

    struct sonata_sec_api_cb * _next;
}sonata_sec_api_cb_t;



typedef struct sonata_public_key
{
    uint8_t pub_key_x[32];
    uint8_t pub_key_y[32];
}sonata_public_key_t;


typedef  void (* sonata_sec_public_key_gen_complete)(sonata_public_key_t * pub_key, uint16_t status) ;

typedef void (*sonata_sec_ecdh_secret_complete) (const uint8_t* p_ecdh_secret,uint16_t status);

typedef void (*sonata_sec_ecdh_session_key_complete) (const uint8_t* p_ecdh_session,uint16_t status);

typedef  void (* sonata_sec_keys_gen_complete)(uint8_t * pub_key, uint8_t * priv_key, uint16_t status) ;


/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */



/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

 /**
 ****************************************************************************************
 * @brief Initialize User Security Module
 ****************************************************************************************
 */
void sonata_sec_api_init(void);
/**
 ****************************************************************************************
 * @brief Generate a new DHKey using ECC P256 algorithm
 *
 * @param[in] secret_key Private key                  - LSB First
 * @param[in] pub_key_x  Peer public key x coordinate - LSB First
 * @param[in] pub_key_y  Peer public key y coordinate - LSB First
 * @param[in] callback   callback
 *
 * @return status   0 if key generation is started, > 0 otherwise
 ****************************************************************************************
 */
uint8_t sonata_genc_DH_key(uint8_t* secret_key,uint8_t* public_key_x, uint8_t* public_key_y ,sonata_sec_ecdh_secret_complete callback);
/**
 ****************************************************************************************
 * @brief Generate a new Private key compliant with ECC P256 algorithm and Generate a publickey using ECC P256 algorithm
 *
 * @param[out]  p_public_key  publickey  (64byte)       - LSB First
 * @param[out]  p_privateKey  Private key(32byte)       - LSB First
 *
 ****************************************************************************************
 */
void sonata_ble_ecdh_generate_keys_secp256r1(uint8_t* p_public_key,uint8_t* p_privateKey);
/**
 ****************************************************************************************
 * @brief Generate a new DHKey using ECC P256 algorithm
 *
 * @param[in]  p_public_key  Peer publickey(64byte)     - LSB First
 * @param[in]  p_privateKey  Private key   (32byte)     - LSB First
 * @param[out] DHKey         DHKey         (32byte)     - LSB First
 *
 ****************************************************************************************
 */
void sonata_ble_ecdh_shared_secret_secp256r1(uint8_t* p_public_key,uint8_t* p_privateKey,uint8_t* DHKey);
/**
 ****************************************************************************************
 * @brief Generate a new Public key pair using ECC P256 algorithm
 *
 * @param[in] secret_key Private key - LSB First
 ****************************************************************************************
 */
void sonata_genc_public_key(uint8_t* secret_key,sonata_sec_public_key_gen_complete callback);
/**
 ****************************************************************************************
 * @brief Generate a Secret Key compliant with ECC P256 algorithm
 *
 * @param[out] secret_key Private key - LSB First
 ****************************************************************************************
 */
void sonata_genc_secret_key(uint8_t* secret_key256);
/**
 ****************************************************************************************
 * @brief Generate a new Private key compliant with ECC P256 algorithm and Generate a publickey using ECC P256 algorithm
 *
 ****************************************************************************************
 */
void sonata_ecdh_generate_keys(sonata_sec_keys_gen_complete callback);


#endif //_SONATA_SEC_API_H_

