/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch71: HKDF-Extract / HKDF-Expand (RFC 5869)
 * for SHA-256 and SHA-512 as separate steps.
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   hkdf_sha256 / hkdf_sha512 / __hkdf_sha256 / __hkdf_sha512
 *     → graph_batch40.c (combined Extract+Expand one-shot)
 *   hmac_sha256 → graph_batch38.c
 *   hmac_sha512 → graph_batch39.c
 *   SHA256_* / SHA512_* → graph_batch31/32
 *
 * This TU adds only symbols that were missing:
 *   hkdf_sha256_extract / hkdf_sha256_expand
 *   hkdf_sha512_extract / hkdf_sha512_expand
 *   __hkdf_sha256_extract / __hkdf_sha256_expand
 *   __hkdf_sha512_extract / __hkdf_sha512_expand
 *   __libcgj_batch71_marker = "libcgj-batch71"
 *
 * Freestanding pure C, integer/pointer only (no SSE). Clean-room RFC 5869.
 * Extract: PRK = HMAC-Hash(salt, IKM); null/empty salt → HashLen zeros.
 * Expand:  OKM = first L octets of T(1)|T(2)|… with
 *          T(i) = HMAC-Hash(PRK, T(i-1)|info|i), T(0)=empty.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch71_marker[] = "libcgj-batch71";

/* Existing HMAC surface (batch38/39) — extern only. */
void hmac_sha256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[32]);
void hmac_sha512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[64]);

#define B71_SHA256_LEN 32u
#define B71_SHA512_LEN 64u
/* Same info cap as batch40 combined HKDF (stack buffer bound). */
#define B71_INFO_MAX 256u

/* --------------------------------------------------------------------------
 * Freestanding mem helpers (no libc string dependency in this TU)
 * -------------------------------------------------------------------------- */

static void
b71_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        d[i] = s[i];
    }
}

static void
b71_memzero(void *pDst, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

/* --------------------------------------------------------------------------
 * HKDF-Extract (RFC 5869 §2.2)
 * -------------------------------------------------------------------------- */

/*
 * hkdf_sha256_extract — PRK = HMAC-SHA256(salt, IKM), HashLen = 32.
 * If pSalt == NULL or cbSalt == 0, salt is HashLen zero octets.
 * pIkm may be NULL only when cbIkm == 0.
 * Writes exactly 32 bytes to pPrk. Returns 0 on success, -1 on error.
 */
int
hkdf_sha256_extract(const void *pSalt, size_t cbSalt, const void *pIkm,
                    size_t cbIkm, unsigned char aPrk[32])
{
    static const unsigned char aZeros[B71_SHA256_LEN] = {0};

    if (aPrk == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pIkm == NULL && cbIkm != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (pSalt == NULL || cbSalt == 0u) {
        hmac_sha256(aZeros, B71_SHA256_LEN, pIkm, cbIkm, aPrk);
    } else {
        hmac_sha256(pSalt, cbSalt, pIkm, cbIkm, aPrk);
    }
    return 0;
}

/*
 * hkdf_sha512_extract — PRK = HMAC-SHA512(salt, IKM), HashLen = 64.
 * Same salt-default rule as SHA-256. Writes exactly 64 bytes to pPrk.
 */
int
hkdf_sha512_extract(const void *pSalt, size_t cbSalt, const void *pIkm,
                    size_t cbIkm, unsigned char aPrk[64])
{
    static const unsigned char aZeros[B71_SHA512_LEN] = {0};

    if (aPrk == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pIkm == NULL && cbIkm != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (pSalt == NULL || cbSalt == 0u) {
        hmac_sha512(aZeros, B71_SHA512_LEN, pIkm, cbIkm, aPrk);
    } else {
        hmac_sha512(pSalt, cbSalt, pIkm, cbIkm, aPrk);
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * HKDF-Expand (RFC 5869 §2.3)
 * -------------------------------------------------------------------------- */

/*
 * hkdf_sha256_expand — expand PRK to cbOkm bytes of OKM.
 * pPrk must be non-NULL; cbPrk typically HashLen (32) but any length is
 * accepted (HMAC key schedule). cbOkm in 1..255*HashLen. cbInfo <= 256.
 * pInfo may be NULL only when cbInfo == 0.
 */
int
hkdf_sha256_expand(const unsigned char *pPrk, size_t cbPrk, const void *pInfo,
                   size_t cbInfo, unsigned char *pOkm, size_t cbOkm)
{
    unsigned char aT[B71_SHA256_LEN];
    unsigned char aBuf[B71_SHA256_LEN + B71_INFO_MAX + 1u];
    size_t cbT = 0;
    size_t nOff = 0;
    unsigned char uCtr = 1;

    if (pPrk == NULL || cbPrk == 0u || pOkm == NULL || cbOkm == 0u ||
        cbOkm > 255u * B71_SHA256_LEN || cbInfo > B71_INFO_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (pInfo == NULL && cbInfo != 0u) {
        errno = EINVAL;
        return -1;
    }

    while (nOff < cbOkm) {
        size_t n = 0;
        size_t cbCopy;

        if (cbT > 0u) {
            b71_memcpy(aBuf + n, aT, cbT);
            n += cbT;
        }
        if (pInfo != NULL && cbInfo > 0u) {
            b71_memcpy(aBuf + n, pInfo, cbInfo);
            n += cbInfo;
        }
        aBuf[n++] = uCtr;
        hmac_sha256(pPrk, cbPrk, aBuf, n, aT);
        cbT = B71_SHA256_LEN;
        cbCopy = (cbOkm - nOff < B71_SHA256_LEN) ? (cbOkm - nOff)
                                                 : B71_SHA256_LEN;
        b71_memcpy(pOkm + nOff, aT, cbCopy);
        nOff += cbCopy;
        uCtr++;
    }
    b71_memzero(aT, sizeof(aT));
    b71_memzero(aBuf, sizeof(aBuf));
    return 0;
}

/*
 * hkdf_sha512_expand — expand PRK to cbOkm bytes of OKM (HashLen = 64).
 * Same validation as SHA-256 expand with 64-byte block parameters.
 */
int
hkdf_sha512_expand(const unsigned char *pPrk, size_t cbPrk, const void *pInfo,
                   size_t cbInfo, unsigned char *pOkm, size_t cbOkm)
{
    unsigned char aT[B71_SHA512_LEN];
    unsigned char aBuf[B71_SHA512_LEN + B71_INFO_MAX + 1u];
    size_t cbT = 0;
    size_t nOff = 0;
    unsigned char uCtr = 1;

    if (pPrk == NULL || cbPrk == 0u || pOkm == NULL || cbOkm == 0u ||
        cbOkm > 255u * B71_SHA512_LEN || cbInfo > B71_INFO_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (pInfo == NULL && cbInfo != 0u) {
        errno = EINVAL;
        return -1;
    }

    while (nOff < cbOkm) {
        size_t n = 0;
        size_t cbCopy;

        if (cbT > 0u) {
            b71_memcpy(aBuf + n, aT, cbT);
            n += cbT;
        }
        if (pInfo != NULL && cbInfo > 0u) {
            b71_memcpy(aBuf + n, pInfo, cbInfo);
            n += cbInfo;
        }
        aBuf[n++] = uCtr;
        hmac_sha512(pPrk, cbPrk, aBuf, n, aT);
        cbT = B71_SHA512_LEN;
        cbCopy = (cbOkm - nOff < B71_SHA512_LEN) ? (cbOkm - nOff)
                                                 : B71_SHA512_LEN;
        b71_memcpy(pOkm + nOff, aT, cbCopy);
        nOff += cbCopy;
        uCtr++;
    }
    b71_memzero(aT, sizeof(aT));
    b71_memzero(aBuf, sizeof(aBuf));
    return 0;
}

/* Hidden aliases (glibc-style __* surface). */
int __hkdf_sha256_extract(const void *pSalt, size_t cbSalt, const void *pIkm,
                          size_t cbIkm, unsigned char aPrk[32])
    __attribute__((alias("hkdf_sha256_extract")));
int __hkdf_sha256_expand(const unsigned char *pPrk, size_t cbPrk,
                         const void *pInfo, size_t cbInfo, unsigned char *pOkm,
                         size_t cbOkm)
    __attribute__((alias("hkdf_sha256_expand")));
int __hkdf_sha512_extract(const void *pSalt, size_t cbSalt, const void *pIkm,
                          size_t cbIkm, unsigned char aPrk[64])
    __attribute__((alias("hkdf_sha512_extract")));
int __hkdf_sha512_expand(const unsigned char *pPrk, size_t cbPrk,
                         const void *pInfo, size_t cbInfo, unsigned char *pOkm,
                         size_t cbOkm)
    __attribute__((alias("hkdf_sha512_expand")));
