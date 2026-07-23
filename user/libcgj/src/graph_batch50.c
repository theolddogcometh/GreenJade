/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50: AES_ecb_encrypt_block convenience wrapper.
 * Full AES_set_encrypt_key / AES_encrypt live in batch45 (no multi-def).
 * Integer/pointer only (no SSE). Clean-room public ABI.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch50_marker[] = "libcgj-batch50";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — opaque match for key type. */
#define AES_MAXNR 14

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *pKey);
void AES_encrypt(const unsigned char *pIn, unsigned char *pOut,
                 const AES_KEY *pKey);

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
