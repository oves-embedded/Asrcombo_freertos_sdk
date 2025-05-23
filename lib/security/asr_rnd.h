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

#ifndef _ASR_RND_H_
#define _ASR_RND_H_

#include <stdint.h>

#define SEED_MAX_SIZE_WORDS 12
#define BLOCK_SIZE_IN_WORDS 4

typedef struct
{
    /* Seed buffer, consists from concatenated Key||V: max size 12 words */
    uint32_t Seed[SEED_MAX_SIZE_WORDS];
    /* Previous value for continuous test */
    uint32_t PreviousRandValue[BLOCK_SIZE_IN_WORDS];

    /* AdditionalInput buffer max size = seed max size words + 4w for padding*/
    uint32_t PreviousAdditionalInput[SEED_MAX_SIZE_WORDS+3];
    uint32_t AdditionalInput[SEED_MAX_SIZE_WORDS+4];
    uint32_t AddInputSizeWords; /* size of additional data set by user, words   */

    /* entropy source size in words */
    uint32_t EntropySourceSizeWords;

    /* reseed counter (32 bits active) - indicates number of requests for entropy
    since instantiation or reseeding */
    uint32_t ReseedCounter;

    /* key size: 4 or 8 words according to security strength 128 bits or 256 bits*/
    uint32_t KeySizeWords;

    /* State flag (see definition of StateFlag above), containing bit-fields, defining:
    - b'0: instantiation steps:   0 - not done, 1 - done;
    - 2b'9,8: working or testing mode: 0 - working, 1 - KAT DRBG test, 2 -
        KAT TRNG test;
    b'16: flag defining is Previous random valid or not:
            0 - not valid, 1 - valid */
    uint32_t StateFlag;

    /* Trng processing flag - indicates which ROSC lengths are:
    -  allowed (bits 0-3);
    -  total started (bits 8-11);
    -  processed (bits 16-19);
    -  started, but not processed (bits24-27)              */
    uint32_t TrngProcesState;

    /* validation tag */
    uint32_t ValidTag;

    /* Rnd source entropy size in bits */
    uint32_t EntropySizeBits;
}RND_State_t;


typedef uint32_t (*RndGenerateVectWorkFunc_t)(  \
    RND_State_t *rndState_ptr,  /*context*/     \
    uint16_t outSizeBytes,      /*in*/          \
    uint8_t *out_ptr            /*out*/);

typedef struct
{
    /* The pointer to internal state of RND */
    RND_State_t rndState;

    /* The pointer to user given function for generation random vector */
    RndGenerateVectWorkFunc_t rndGenerateVectFunc;
}RND_Context_t;


#define RND_WORK_BUFFER_SIZE_WORDS 1528  //25

/*! A definition for RAM buffer to be internally used in instantiation (or reseeding) operation. */
typedef struct
{
    /* include the specific fields that are used by the low level */
    uint32_t RndWorkBuff[RND_WORK_BUFFER_SIZE_WORDS];
}RND_WorkBuff_t;

/*****************************************************************************/
/**********************        Public Functions      *************************/
/*****************************************************************************/

/*!
@brief This function initializes the RND context.
It must be called at least once prior to using this context with any API that requires it as a parameter (e.g., other RND APIs, asymmetric
cryptography key generation and signatures).
It is called as part of ARM TrustZone CryptoCell library initialization, which initializes and returns the primary RND context.
This primary context can be used as a single global context for all RND needs.
Alternatively, other contexts may be initialized and used with a more limited scope (for specific applications or specific threads).
The call to this function must be followed by a call to ::CRYS_RND_SetGenerateVectorFunc API to set the generate vector function.
It implements referenced standard [SP800-90] - 10.2.1.3.2 - CTR-DRBG Instantiate algorithm using AES (FIPS-PUB 197) and Derivation Function (DF).
\note Additional data can be mixed with the random seed (personalization data or nonce). If required, this data should be provided by calling ::CRYS_RND_AddAdditionalInput prior to using this API.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_Instantiation
(
    RND_Context_t *rndContext_ptr,      /*!< [in/out]  Pointer to the RND context buffer allocated by the user, which is used to
                                        maintain the RND state, as well as pointers to the functions used for
                                        random vector generation. This context must be saved and provided as a
                                        parameter to any API that uses the RND module.
                                        \note the context must be cleared before sent to the function. */
    RND_WorkBuff_t *rndWorkBuff_ptr     /*!< [in/out] Scratchpad for the RND module's work. */
);


/*!
@brief Clears existing RNG instantiation state.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_UnInstantiation(RND_Context_t *rndContext_ptr       /*!< [in/out] Pointer to the RND context buffer. */);


/*!
@brief This function is used for reseeding the RNG with additional entropy and additional user-provided input.
(additional data should be provided by calling ::CRYS_RND_AddAdditionalInput prior to using this API).
It implements referenced standard [SP800-90] - 10.2.1.4.2 - CTR-DRBG Reseeding algorithm, using AES (FIPS-PUB 197) and Derivation Function (DF).

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_Reseeding
(
    RND_Context_t *rndContext_ptr,      /*!< [in/out] Pointer to the RND context buffer. */
    RND_WorkBuff_t *rndWorkBuff_ptr     /*!< [in/out] Scratchpad for the RND module's work. */
);


/****************************************************************************************/
/*!
@brief Generates a random vector according to the algorithm defined in referenced standard [SP800-90] - 10.2.1.5.2 - CTR-DRBG.
The generation algorithm uses AES (FIPS-PUB 197) and Derivation Function (DF).

\note
<ul id="noteb"><li> The RND module must be instantiated prior to invocation of this API.</li>
<li> In the following cases, Reseeding operation must be performed prior to vector generation:</li>
    <ul><li> Prediction resistance is required.</li>
    <li> The function returns CRYS_RND_RESEED_COUNTER_OVERFLOW_ERROR, stating that the Reseed Counter has passed its upper-limit (2^32-2).</li></ul></ul>

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_GenerateVector
(
    RND_State_t *rndState_ptr,  /*!< [in/out] Pointer to the RND state structure, which is part of the RND context structure.
                                Use rndContext->rndState field of the context for this parameter. */
    uint16_t outSizeBytes,      /*!< [in]  The size in bytes of the random vector required. The maximal size is 2^16 -1 bytes. */
    uint8_t *out_ptr            /*!< [out] The pointer to output buffer. */
);


/****************************************************************************************/
/*!

@brief This function sets the RND vector generation function into the RND context.

It must be called after ::CRYS_RND_Instantiation, and prior to any other API that requires the RND context as parameter.
It is called as part of ARM TrustZone CryptoCell library initialization, to set the RND vector generation function into the primary RND context,
after ::CRYS_RND_Instantiation is called.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_SetGenerateVectorFunc
(
    RND_Context_t *rndContext_ptr,  /*!< [in/out] Pointer to the RND context buffer allocated by the user,
                                    which is used to maintain the RND state as well as pointers
                                    to the functions used for random vector generation. */
    RndGenerateVectWorkFunc_t rndGenerateVectFunc       /*!< [in] Pointer to the random vector generation function.
                                            The pointer should point to the ::CRYS_RND_GenerateVector function. */
);


/**********************************************************************************************************/
/*!
@brief Generates a random vector with specific limitations by testing candidates (described and used in FIPS 186-4: B.1.2, B.4.2 etc.).

This function will draw a random vector, compare it to the range limits, and if within range - return it in rndVect_ptr.
If outside the range, the function will continue retrying until a conforming vector is found, or the maximal retries limit is exceeded.
If maxVect_ptr is provided, rndSizeInBits specifies its size, and the output vector must conform to the range [1 < rndVect < maxVect_ptr].
If maxVect_ptr is NULL, rndSizeInBits specifies the exact required vector size, and the output vector must be the exact same
bit size (with its most significant bit = 1).
\note
The RND module must be instantiated prior to invocation of this API.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_GenerateVectorInRange
(
    RND_Context_t *rndContext_ptr,  /*!< [in/out] Pointer to the RND context buffer. */
    uint32_t rndSizeInBits,         /*!< [in]  The size in bits of the random vector required. The allowed size in range  2 <= rndSizeInBits < 2^19-1, bits. */
    uint8_t *maxVect_ptr,           /*!< [in]  Pointer to the vector defining the upper limit for the random vector output, Given as little-endian byte array.
                                        If not NULL, its actual size is treated as [(rndSizeInBits+7)/8] bytes. */
    uint8_t *rndVect_ptr            /*!< [in/out] Pointer to the output buffer for the random vector. Must be at least [(rndSizeInBits+7)/8] bytes.
                                        Treated as little-endian byte array. */
);


/*************************************************************************************/
/*!
@brief Used for adding additional input/personalization data provided by the user,
to be later used by the ::CRYS_RND_Instantiation/::CRYS_RND_Reseeding/::CRYS_RND_GenerateVector functions.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_AddAdditionalInput
(
    RND_Context_t *rndContext_ptr,     /*!< [in/out] Pointer to the RND context buffer. */
    uint8_t *additonalInput_ptr,            /*!< [in]  The Additional Input buffer. */
    uint16_t additonalInputSize             /*!< [in]  The size of the Additional Input buffer. Must be <= 48, and a multiple of 4. */
);

/*!
@brief The CRYS_RND_EnterKatMode function sets KAT mode bit into StateFlag of global CRYS_RND_WorkingState structure.

The user must call this function before calling functions performing KAT tests.

\note Total size of entropy and nonce must be not great than: ::CRYS_RND_MAX_KAT_ENTROPY_AND_NONCE_SIZE, defined.

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
int asr_RND_EnterKatMode
(
    RND_Context_t *rndContext_ptr,     /*!< [in/out] Pointer to the RND context buffer. */
    uint8_t *entrData_ptr,       /*!< [in] Entropy data. */
    uint32_t entrSize,           /*!< [in] Entropy size in bytes. */
    uint8_t *nonce_ptr,          /*!< [in] Nonce. */
    uint32_t nonceSize,          /*!< [in] Entropy size in bytes. */
    RND_WorkBuff_t *workBuff_ptr      /*!< [out] RND working buffer, must be the same buffer, which should be passed into
                        Instantiation/Reseeding functions. */
);

/**********************************************************************************************************/
/*!
@brief The CRYS_RND_DisableKatMode function disables KAT mode bit into StateFlag of global CRYS_RND_WorkingState structure.

The user must call this function after KAT tests before actual using RND module (Instantiation etc.).

@return CRYS_OK on success.
@return A non-zero value from crys_rnd_error.h on failure.
*/
void asr_RND_DisableKatMode
(
    RND_Context_t *rndContext_ptr     /*!< [in/out] Pointer to the RND context buffer. */
);

int asr_hal_random_get(void *buf, uint16_t bytes);

#endif //_ASR_RND_H_