/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39: HMAC-MD5 / HMAC-SHA512, SHA-384 (via
 * SHA-512 compression), zlib-shaped crc32/adler32, Castagnoli crc32c,
 * and iconvctl. Integer/pointer only (no SSE). Clean-room public ABI
 * over batch31/32 MD5 and SHA512 Init/Update/Final.
 */
#include <errno.h>
#include <iconv.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Existing digests */
void MD5Init(void *pCtx);
void MD5Update(void *pCtx, const void *pData, size_t cb);
void MD5Final(unsigned char aDig[16], void *pCtx);
void SHA512_Init(void *pCtx);
void SHA512_Update(void *pCtx, const void *pData, size_t cb);
void SHA512_Final(unsigned char aDig[64], void *pCtx);

const char __libcgj_batch39_marker[] = "libcgj-batch39";

/* ---- HMAC-MD5 / HMAC-SHA512 (RFC 2104) ---------------------------------- */

void
hmac_md5(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
         unsigned char aOut[16])
{
    unsigned char aKey[64];
    unsigned char aIpad[64];
    unsigned char aOpad[64];
    unsigned char aInner[16];
    unsigned char aCtx[128];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aKey, 0, sizeof(aKey));
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > 64u) {
            memset(aCtx, 0, sizeof(aCtx));
            MD5Init(aCtx);
            MD5Update(aCtx, pKey, cbKey);
            MD5Final(aKey, aCtx);
        } else {
            memcpy(aKey, pKey, cbKey);
        }
    }
    for (i = 0; i < 64u; i++) {
        aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }
    memset(aCtx, 0, sizeof(aCtx));
    MD5Init(aCtx);
    MD5Update(aCtx, aIpad, 64u);
    if (pData != NULL && cbData > 0u) {
        MD5Update(aCtx, pData, cbData);
    }
    MD5Final(aInner, aCtx);
    memset(aCtx, 0, sizeof(aCtx));
    MD5Init(aCtx);
    MD5Update(aCtx, aOpad, 64u);
    MD5Update(aCtx, aInner, 16u);
    MD5Final(aOut, aCtx);
    memset(aKey, 0, sizeof(aKey));
    memset(aIpad, 0, sizeof(aIpad));
    memset(aOpad, 0, sizeof(aOpad));
    memset(aInner, 0, sizeof(aInner));
}

void
hmac_sha512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
            unsigned char aOut[64])
{
    unsigned char aKey[128];
    unsigned char aIpad[128];
    unsigned char aOpad[128];
    unsigned char aInner[64];
    unsigned char aCtx[256];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aKey, 0, sizeof(aKey));
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > 128u) {
            memset(aCtx, 0, sizeof(aCtx));
            SHA512_Init(aCtx);
            SHA512_Update(aCtx, pKey, cbKey);
            SHA512_Final(aKey, aCtx);
        } else {
            memcpy(aKey, pKey, cbKey);
        }
    }
    for (i = 0; i < 128u; i++) {
        aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }
    memset(aCtx, 0, sizeof(aCtx));
    SHA512_Init(aCtx);
    SHA512_Update(aCtx, aIpad, 128u);
    if (pData != NULL && cbData > 0u) {
        SHA512_Update(aCtx, pData, cbData);
    }
    SHA512_Final(aInner, aCtx);
    memset(aCtx, 0, sizeof(aCtx));
    SHA512_Init(aCtx);
    SHA512_Update(aCtx, aOpad, 128u);
    SHA512_Update(aCtx, aInner, 64u);
    SHA512_Final(aOut, aCtx);
    memset(aKey, 0, sizeof(aKey));
    memset(aIpad, 0, sizeof(aIpad));
    memset(aOpad, 0, sizeof(aOpad));
    memset(aInner, 0, sizeof(aInner));
}

void __hmac_md5(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                unsigned char aOut[16]) __attribute__((alias("hmac_md5")));
void __hmac_sha512(const void *pKey, size_t cbKey, const void *pData,
                   size_t cbData, unsigned char aOut[64])
    __attribute__((alias("hmac_sha512")));

/* ---- SHA-384 (SHA-512 compression, FIPS 180-4 IVs, 48-byte out) --------- */

/*
 * Context layout must match batch31 struct b31_sha512 so SHA512_Update/Final
 * may operate on it after SHA384_Init installs the SHA-384 IV.
 */
struct b39_sha384 {
    uint64_t aState[8];
    uint64_t aCount[2];
    unsigned char aBuf[128];
    unsigned char _pad[16];
};

void
SHA384_Init(void *pCtx)
{
    struct b39_sha384 *p = (struct b39_sha384 *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0xcbbb9d5dc1059ed8ull;
    p->aState[1] = 0x629a292a367cd507ull;
    p->aState[2] = 0x9159015a3070dd17ull;
    p->aState[3] = 0x152fecd8f70e5939ull;
    p->aState[4] = 0x67332667ffc00b31ull;
    p->aState[5] = 0x8eb44a8768581511ull;
    p->aState[6] = 0xdb0c2e0d64f98fa7ull;
    p->aState[7] = 0x47b5481dbefa4fa4ull;
}

void
SHA384_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b39_sha384 *p = (struct b39_sha384 *)pCtx;

    if (p == NULL) {
        return;
    }
    /* If never inited, install SHA-384 IV (do not fall through to SHA-512). */
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA384_Init(p);
    }
    SHA512_Update(pCtx, pData, cb);
}

void
SHA384_Final(unsigned char aDig[48], void *pCtx)
{
    unsigned char aFull[64];
    struct b39_sha384 *p = (struct b39_sha384 *)pCtx;

    if (aDig == NULL || p == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA384_Init(p);
    }
    SHA512_Final(aFull, pCtx);
    memcpy(aDig, aFull, 48u);
}

static char *
b39_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
{
    static char aLocal[128];
    static const char *szDigits = "0123456789abcdef";
    char *p;
    size_t i;

    p = (szBuf != NULL) ? szBuf : aLocal;
    for (i = 0; i < cb; i++) {
        p[i * 2u] = szDigits[(pDig[i] >> 4) & 0xfu];
        p[i * 2u + 1u] = szDigits[pDig[i] & 0xfu];
    }
    p[cb * 2u] = '\0';
    return p;
}

char *
SHA384_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b39_sha384)];
    unsigned char aDig[48];

    memset(aCtx, 0, sizeof(aCtx));
    SHA384_Init(aCtx);
    SHA384_Update(aCtx, pData, cb);
    SHA384_Final(aDig, aCtx);
    return b39_hex_end(aDig, 48u, szBuf);
}

void __SHA384_Init(void *p) __attribute__((alias("SHA384_Init")));
void __SHA384_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA384_Update")));
void __SHA384_Final(unsigned char a[48], void *p)
    __attribute__((alias("SHA384_Final")));
char *__SHA384_Data(const void *d, size_t n, char *s)
    __attribute__((alias("SHA384_Data")));

/* ---- zlib-shaped CRC-32 / Adler-32 / CRC-32C ---------------------------- */

static uint32_t s_b39_crc_tab[256];
static uint32_t s_b39_crc32c_tab[256];
static int s_b39_crc_ready;
static int s_b39_crc32c_ready;

static void
b39_crc_init_tab(void)
{
    uint32_t uPoly = 0xedb88320u;
    unsigned i;
    unsigned b;

    if (s_b39_crc_ready) {
        return;
    }
    for (i = 0; i < 256u; i++) {
        uint32_t uC = (uint32_t)i;

        for (b = 0; b < 8u; b++) {
            if ((uC & 1u) != 0u) {
                uC = uPoly ^ (uC >> 1);
            } else {
                uC >>= 1;
            }
        }
        s_b39_crc_tab[i] = uC;
    }
    s_b39_crc_ready = 1;
}

static void
b39_crc32c_init_tab(void)
{
    /* Castagnoli poly 0x1EDC6F41 reflected → 0x82F63B78 */
    uint32_t uPoly = 0x82f63b78u;
    unsigned i;
    unsigned b;

    if (s_b39_crc32c_ready) {
        return;
    }
    for (i = 0; i < 256u; i++) {
        uint32_t uC = (uint32_t)i;

        for (b = 0; b < 8u; b++) {
            if ((uC & 1u) != 0u) {
                uC = uPoly ^ (uC >> 1);
            } else {
                uC >>= 1;
            }
        }
        s_b39_crc32c_tab[i] = uC;
    }
    s_b39_crc32c_ready = 1;
}

unsigned long
crc32_z(unsigned long uCrc, const unsigned char *pBuf, size_t cb)
{
    size_t i;

    b39_crc_init_tab();
    if (pBuf == NULL) {
        return 0ul;
    }
    uCrc = uCrc ^ 0xfffffffful;
    for (i = 0; i < cb; i++) {
        uCrc = s_b39_crc_tab[(uCrc ^ pBuf[i]) & 0xffu] ^ (uCrc >> 8);
    }
    return uCrc ^ 0xfffffffful;
}

unsigned long
crc32(unsigned long uCrc, const unsigned char *pBuf, unsigned int cb)
{
    return crc32_z(uCrc, pBuf, (size_t)cb);
}

uint32_t
crc32c(uint32_t uCrc, const void *pBuf, size_t cb)
{
    const unsigned char *q = (const unsigned char *)pBuf;
    size_t i;

    b39_crc32c_init_tab();
    if (q == NULL) {
        return 0u;
    }
    uCrc = ~uCrc;
    for (i = 0; i < cb; i++) {
        uCrc = s_b39_crc32c_tab[(uCrc ^ q[i]) & 0xffu] ^ (uCrc >> 8);
    }
    return ~uCrc;
}

unsigned long
adler32_z(unsigned long uAdler, const unsigned char *pBuf, size_t cb)
{
    uint32_t uA;
    uint32_t uB;
    size_t i;
    const uint32_t uMod = 65521u;

    if (pBuf == NULL) {
        return 1ul;
    }
    uA = (uint32_t)(uAdler & 0xfffful);
    uB = (uint32_t)((uAdler >> 16) & 0xfffful);
    for (i = 0; i < cb; i++) {
        uA = (uA + pBuf[i]) % uMod;
        uB = (uB + uA) % uMod;
    }
    return ((unsigned long)uB << 16) | (unsigned long)uA;
}

unsigned long
adler32(unsigned long uAdler, const unsigned char *pBuf, unsigned int cb)
{
    return adler32_z(uAdler, pBuf, (size_t)cb);
}

unsigned long __crc32_z(unsigned long u, const unsigned char *p, size_t n)
    __attribute__((alias("crc32_z")));
uint32_t __crc32c(uint32_t u, const void *p, size_t n)
    __attribute__((alias("crc32c")));
unsigned long __adler32(unsigned long u, const unsigned char *p, unsigned int n)
    __attribute__((alias("adler32")));

/* ---- iconvctl (glibc-shaped control; no extra state required) ----------- */

/* Local request numbers matching common glibc/libiconv values. */
#ifndef ICONV_TRIVIALP
#define ICONV_TRIVIALP 0
#endif
#ifndef ICONV_GET_TRANSLITERATE
#define ICONV_GET_TRANSLITERATE 1
#endif
#ifndef ICONV_SET_TRANSLITERATE
#define ICONV_SET_TRANSLITERATE 2
#endif
#ifndef ICONV_GET_DISCARD_ILSEQ
#define ICONV_GET_DISCARD_ILSEQ 3
#endif
#ifndef ICONV_SET_DISCARD_ILSEQ
#define ICONV_SET_DISCARD_ILSEQ 4
#endif

int
iconvctl(iconv_t cd, int nRequest, void *pArg)
{
    if (cd == NULL || cd == (iconv_t)-1) {
        errno = EBADF;
        return -1;
    }
    switch (nRequest) {
    case ICONV_TRIVIALP:
        if (pArg == NULL) {
            errno = EINVAL;
            return -1;
        }
        /* Our converters are not pure trivial identity for all pairs. */
        *(int *)pArg = 0;
        return 0;
    case ICONV_GET_TRANSLITERATE:
        if (pArg == NULL) {
            errno = EINVAL;
            return -1;
        }
        *(int *)pArg = 0;
        return 0;
    case ICONV_SET_TRANSLITERATE:
        /* Accept and ignore (no translit engine). */
        return 0;
    case ICONV_GET_DISCARD_ILSEQ:
        if (pArg == NULL) {
            errno = EINVAL;
            return -1;
        }
        *(int *)pArg = 0;
        return 0;
    case ICONV_SET_DISCARD_ILSEQ:
        return 0;
    default:
        errno = EINVAL;
        return -1;
    }
}

int __iconvctl(iconv_t cd, int nRequest, void *pArg)
    __attribute__((alias("iconvctl")));
