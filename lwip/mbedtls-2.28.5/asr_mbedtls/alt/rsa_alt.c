/*
 *  The RSA public-key cryptosystem
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  The following sources were referenced in the design of this implementation
 *  of the RSA algorithm:
 *
 *  [1] A method for obtaining digital signatures and public-key cryptosystems
 *      R Rivest, A Shamir, and L Adleman
 *      http://people.csail.mit.edu/rivest/pubs.html#RSA78
 *
 *  [2] Handbook of Applied Cryptography - 1997, Chapter 8
 *      Menezes, van Oorschot and Vanstone
 *
 *  [3] Malware Guard Extension: Using SGX to Conceal Cache Attacks
 *      Michael Schwarz, Samuel Weiser, Daniel Gruss, Clémentine Maurice and
 *      Stefan Mangard
 *      https://arxiv.org/abs/1702.08719v2
 *
 */
#include "common.h"

#if defined(MBEDTLS_RSA_C)

#include "mbedtls/rsa.h"
#include "mbedtls/rsa_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "constant_time_internal.h"
#include "mbedtls/constant_time.h"

#include <string.h>

#if defined(MBEDTLS_PKCS1_V21)
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_PKCS1_V15) && !defined(__OpenBSD__) && !defined(__NetBSD__)
#include <stdlib.h>
#endif

#include "mbedtls/platform.h"

#if defined(MBEDTLS_RSA_ALT)

#include "mbedtls_hw.h"

/* Parameter validation macros */
#define RSA_VALIDATE_RET(cond)                                       \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_RSA_BAD_INPUT_DATA)
#define RSA_VALIDATE(cond)                                           \
    MBEDTLS_INTERNAL_VALIDATE(cond)

int mbedtls_rsa_import(mbedtls_rsa_context *ctx,
                       const mbedtls_mpi *N,
                       const mbedtls_mpi *P, const mbedtls_mpi *Q,
                       const mbedtls_mpi *D, const mbedtls_mpi *E)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    RSA_VALIDATE_RET(ctx != NULL);

    if ((N != NULL && (ret = mbedtls_mpi_copy(&ctx->N, N)) != 0) ||
        (P != NULL && (ret = mbedtls_mpi_copy(&ctx->P, P)) != 0) ||
        (Q != NULL && (ret = mbedtls_mpi_copy(&ctx->Q, Q)) != 0) ||
        (D != NULL && (ret = mbedtls_mpi_copy(&ctx->D, D)) != 0) ||
        (E != NULL && (ret = mbedtls_mpi_copy(&ctx->E, E)) != 0)) {
        return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }

    if (N != NULL) {
        ctx->len = mbedtls_mpi_size(&ctx->N);
    }

    return 0;
}

int mbedtls_rsa_import_raw(mbedtls_rsa_context *ctx,
                           unsigned char const *N, size_t N_len,
                           unsigned char const *P, size_t P_len,
                           unsigned char const *Q, size_t Q_len,
                           unsigned char const *D, size_t D_len,
                           unsigned char const *E, size_t E_len)
{
    int ret = 0;
    RSA_VALIDATE_RET(ctx != NULL);

    if (N != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->N, N, N_len));
        ctx->len = mbedtls_mpi_size(&ctx->N);
    }

    if (P != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->P, P, P_len));
    }

    if (Q != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->Q, Q, Q_len));
    }

    if (D != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->D, D, D_len));
    }

    if (E != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->E, E, E_len));
    }

cleanup:

    if (ret != 0) {
        return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }

    return 0;
}

/*
 * Checks whether the context fields are set in such a way
 * that the RSA primitives will be able to execute without error.
 * It does *not* make guarantees for consistency of the parameters.
 */
static int rsa_check_context(mbedtls_rsa_context const *ctx, int is_priv,
                             int blinding_needed)
{
#if !defined(MBEDTLS_RSA_NO_CRT)
    /* blinding_needed is only used for NO_CRT to decide whether
     * P,Q need to be present or not. */
    ((void) blinding_needed);
#endif

    if (ctx->len != mbedtls_mpi_size(&ctx->N) ||
        ctx->len > MBEDTLS_MPI_MAX_SIZE) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    /*
     * 1. Modular exponentiation needs positive, odd moduli.
     */

    /* Modular exponentiation wrt. N is always used for
     * RSA public key operations. */
    if (mbedtls_mpi_cmp_int(&ctx->N, 0) <= 0 ||
        mbedtls_mpi_get_bit(&ctx->N, 0) == 0) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Modular exponentiation for P and Q is only
     * used for private key operations and if CRT
     * is used. */
    if (is_priv &&
        (mbedtls_mpi_cmp_int(&ctx->P, 0) <= 0 ||
         mbedtls_mpi_get_bit(&ctx->P, 0) == 0 ||
         mbedtls_mpi_cmp_int(&ctx->Q, 0) <= 0 ||
         mbedtls_mpi_get_bit(&ctx->Q, 0) == 0)) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif /* !MBEDTLS_RSA_NO_CRT */

    /*
     * 2. Exponents must be positive
     */

    /* Always need E for public key operations */
    if (mbedtls_mpi_cmp_int(&ctx->E, 0) <= 0) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

#if defined(MBEDTLS_RSA_NO_CRT)
    /* For private key operations, use D or DP & DQ
     * as (unblinded) exponents. */
    if (is_priv && mbedtls_mpi_cmp_int(&ctx->D, 0) <= 0) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#else
    if (is_priv &&
        (mbedtls_mpi_cmp_int(&ctx->DP, 0) <= 0 ||
         mbedtls_mpi_cmp_int(&ctx->DQ, 0) <= 0)) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Blinding shouldn't make exponents negative either,
     * so check that P, Q >= 1 if that hasn't yet been
     * done as part of 1. */
#if defined(MBEDTLS_RSA_NO_CRT)
    if (is_priv && blinding_needed &&
        (mbedtls_mpi_cmp_int(&ctx->P, 0) <= 0 ||
         mbedtls_mpi_cmp_int(&ctx->Q, 0) <= 0)) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif

    /* It wouldn't lead to an error if it wasn't satisfied,
     * but check for QP >= 1 nonetheless. */
#if !defined(MBEDTLS_RSA_NO_CRT)
    if (is_priv &&
        mbedtls_mpi_cmp_int(&ctx->QP, 0) <= 0) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
#endif

    return 0;
}

int mbedtls_rsa_complete(mbedtls_rsa_context *ctx)
{
    int ret = 0;
    int have_N, have_P, have_Q, have_D, have_E;
#if !defined(MBEDTLS_RSA_NO_CRT)
    int have_DP, have_DQ, have_QP;
#endif
    int n_missing, pq_missing, d_missing, is_pub, is_priv;

    RSA_VALIDATE_RET(ctx != NULL);

    have_N = (mbedtls_mpi_cmp_int(&ctx->N, 0) != 0);
    have_P = (mbedtls_mpi_cmp_int(&ctx->P, 0) != 0);
    have_Q = (mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0);
    have_D = (mbedtls_mpi_cmp_int(&ctx->D, 0) != 0);
    have_E = (mbedtls_mpi_cmp_int(&ctx->E, 0) != 0);

#if !defined(MBEDTLS_RSA_NO_CRT)
    have_DP = (mbedtls_mpi_cmp_int(&ctx->DP, 0) != 0);
    have_DQ = (mbedtls_mpi_cmp_int(&ctx->DQ, 0) != 0);
    have_QP = (mbedtls_mpi_cmp_int(&ctx->QP, 0) != 0);
#endif

    /*
     * Check whether provided parameters are enough
     * to deduce all others. The following incomplete
     * parameter sets for private keys are supported:
     *
     * (1) P, Q missing.
     * (2) D and potentially N missing.
     *
     */

    n_missing  =              have_P &&  have_Q &&  have_D && have_E;
    pq_missing =   have_N && !have_P && !have_Q &&  have_D && have_E;
    d_missing  =              have_P &&  have_Q && !have_D && have_E;
    is_pub     =   have_N && !have_P && !have_Q && !have_D && have_E;

    /* These three alternatives are mutually exclusive */
    is_priv = n_missing || pq_missing || d_missing;

    if (!is_priv && !is_pub) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    /*
     * Step 1: Deduce N if P, Q are provided.
     */

    if (!have_N && have_P && have_Q) {
        if ((ret = mbedtls_mpi_mul_mpi(&ctx->N, &ctx->P,
                                       &ctx->Q)) != 0) {
            return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
        }

        ctx->len = mbedtls_mpi_size(&ctx->N);
    }

    /*
     * Step 2: Deduce and verify all remaining core parameters.
     */

    if (pq_missing) {
        ret = mbedtls_rsa_deduce_primes(&ctx->N, &ctx->E, &ctx->D,
                                        &ctx->P, &ctx->Q);
        if (ret != 0) {
            return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
        }

    } else if (d_missing) {
        if ((ret = mbedtls_rsa_deduce_private_exponent(&ctx->P,
                                                       &ctx->Q,
                                                       &ctx->E,
                                                       &ctx->D)) != 0) {
            return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
        }
    }

    /*
     * Step 3: Deduce all additional parameters specific
     *         to our current RSA implementation.
     */

#if !defined(MBEDTLS_RSA_NO_CRT)
    if (is_priv && !(have_DP && have_DQ && have_QP)) {
        ret = mbedtls_rsa_deduce_crt(&ctx->P,  &ctx->Q,  &ctx->D,
                                     &ctx->DP, &ctx->DQ, &ctx->QP);
        if (ret != 0) {
            return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
        }
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /*
     * Step 3: Basic sanity checks
     */

    return rsa_check_context(ctx, is_priv, 1);
}

int mbedtls_rsa_export_raw(const mbedtls_rsa_context *ctx,
                           unsigned char *N, size_t N_len,
                           unsigned char *P, size_t P_len,
                           unsigned char *Q, size_t Q_len,
                           unsigned char *D, size_t D_len,
                           unsigned char *E, size_t E_len)
{
    int ret = 0;
    int is_priv;
    RSA_VALIDATE_RET(ctx != NULL);

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

    if (!is_priv) {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if (P != NULL || Q != NULL || D != NULL) {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        }

    }

    if (N != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->N, N, N_len));
    }

    if (P != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->P, P, P_len));
    }

    if (Q != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->Q, Q, Q_len));
    }

    if (D != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->D, D, D_len));
    }

    if (E != NULL) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->E, E, E_len));
    }

cleanup:

    return ret;
}

int mbedtls_rsa_export(const mbedtls_rsa_context *ctx,
                       mbedtls_mpi *N, mbedtls_mpi *P, mbedtls_mpi *Q,
                       mbedtls_mpi *D, mbedtls_mpi *E)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    int is_priv;
    RSA_VALIDATE_RET(ctx != NULL);

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

    if (!is_priv) {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if (P != NULL || Q != NULL || D != NULL) {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        }

    }

    /* Export all requested core parameters. */

    if ((N != NULL && (ret = mbedtls_mpi_copy(N, &ctx->N)) != 0) ||
        (P != NULL && (ret = mbedtls_mpi_copy(P, &ctx->P)) != 0) ||
        (Q != NULL && (ret = mbedtls_mpi_copy(Q, &ctx->Q)) != 0) ||
        (D != NULL && (ret = mbedtls_mpi_copy(D, &ctx->D)) != 0) ||
        (E != NULL && (ret = mbedtls_mpi_copy(E, &ctx->E)) != 0)) {
        return ret;
    }

    return 0;
}

/*
 * Export CRT parameters
 * This must also be implemented if CRT is not used, for being able to
 * write DER encoded RSA keys. The helper function mbedtls_rsa_deduce_crt
 * can be used in this case.
 */
int mbedtls_rsa_export_crt(const mbedtls_rsa_context *ctx,
                           mbedtls_mpi *DP, mbedtls_mpi *DQ, mbedtls_mpi *QP)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    int is_priv;
    RSA_VALIDATE_RET(ctx != NULL);

    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
        mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

    if (!is_priv) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Export all requested blinding parameters. */
    if ((DP != NULL && (ret = mbedtls_mpi_copy(DP, &ctx->DP)) != 0) ||
        (DQ != NULL && (ret = mbedtls_mpi_copy(DQ, &ctx->DQ)) != 0) ||
        (QP != NULL && (ret = mbedtls_mpi_copy(QP, &ctx->QP)) != 0)) {
        return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }
#else
    if ((ret = mbedtls_rsa_deduce_crt(&ctx->P, &ctx->Q, &ctx->D,
                                      DP, DQ, QP)) != 0) {
        return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }
#endif

    return 0;
}

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx,
                      int padding,
                      int hash_id)
{
    RSA_VALIDATE(ctx != NULL);
    RSA_VALIDATE(padding == MBEDTLS_RSA_PKCS_V15 ||
                 padding == MBEDTLS_RSA_PKCS_V21);

    memset(ctx, 0, sizeof(mbedtls_rsa_context));

    mbedtls_rsa_set_padding(ctx, padding, hash_id);

#if defined(MBEDTLS_THREADING_C)
    /* Set ctx->ver to nonzero to indicate that the mutex has been
     * initialized and will need to be freed. */
    ctx->ver = 1;
    mbedtls_mutex_init(&ctx->mutex);
#endif
}

/*
 * Set padding for an existing RSA context
 */
void mbedtls_rsa_set_padding(mbedtls_rsa_context *ctx, int padding,
                             int hash_id)
{
    RSA_VALIDATE(ctx != NULL);
    RSA_VALIDATE(padding == MBEDTLS_RSA_PKCS_V15 ||
                 padding == MBEDTLS_RSA_PKCS_V21);

    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Get length in bytes of RSA modulus
 */

size_t mbedtls_rsa_get_len(const mbedtls_rsa_context *ctx)
{
    return ctx->len;
}


#if defined(MBEDTLS_GENPRIME)

/*
 * Generate an RSA keypair
 *
 * This generation method follows the RSA key pair generation procedure of
 * FIPS 186-4 if 2^16 < exponent < 2^256 and nbits = 2048 or nbits = 3072.
 */
int mbedtls_rsa_gen_key(mbedtls_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng,
                        unsigned int nbits, int exponent)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_mpi H, G, L;
    int prime_quality = 0;
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(f_rng != NULL);

    /*
     * If the modulus is 1024 bit long or shorter, then the security strength of
     * the RSA algorithm is less than or equal to 80 bits and therefore an error
     * rate of 2^-80 is sufficient.
     */
    if (nbits > 1024) {
        prime_quality = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;
    }

    mbedtls_mpi_init(&H);
    mbedtls_mpi_init(&G);
    mbedtls_mpi_init(&L);

    if (nbits < 128 || exponent < 3 || nbits % 2 != 0) {
        ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    /*
     * find primes P and Q with Q < P so that:
     * 1.  |P-Q| > 2^( nbits / 2 - 100 )
     * 2.  GCD( E, (P-1)*(Q-1) ) == 1
     * 3.  E^-1 mod LCM(P-1, Q-1) > 2^( nbits / 2 )
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ctx->E, exponent));

    do {
        MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime(&ctx->P, nbits >> 1,
                                              prime_quality, f_rng, p_rng));

        MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime(&ctx->Q, nbits >> 1,
                                              prime_quality, f_rng, p_rng));

        /* make sure the difference between p and q is not too small (FIPS 186-4 §B.3.3 step 5.4) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&H, &ctx->P, &ctx->Q));
        if (mbedtls_mpi_bitlen(&H) <= ((nbits >= 200) ? ((nbits >> 1) - 99) : 0)) {
            continue;
        }

        /* not required by any standards, but some users rely on the fact that P > Q */
        if (H.s < 0) {
            mbedtls_mpi_swap(&ctx->P, &ctx->Q);
        }

        /* Temporarily replace P,Q by P-1, Q-1 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->P, &ctx->P, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->Q, &ctx->Q, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&H, &ctx->P, &ctx->Q));

        /* check GCD( E, (P-1)*(Q-1) ) == 1 (FIPS 186-4 §B.3.1 criterion 2(a)) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->E, &H));
        if (mbedtls_mpi_cmp_int(&G, 1) != 0) {
            continue;
        }

        /* compute smallest possible D = E^-1 mod LCM(P-1, Q-1) (FIPS 186-4 §B.3.1 criterion 3(b)) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->P, &ctx->Q));
        MBEDTLS_MPI_CHK(mbedtls_mpi_div_mpi(&L, NULL, &H, &G));
        MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&ctx->D, &ctx->E, &L));

        if (mbedtls_mpi_bitlen(&ctx->D) <= ((nbits + 1) / 2)) {      // (FIPS 186-4 §B.3.1 criterion 3(a))
            continue;
        }

        break;
    } while (1);

    /* Restore P,Q */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->P,  &ctx->P, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->Q,  &ctx->Q, 1));

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&ctx->N, &ctx->P, &ctx->Q));

    ctx->len = mbedtls_mpi_size(&ctx->N);

#if !defined(MBEDTLS_RSA_NO_CRT)
    /*
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MBEDTLS_MPI_CHK(mbedtls_rsa_deduce_crt(&ctx->P, &ctx->Q, &ctx->D,
                                           &ctx->DP, &ctx->DQ, &ctx->QP));
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Double-check */
    MBEDTLS_MPI_CHK(mbedtls_rsa_check_privkey(ctx));

cleanup:

    mbedtls_mpi_free(&H);
    mbedtls_mpi_free(&G);
    mbedtls_mpi_free(&L);

    if (ret != 0) {
        mbedtls_rsa_free(ctx);

        if ((-ret & ~0x7f) == 0) {
            ret = MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_KEY_GEN_FAILED, ret);
        }
        return ret;
    }

    return 0;
}

#endif /* MBEDTLS_GENPRIME */

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx)
{
    RSA_VALIDATE_RET(ctx != NULL);

    if (rsa_check_context(ctx, 0 /* public */, 0 /* no blinding */) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    if (mbedtls_mpi_bitlen(&ctx->N) < 128) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    if (mbedtls_mpi_get_bit(&ctx->E, 0) == 0 ||
        mbedtls_mpi_bitlen(&ctx->E)     < 2  ||
        mbedtls_mpi_cmp_mpi(&ctx->E, &ctx->N) >= 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    return 0;
}

/*
 * Check for the consistency of all fields in an RSA private key context
 */
int mbedtls_rsa_check_privkey(const mbedtls_rsa_context *ctx)
{
    RSA_VALIDATE_RET(ctx != NULL);

    if (mbedtls_rsa_check_pubkey(ctx) != 0 ||
        rsa_check_context(ctx, 1 /* private */, 1 /* blinding */) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    if (mbedtls_rsa_validate_params(&ctx->N, &ctx->P, &ctx->Q,
                                    &ctx->D, &ctx->E, NULL, NULL) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    else if (mbedtls_rsa_validate_crt(&ctx->P, &ctx->Q, &ctx->D,
                                      &ctx->DP, &ctx->DQ, &ctx->QP) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }
#endif

    return 0;
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv(const mbedtls_rsa_context *pub,
                               const mbedtls_rsa_context *prv)
{
    RSA_VALIDATE_RET(pub != NULL);
    RSA_VALIDATE_RET(prv != NULL);

    if (mbedtls_rsa_check_pubkey(pub)  != 0 ||
        mbedtls_rsa_check_privkey(prv) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    if (mbedtls_mpi_cmp_mpi(&pub->N, &prv->N) != 0 ||
        mbedtls_mpi_cmp_mpi(&pub->E, &prv->E) != 0) {
        return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

    return 0;
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-ENCRYPT function
 */
int mbedtls_rsa_rsaes_oaep_encrypt(mbedtls_rsa_context *ctx,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng,
                                   int mode,
                                   const unsigned char *label, size_t label_len,
                                   size_t ilen,
                                   const unsigned char *input,
                                   unsigned char *output)
{

    return mbedtls_hw_rsa_oaep_encrypt(ctx, mode, label, label_len, ilen, input, output);
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_encrypt(mbedtls_rsa_context *ctx,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng,
                                        int mode, size_t ilen,
                                        const unsigned char *input,
                                        unsigned char *output)
{
    return mbedtls_hw_rsa_pkcs1_v15_encrypt(ctx, mode, ilen, input, output);
}

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t ilen,
                              const unsigned char *input,
                              unsigned char *output)
{
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE ||
                     mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET(output != NULL);
    RSA_VALIDATE_RET(ilen == 0 || input != NULL);

    switch (ctx->padding) {
        case MBEDTLS_RSA_PKCS_V15:
                return mbedtls_hw_rsa_pkcs1_v15_encrypt(ctx, mode, ilen, input, output);

        case MBEDTLS_RSA_PKCS_V21:
                return mbedtls_hw_rsa_oaep_encrypt(ctx, mode, NULL, 0, ilen, input, output);
        default:
            return MBEDTLS_ERR_RSA_INVALID_PADDING;
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-DECRYPT function
 */
int mbedtls_rsa_rsaes_oaep_decrypt(mbedtls_rsa_context *ctx,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng,
                                   int mode,
                                   const unsigned char *label, size_t label_len,
                                   size_t *olen,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   size_t output_max_len)
{

    return mbedtls_hw_rsa_oaep_decrypt(ctx, mode, label, label_len, olen, input, output, output_max_len);
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-DECRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_decrypt(mbedtls_rsa_context *ctx,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng,
                                        int mode,
                                        size_t *olen,
                                        const unsigned char *input,
                                        unsigned char *output,
                                        size_t output_max_len)
{

    return mbedtls_hw_rsa_pkcs1_v15_decrypt(ctx, mode, olen, input, output, output_max_len);
}


/*
 * Do an RSA operation, then remove the message padding
 */
int mbedtls_rsa_pkcs1_decrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t *olen,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t output_max_len)
{
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE ||
                     mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET(output_max_len == 0 || output != NULL);
    RSA_VALIDATE_RET(input != NULL);
    RSA_VALIDATE_RET(olen != NULL);

    switch (ctx->padding) {
        case MBEDTLS_RSA_PKCS_V15:
                return mbedtls_hw_rsa_pkcs1_v15_decrypt(ctx, mode, olen, input, output, output_max_len);

        case MBEDTLS_RSA_PKCS_V21:
                return mbedtls_hw_rsa_oaep_decrypt(ctx, mode, NULL, 0, olen, input, output, output_max_len);

        default:
            return MBEDTLS_ERR_RSA_INVALID_PADDING;
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function with
 * the option to pass in the salt length.
 */
int mbedtls_rsa_rsassa_pss_sign_ext(mbedtls_rsa_context *ctx,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng,
                                    mbedtls_md_type_t md_alg,
                                    unsigned int hashlen,
                                    const unsigned char *hash,
                                    int saltlen,
                                    unsigned char *sig)
{
    return mbedtls_hw_rsa_oaep_sign(ctx, MBEDTLS_RSA_PRIVATE, md_alg,
                                    hashlen, hash, saltlen, sig);
}


/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function
 */
int mbedtls_rsa_rsassa_pss_sign(mbedtls_rsa_context *ctx,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng,
                                int mode,
                                mbedtls_md_type_t md_alg,
                                unsigned int hashlen,
                                const unsigned char *hash,
                                unsigned char *sig)
{
    return mbedtls_hw_rsa_oaep_sign(ctx, mode, md_alg,
                                    hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, sig);
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_rsassa_pkcs1_v15_sign(mbedtls_rsa_context *ctx,
                                      int (*f_rng)(void *, unsigned char *, size_t),
                                      void *p_rng,
                                      int mode,
                                      mbedtls_md_type_t md_alg,
                                      unsigned int hashlen,
                                      const unsigned char *hash,
                                      unsigned char *sig)
{
    return mbedtls_hw_rsa_pkcs1_v15_sign(ctx, mode, md_alg, hashlen, hash, sig);
}

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign(mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           unsigned char *sig)
{
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE ||
                     mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET((md_alg  == MBEDTLS_MD_NONE &&
                      hashlen == 0) ||
                      hash != NULL);
    RSA_VALIDATE_RET(sig != NULL);

    switch (ctx->padding) {
        case MBEDTLS_RSA_PKCS_V15:
                return mbedtls_hw_rsa_pkcs1_v15_sign(ctx, mode, md_alg, hashlen, hash, sig);

        case MBEDTLS_RSA_PKCS_V21:
                return mbedtls_hw_rsa_oaep_sign(ctx, mode, md_alg, hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, sig);

        default:
            return MBEDTLS_ERR_RSA_INVALID_PADDING;
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify_ext(mbedtls_rsa_context *ctx,
                                      int (*f_rng)(void *, unsigned char *, size_t),
                                      void *p_rng,
                                      int mode,
                                      mbedtls_md_type_t md_alg,
                                      unsigned int hashlen,
                                      const unsigned char *hash,
                                      mbedtls_md_type_t mgf1_hash_id,
                                      int expected_salt_len,
                                      const unsigned char *sig)
{
    return mbedtls_hw_rsa_oaep_verify(ctx, mode, md_alg, hashlen, hash,
                                      expected_salt_len, sig);
}

/*
 * Simplified PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify(mbedtls_rsa_context *ctx,
                                  int (*f_rng)(void *, unsigned char *, size_t),
                                  void *p_rng,
                                  int mode,
                                  mbedtls_md_type_t md_alg,
                                  unsigned int hashlen,
                                  const unsigned char *hash,
                                  const unsigned char *sig)
{
    mbedtls_md_type_t mgf1_hash_id;
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE ||
                     mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET(sig != NULL);
    RSA_VALIDATE_RET((md_alg  == MBEDTLS_MD_NONE &&
                      hashlen == 0) ||
                     hash != NULL);

    mgf1_hash_id = (ctx->hash_id != MBEDTLS_MD_NONE)
                             ? (mbedtls_md_type_t) ctx->hash_id
                             : md_alg;

    return mbedtls_rsa_rsassa_pss_verify_ext(ctx, f_rng, p_rng, mode,
                                             md_alg, hashlen, hash,
                                             mgf1_hash_id, MBEDTLS_RSA_SALT_LEN_ANY,
                                             sig);

}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
 */
int mbedtls_rsa_rsassa_pkcs1_v15_verify(mbedtls_rsa_context *ctx,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng,
                                        int mode,
                                        mbedtls_md_type_t md_alg,
                                        unsigned int hashlen,
                                        const unsigned char *hash,
                                        const unsigned char *sig)
{
    return mbedtls_hw_rsa_pkcs1_v15_verify(ctx, mode, md_alg, hashlen, hash, sig);
}

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng,
                             int mode,
                             mbedtls_md_type_t md_alg,
                             unsigned int hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig)
{
    RSA_VALIDATE_RET(ctx != NULL);
    RSA_VALIDATE_RET(mode == MBEDTLS_RSA_PRIVATE ||
                     mode == MBEDTLS_RSA_PUBLIC);
    RSA_VALIDATE_RET(sig != NULL);
    RSA_VALIDATE_RET((md_alg  == MBEDTLS_MD_NONE &&
                      hashlen == 0) ||
                     hash != NULL);

    switch (ctx->padding) {
        case MBEDTLS_RSA_PKCS_V15:
                return mbedtls_hw_rsa_pkcs1_v15_verify(ctx, mode, md_alg, hashlen, hash, sig);

        case MBEDTLS_RSA_PKCS_V21:
                return mbedtls_hw_rsa_oaep_verify(ctx, mode, md_alg, hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, sig);

        default:
            return MBEDTLS_ERR_RSA_INVALID_PADDING;
    }
}

/*
 * Copy the components of an RSA key
 */
int mbedtls_rsa_copy(mbedtls_rsa_context *dst, const mbedtls_rsa_context *src)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    RSA_VALIDATE_RET(dst != NULL);
    RSA_VALIDATE_RET(src != NULL);

    dst->len = src->len;

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->N, &src->N));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->E, &src->E));

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->D, &src->D));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->P, &src->P));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Q, &src->Q));

#if !defined(MBEDTLS_RSA_NO_CRT)
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->DP, &src->DP));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->DQ, &src->DQ));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->QP, &src->QP));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RP, &src->RP));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RQ, &src->RQ));
#endif

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RN, &src->RN));

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Vi, &src->Vi));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Vf, &src->Vf));

    dst->padding = src->padding;
    dst->hash_id = src->hash_id;

cleanup:
    if (ret != 0) {
        mbedtls_rsa_free(dst);
    }

    return ret;
}

/*
 * Free the components of an RSA key
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_mpi_free(&ctx->Vi);
    mbedtls_mpi_free(&ctx->Vf);
    mbedtls_mpi_free(&ctx->RN);
    mbedtls_mpi_free(&ctx->D);
    mbedtls_mpi_free(&ctx->Q);
    mbedtls_mpi_free(&ctx->P);
    mbedtls_mpi_free(&ctx->E);
    mbedtls_mpi_free(&ctx->N);

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_free(&ctx->RQ);
    mbedtls_mpi_free(&ctx->RP);
    mbedtls_mpi_free(&ctx->QP);
    mbedtls_mpi_free(&ctx->DQ);
    mbedtls_mpi_free(&ctx->DP);
#endif /* MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_THREADING_C)
    /* Free the mutex, but only if it hasn't been freed already. */
    if (ctx->ver != 0) {
        mbedtls_mutex_free(&ctx->mutex);
        ctx->ver = 0;
    }
#endif
}

#endif /* MBEDTLS_RSA_ALT */

#endif /* MBEDTLS_RSA_C */
