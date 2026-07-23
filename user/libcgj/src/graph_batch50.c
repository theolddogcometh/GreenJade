/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50: AES one-shot / multi-block ECB convenience
 * wrappers over batch45 AES_set_*_key / AES_encrypt / AES_decrypt.
 * Does not redefine AES core (no multi-def). Integer/pointer only (no SSE).
 * Clean-room public ABI.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const char __libcgj_batch50_marker[] = "libcgj-batch50";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — opaque match for key type. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *pKey);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *pKey);
void AES_encrypt(const unsigned char *pIn, unsigned char *pOut,
                 const AES_KEY *pKey);
void AES_decrypt(const unsigned char *pIn, unsigned char *pOut,
                 const AES_KEY *pKey);

/* ---- block size probe (unique) ------------------------------------------ */

size_t
AES_block_size(void)
{
    return (size_t)AES_BLOCK_SIZE;
}

size_t
__AES_block_size(void) __attribute__((alias("AES_block_size")));

/* One-shot 16-byte encrypt (unique to batch50; calls batch45 AES). */
int
AES_ecb_encrypt_block(const unsigned char *pIn, unsigned char *pOut,
                      const unsigned char *pUserKey, int nBits)
{
    AES_KEY key;
    int nRc;

    if (pIn == NULL || pOut == NULL || pUserKey == NULL) {
        return -1;
    }
    nRc = AES_set_encrypt_key(pUserKey, nBits, &key);
    if (nRc != 0) {
        return nRc;
    }
    AES_encrypt(pIn, pOut, &key);
    return 0;
}

int __AES_ecb_encrypt_block(const unsigned char *pIn, unsigned char *pOut,
                            const unsigned char *pUserKey, int nBits)
    __attribute__((alias("AES_ecb_encrypt_block")));

/* One-shot 16-byte decrypt (unique; pair of AES_ecb_encrypt_block). */
int
AES_ecb_decrypt_block(const unsigned char *pIn, unsigned char *pOut,
                      const unsigned char *pUserKey, int nBits)
{
    AES_KEY key;
    int nRc;

    if (pIn == NULL || pOut == NULL || pUserKey == NULL) {
        return -1;
    }
    nRc = AES_set_decrypt_key(pUserKey, nBits, &key);
    if (nRc != 0) {
        return nRc;
    }
    AES_decrypt(pIn, pOut, &key);
    return 0;
}

int __AES_ecb_decrypt_block(const unsigned char *pIn, unsigned char *pOut,
                            const unsigned char *pUserKey, int nBits)
    __attribute__((alias("AES_ecb_decrypt_block")));

/*
 * OpenSSL-shaped AES_ecb_encrypt: in/out may alias; enc != 0 encrypts.
 * Distinct name from AES_ecb_encrypt_block (one-shot key material).
 */
void
AES_ecb_encrypt(const unsigned char *pIn, unsigned char *pOut,
                const AES_KEY *pKey, const int nEnc)
{
    if (pIn == NULL || pOut == NULL || pKey == NULL) {
        return;
    }
    if (nEnc != 0) {
        AES_encrypt(pIn, pOut, pKey);
    } else {
        AES_decrypt(pIn, pOut, pKey);
    }
}

void __AES_ecb_encrypt(const unsigned char *pIn, unsigned char *pOut,
                       const AES_KEY *pKey, const int nEnc)
    __attribute__((alias("AES_ecb_encrypt")));

/*
 * Multi-block ECB: cb must be a multiple of 16. Processes in place-safe
 * order (uses temp when pIn == pOut would still be fine since block-local).
 */
int
AES_ecb_encrypt_blocks(const unsigned char *pIn, unsigned char *pOut,
                       size_t cb, const AES_KEY *pKey, int nEnc)
{
    size_t i;
    unsigned char aTmp[AES_BLOCK_SIZE];

    if (pKey == NULL) {
        return -1;
    }
    if (cb == 0u) {
        return 0;
    }
    if (pIn == NULL || pOut == NULL || (cb % (size_t)AES_BLOCK_SIZE) != 0u) {
        return -1;
    }
    for (i = 0; i < cb; i += (size_t)AES_BLOCK_SIZE) {
        if (nEnc != 0) {
            AES_encrypt(pIn + i, aTmp, pKey);
        } else {
            AES_decrypt(pIn + i, aTmp, pKey);
        }
        memcpy(pOut + i, aTmp, (size_t)AES_BLOCK_SIZE);
    }
    return 0;
}

int __AES_ecb_encrypt_blocks(const unsigned char *pIn, unsigned char *pOut,
                             size_t cb, const AES_KEY *pKey, int nEnc)
    __attribute__((alias("AES_ecb_encrypt_blocks")));

/* One-shot multi-block with raw key material (setup + ECB). */
int
AES_ecb_crypt_n(const unsigned char *pIn, unsigned char *pOut, size_t cb,
                const unsigned char *pUserKey, int nBits, int nEnc)
{
    AES_KEY key;
    int nRc;

    if (pUserKey == NULL) {
        return -1;
    }
    if (nEnc != 0) {
        nRc = AES_set_encrypt_key(pUserKey, nBits, &key);
    } else {
        nRc = AES_set_decrypt_key(pUserKey, nBits, &key);
    }
    if (nRc != 0) {
        return nRc;
    }
    return AES_ecb_encrypt_blocks(pIn, pOut, cb, &key, nEnc);
}

int __AES_ecb_crypt_n(const unsigned char *pIn, unsigned char *pOut, size_t cb,
                      const unsigned char *pUserKey, int nBits, int nEnc)
    __attribute__((alias("AES_ecb_crypt_n")));
