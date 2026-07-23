/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch72: PBKDF2-HMAC-SM3 (RFC 8018 / PKCS #5 with
 * SM3 as the PRF hash). Uses SM3_Init / SM3_Update / SM3_Final from
 * batch42 (extern only; no SM3 reimplementation). Does NOT define scrypt
 * (batch46). Integer/pointer only (no SSE). Clean-room public ABI.
 *
 * Surface:
 *   pbkdf2_hmac_sm3 / __pbkdf2_hmac_sm3
 *   __libcgj_batch72_marker = "libcgj-batch72"
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* SM3 from batch42 — digest 32, Merkle-Damgård block 64 */
void SM3_Init(void *pCtx);
void SM3_Update(void *pCtx, const void *pData, size_t cb);
void SM3_Final(unsigned char aDig[32], void *pCtx);

void *malloc(size_t cb);
void free(void *p);

const char __libcgj_batch72_marker[] = "libcgj-batch72";

#define B72_SM3_DIG 32u
#define B72_SM3_BLK 64u
/* Room for batch42 SM3 context (struct ~112B) plus alignment pad */
#define B72_SM3_CTX 256u

/* ---- HMAC-SM3 (RFC 2104) — static; PRF for PBKDF2 ---------------------- */

static void
b72_hmac_sm3(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
             unsigned char aOut[B72_SM3_DIG])
{
    unsigned char aKey[B72_SM3_BLK];
    unsigned char aIpad[B72_SM3_BLK];
    unsigned char aOpad[B72_SM3_BLK];
    unsigned char aInner[B72_SM3_DIG];
    unsigned char aCtx[B72_SM3_CTX];
    size_t i;

    if (aOut == NULL) {
        return;
    }

    memset(aKey, 0, B72_SM3_BLK);
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > B72_SM3_BLK) {
            memset(aCtx, 0, sizeof(aCtx));
            SM3_Init(aCtx);
            SM3_Update(aCtx, pKey, cbKey);
            SM3_Final(aKey, aCtx);
        } else {
            memcpy(aKey, pKey, cbKey);
        }
    }

    for (i = 0; i < B72_SM3_BLK; i++) {
        aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }

    memset(aCtx, 0, sizeof(aCtx));
    SM3_Init(aCtx);
    SM3_Update(aCtx, aIpad, B72_SM3_BLK);
    if (pData != NULL && cbData > 0u) {
        SM3_Update(aCtx, pData, cbData);
    }
    SM3_Final(aInner, aCtx);

    memset(aCtx, 0, sizeof(aCtx));
    SM3_Init(aCtx);
    SM3_Update(aCtx, aOpad, B72_SM3_BLK);
    SM3_Update(aCtx, aInner, B72_SM3_DIG);
    SM3_Final(aOut, aCtx);

    memset(aKey, 0, sizeof(aKey));
    memset(aIpad, 0, sizeof(aIpad));
    memset(aOpad, 0, sizeof(aOpad));
    memset(aInner, 0, sizeof(aInner));
    memset(aCtx, 0, sizeof(aCtx));
}

/* ---- PBKDF2-HMAC-SM3 (RFC 8018) ---------------------------------------- */

int
pbkdf2_hmac_sm3(const unsigned char *pwd, size_t plen,
                const unsigned char *salt, size_t slen, unsigned iterations,
                unsigned char *out, size_t outlen)
{
    unsigned char aU[B72_SM3_DIG];
    unsigned char aT[B72_SM3_DIG];
    unsigned char *pBlock;
    unsigned uBlk = 1;
    size_t nOff = 0;
    size_t cbBlock;

    if (out == NULL || outlen == 0u || iterations == 0u) {
        errno = EINVAL;
        return -1;
    }
    if (pwd == NULL) {
        pwd = (const unsigned char *)"";
        plen = 0;
    }
    if (salt == NULL) {
        salt = (const unsigned char *)"";
        slen = 0;
    }
    if (slen > (size_t)(SIZE_MAX - 4u)) {
        errno = EINVAL;
        return -1;
    }

    cbBlock = slen + 4u;
    pBlock = (unsigned char *)malloc(cbBlock);
    if (pBlock == NULL) {
        errno = ENOMEM;
        return -1;
    }
    if (slen > 0u) {
        memcpy(pBlock, salt, slen);
    }

    while (nOff < outlen) {
        size_t i;
        size_t cbCopy;
        unsigned r;

        pBlock[slen] = (unsigned char)(uBlk >> 24);
        pBlock[slen + 1u] = (unsigned char)(uBlk >> 16);
        pBlock[slen + 2u] = (unsigned char)(uBlk >> 8);
        pBlock[slen + 3u] = (unsigned char)uBlk;

        b72_hmac_sm3(pwd, plen, pBlock, cbBlock, aU);
        memcpy(aT, aU, B72_SM3_DIG);
        for (r = 1; r < iterations; r++) {
            b72_hmac_sm3(pwd, plen, aU, B72_SM3_DIG, aU);
            for (i = 0; i < B72_SM3_DIG; i++) {
                aT[i] ^= aU[i];
            }
        }

        cbCopy = (outlen - nOff < B72_SM3_DIG) ? (outlen - nOff) : B72_SM3_DIG;
        memcpy(out + nOff, aT, cbCopy);
        nOff += cbCopy;
        uBlk++;
    }

    memset(pBlock, 0, cbBlock);
    free(pBlock);
    memset(aU, 0, sizeof(aU));
    memset(aT, 0, sizeof(aT));
    return 0;
}

int __pbkdf2_hmac_sm3(const unsigned char *pwd, size_t plen,
                      const unsigned char *salt, size_t slen,
                      unsigned iterations, unsigned char *out, size_t outlen)
    __attribute__((alias("pbkdf2_hmac_sm3")));
