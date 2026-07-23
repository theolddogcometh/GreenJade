/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36: Linux libuuid-compatible API over the batch35
 * BSD UUID surface (generate/parse/unparse/clear/copy), plus MD4/RMD160/
 * BLAKE2s End+Data helpers and SipHash-2-4. Integer/pointer only (no SSE).
 * Clean-room public ABI only.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* batch35 BSD-style surface */
void uuid_create(void *pUuid, uint32_t *pStatus);
void uuid_create_nil(void *pUuid, uint32_t *pStatus);
int uuid_is_nil(const void *pUuid, uint32_t *pStatus);
int32_t uuid_compare(const void *pA, const void *pB, uint32_t *pStatus);
void uuid_to_string(const void *pUuid, char **ppStr, uint32_t *pStatus);
void uuid_from_string(const char *sz, void *pUuid, uint32_t *pStatus);

/* batch34 digests */
void MD4Init(void *pCtx);
void MD4Update(void *pCtx, const void *pData, size_t cb);
void MD4Final(unsigned char aDig[16], void *pCtx);
void RMD160Init(void *pCtx);
void RMD160Update(void *pCtx, const void *pData, size_t cb);
void RMD160Final(unsigned char aDig[20], void *pCtx);
void BLAKE2s_Init(void *pCtx);
void BLAKE2s_Update(void *pCtx, const void *pData, size_t cb);
void BLAKE2s_Final(unsigned char aDig[32], void *pCtx);

const char __libcgj_batch36_marker[] = "libcgj-batch36";

/* ---- libuuid-shaped API (util-linux / e2fsprogs public ABI) -------------- */

void
uuid_clear(unsigned char uu[16])
{
    uuid_create_nil(uu, NULL);
}

void
uuid_copy(unsigned char dst[16], const unsigned char src[16])
{
    if (dst == NULL || src == NULL) {
        return;
    }
    memcpy(dst, src, 16u);
}

int
uuid_is_null(const unsigned char uu[16])
{
    return uuid_is_nil(uu, NULL);
}

void
uuid_generate(unsigned char out[16])
{
    uuid_create(out, NULL);
}

void
uuid_generate_random(unsigned char out[16])
{
    uuid_create(out, NULL);
}

void
uuid_generate_time(unsigned char out[16])
{
    uuid_create(out, NULL);
}

int
uuid_generate_time_safe(unsigned char out[16])
{
    uuid_create(out, NULL);
    return 0;
}

int
uuid_parse(const char *szIn, unsigned char uu[16])
{
    uint32_t st = 0;

    if (szIn == NULL || uu == NULL) {
        return -1;
    }
    uuid_from_string(szIn, uu, &st);
    return (st == 0) ? 0 : -1;
}

int
uuid_parse_range(const char *szIn, const char *szEnd, unsigned char uu[16])
{
    size_t cb;
    char aTmp[48];

    if (szIn == NULL || uu == NULL) {
        return -1;
    }
    if (szEnd == NULL) {
        return uuid_parse(szIn, uu);
    }
    if (szEnd < szIn) {
        return -1;
    }
    cb = (size_t)(szEnd - szIn);
    if (cb >= sizeof(aTmp)) {
        return -1;
    }
    memcpy(aTmp, szIn, cb);
    aTmp[cb] = '\0';
    return uuid_parse(aTmp, uu);
}

static void
b36_unparse(const unsigned char uu[16], char *szOut, int nUpper)
{
    static const char *szLo = "0123456789abcdef";
    static const char *szUp = "0123456789ABCDEF";
    const char *szDigits = nUpper ? szUp : szLo;
    unsigned i;
    unsigned j = 0;
    static const unsigned aDashAfter[] = {4, 6, 8, 10};

    if (szOut == NULL) {
        return;
    }
    if (uu == NULL) {
        szOut[0] = '\0';
        return;
    }
    for (i = 0; i < 16u; i++) {
        unsigned d;
        for (d = 0; d < 4u; d++) {
            if (aDashAfter[d] == i) {
                szOut[j++] = '-';
            }
        }
        szOut[j++] = szDigits[(uu[i] >> 4) & 0xfu];
        szOut[j++] = szDigits[uu[i] & 0xfu];
    }
    szOut[j] = '\0';
}

void
uuid_unparse(const unsigned char uu[16], char *szOut)
{
    b36_unparse(uu, szOut, 0);
}

void
uuid_unparse_lower(const unsigned char uu[16], char *szOut)
{
    b36_unparse(uu, szOut, 0);
}

void
uuid_unparse_upper(const unsigned char uu[16], char *szOut)
{
    b36_unparse(uu, szOut, 1);
}

/* underscored aliases */
void __uuid_generate(unsigned char out[16])
    __attribute__((alias("uuid_generate")));
void __uuid_generate_random(unsigned char out[16])
    __attribute__((alias("uuid_generate_random")));
void __uuid_clear(unsigned char uu[16]) __attribute__((alias("uuid_clear")));
void __uuid_copy(unsigned char dst[16], const unsigned char src[16])
    __attribute__((alias("uuid_copy")));
int __uuid_is_null(const unsigned char uu[16])
    __attribute__((alias("uuid_is_null")));
int __uuid_parse(const char *szIn, unsigned char uu[16])
    __attribute__((alias("uuid_parse")));
void __uuid_unparse(const unsigned char uu[16], char *szOut)
    __attribute__((alias("uuid_unparse")));
void __uuid_unparse_lower(const unsigned char uu[16], char *szOut)
    __attribute__((alias("uuid_unparse_lower")));
void __uuid_unparse_upper(const unsigned char uu[16], char *szOut)
    __attribute__((alias("uuid_unparse_upper")));

/* ---- hex End helpers for batch34 digests -------------------------------- */

static char *
b36_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
{
    static char aLocal[80];
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
MD4End(void *pCtx, char *szBuf)
{
    unsigned char aDig[16];

    MD4Final(aDig, pCtx);
    return b36_hex_end(aDig, 16u, szBuf);
}

char *
MD4Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[128];

    memset(aCtx, 0, sizeof(aCtx));
    MD4Init(aCtx);
    MD4Update(aCtx, pData, cb);
    return MD4End(aCtx, szBuf);
}

char *
RMD160End(void *pCtx, char *szBuf)
{
    unsigned char aDig[20];

    RMD160Final(aDig, pCtx);
    return b36_hex_end(aDig, 20u, szBuf);
}

char *
RMD160Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[256];

    memset(aCtx, 0, sizeof(aCtx));
    RMD160Init(aCtx);
    RMD160Update(aCtx, pData, cb);
    return RMD160End(aCtx, szBuf);
}

char *
BLAKE2s_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[32];

    BLAKE2s_Final(aDig, pCtx);
    return b36_hex_end(aDig, 32u, szBuf);
}

char *
BLAKE2s_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[256];

    memset(aCtx, 0, sizeof(aCtx));
    BLAKE2s_Init(aCtx);
    BLAKE2s_Update(aCtx, pData, cb);
    return BLAKE2s_End(aCtx, szBuf);
}

/* ---- SipHash-2-4 (Aumasson/Bernstein; 64-bit result as LE bytes) -------- */

static uint64_t
b36_rol64(uint64_t x, unsigned n)
{
    return (x << n) | (x >> (64u - n));
}

static uint64_t
b36_load_le64(const unsigned char *p, size_t cb)
{
    uint64_t u = 0;
    size_t i;

    for (i = 0; i < cb && i < 8u; i++) {
        u |= (uint64_t)p[i] << (8u * i);
    }
    return u;
}

static uint64_t
b36_siphash24(const unsigned char *pIn, size_t cb, const unsigned char aKey[16])
{
    uint64_t k0 = b36_load_le64(aKey, 8u);
    uint64_t k1 = b36_load_le64(aKey + 8, 8u);
    uint64_t v0 = 0x736f6d6570736575ull ^ k0;
    uint64_t v1 = 0x646f72616e646f6dull ^ k1;
    uint64_t v2 = 0x6c7967656e657261ull ^ k0;
    uint64_t v3 = 0x7465646279746573ull ^ k1;
    uint64_t b;
    size_t nLeft = cb;
    const unsigned char *p = pIn;

#define SIPROUND                                                              \
    do {                                                                      \
        v0 += v1;                                                             \
        v1 = b36_rol64(v1, 13);                                               \
        v1 ^= v0;                                                             \
        v0 = b36_rol64(v0, 32);                                               \
        v2 += v3;                                                             \
        v3 = b36_rol64(v3, 16);                                               \
        v3 ^= v2;                                                             \
        v0 += v3;                                                             \
        v3 = b36_rol64(v3, 21);                                               \
        v3 ^= v0;                                                             \
        v2 += v1;                                                             \
        v1 = b36_rol64(v1, 17);                                               \
        v1 ^= v2;                                                             \
        v2 = b36_rol64(v2, 32);                                               \
    } while (0)

    while (nLeft >= 8u) {
        uint64_t m = b36_load_le64(p, 8u);

        v3 ^= m;
        SIPROUND;
        SIPROUND;
        v0 ^= m;
        p += 8u;
        nLeft -= 8u;
    }
    b = ((uint64_t)cb) << 56;
    b |= b36_load_le64(p, nLeft);
    v3 ^= b;
    SIPROUND;
    SIPROUND;
    v0 ^= b;
    v2 ^= 0xffull;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
#undef SIPROUND
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t
siphash24(const void *pIn, size_t cb, const unsigned char aKey[16])
{
    static const unsigned char aZero[16];

    if (aKey == NULL) {
        aKey = aZero;
    }
    if (pIn == NULL && cb != 0u) {
        return 0;
    }
    return b36_siphash24((const unsigned char *)pIn, cb, aKey);
}

void
siphash24_buf(const void *pIn, size_t cb, const unsigned char aKey[16],
              unsigned char aOut[8])
{
    uint64_t u = siphash24(pIn, cb, aKey);
    unsigned i;

    if (aOut == NULL) {
        return;
    }
    for (i = 0; i < 8u; i++) {
        aOut[i] = (unsigned char)(u >> (8u * i));
    }
}

uint64_t
__siphash24(const void *pIn, size_t cb, const unsigned char aKey[16])
    __attribute__((alias("siphash24")));

/* ---- BSD radixsort / sradixsort (stable byte-string sort) --------------- */

static void
b36_count_sort(const unsigned char **pp, const unsigned char **ppTmp, int n,
               unsigned uPos, const unsigned char *pTab, unsigned uEnd)
{
    unsigned aCount[256];
    unsigned aOff[256];
    int i;
    unsigned b;

    memset(aCount, 0, sizeof(aCount));
    for (i = 0; i < n; i++) {
        const unsigned char *s = pp[i];
        unsigned char ch;

        if (s == NULL) {
            ch = 0;
        } else {
            ch = s[uPos];
            if (pTab != NULL) {
                ch = pTab[ch];
            }
        }
        aCount[ch]++;
    }
    aOff[0] = 0;
    for (b = 1; b < 256u; b++) {
        aOff[b] = aOff[b - 1u] + aCount[b - 1u];
    }
    for (i = 0; i < n; i++) {
        const unsigned char *s = pp[i];
        unsigned char ch;

        if (s == NULL) {
            ch = 0;
        } else {
            ch = s[uPos];
            if (pTab != NULL) {
                ch = pTab[ch];
            }
        }
        ppTmp[aOff[ch]++] = s;
    }
    memcpy(pp, ppTmp, (size_t)n * sizeof(const unsigned char *));
    (void)uEnd;
}

int
radixsort(const unsigned char **base, int nmemb, const unsigned char *table,
          unsigned endbyte)
{
    const unsigned char **ppTmp;
    unsigned pos;
    int i;
    size_t nMax = 0;

    if (base == NULL || nmemb < 0) {
        errno = EINVAL;
        return -1;
    }
    if (nmemb <= 1) {
        return 0;
    }
    for (i = 0; i < nmemb; i++) {
        size_t n = 0;
        const unsigned char *s = base[i];

        if (s != NULL) {
            while (s[n] != 0u && (endbyte == 0u || n < endbyte)) {
                n++;
            }
        }
        if (n > nMax) {
            nMax = n;
        }
    }
    if (endbyte != 0u && (size_t)endbyte < nMax) {
        nMax = endbyte;
    }
    ppTmp = (const unsigned char **)malloc((size_t)nmemb *
                                           sizeof(const unsigned char *));
    if (ppTmp == NULL) {
        errno = ENOMEM;
        return -1;
    }
    /* LSD from last char toward first (stable) */
    if (nMax == 0u) {
        free(ppTmp);
        return 0;
    }
    for (pos = (unsigned)nMax; pos > 0u; pos--) {
        b36_count_sort(base, ppTmp, nmemb, pos - 1u, table, endbyte);
    }
    free(ppTmp);
    return 0;
}

int
sradixsort(const unsigned char **base, int nmemb, const unsigned char *table,
           unsigned endbyte)
{
    /* Stable variant: same LSD path is already stable. */
    return radixsort(base, nmemb, table, endbyte);
}

int
__radixsort(const unsigned char **base, int nmemb, const unsigned char *table,
            unsigned endbyte) __attribute__((alias("radixsort")));
int
__sradixsort(const unsigned char **base, int nmemb, const unsigned char *table,
             unsigned endbyte) __attribute__((alias("sradixsort")));

/* ---- OpenBSD dehumanize_number ------------------------------------------ */

#ifndef B36_I64_MAX
#define B36_I64_MAX ((int64_t)0x7fffffffffffffffLL)
#endif

int
dehumanize_number(const char *sz, int64_t *pOut)
{
    const char *p;
    int64_t n = 0;
    int fNeg = 0;
    int64_t nMul = 1;
    char ch;

    if (sz == NULL || pOut == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = sz;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '-') {
        fNeg = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    if (*p < '0' || *p > '9') {
        errno = EINVAL;
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        int64_t nDig = (int64_t)(*p - '0');

        if (n > (B36_I64_MAX - nDig) / 10) {
            errno = ERANGE;
            return -1;
        }
        n = n * 10 + nDig;
        p++;
    }
    ch = *p;
    if (ch == 'k' || ch == 'K') {
        nMul = 1024;
        p++;
    } else if (ch == 'm' || ch == 'M') {
        nMul = 1024ll * 1024ll;
        p++;
    } else if (ch == 'g' || ch == 'G') {
        nMul = 1024ll * 1024ll * 1024ll;
        p++;
    } else if (ch == 't' || ch == 'T') {
        nMul = 1024ll * 1024ll * 1024ll * 1024ll;
        p++;
    } else if (ch == 'b' || ch == 'B') {
        nMul = 1;
        p++;
    }
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p != '\0') {
        errno = EINVAL;
        return -1;
    }
    if (n > B36_I64_MAX / nMul) {
        errno = ERANGE;
        return -1;
    }
    n *= nMul;
    *pOut = fNeg ? -n : n;
    return 0;
}

int
__dehumanize_number(const char *sz, int64_t *pOut)
    __attribute__((alias("dehumanize_number")));

/* ---- inet ntoa reentrant helper (unique name) --------------------------- */

char *
__inet_addr_ntoa_r(uint32_t uAddr, char *szBuf)
{
    unsigned a, b, c, d;
    char *p;
    unsigned u;
    char aTmp[4];
    int n;
    int i;

    if (szBuf == NULL) {
        return NULL;
    }
    a = (unsigned)(uAddr & 0xffu);
    b = (unsigned)((uAddr >> 8) & 0xffu);
    c = (unsigned)((uAddr >> 16) & 0xffu);
    d = (unsigned)((uAddr >> 24) & 0xffu);
    p = szBuf;
    for (i = 0; i < 4; i++) {
        u = (i == 0) ? a : (i == 1) ? b : (i == 2) ? c : d;
        n = 0;
        if (u == 0u) {
            aTmp[n++] = '0';
        } else {
            while (u > 0u && n < 3) {
                aTmp[n++] = (char)('0' + (u % 10u));
                u /= 10u;
            }
        }
        while (n > 0) {
            *p++ = aTmp[--n];
        }
        if (i < 3) {
            *p++ = '.';
        }
    }
    *p = '\0';
    return szBuf;
}

/* ---- soft deepen: siphash24_keyed alias + radix sort probe (unique) ----- */

uint64_t siphash24(const void *pIn, size_t cb, const unsigned char aKey[16]);

uint64_t
siphash24_keyed(const void *pIn, size_t cb, const unsigned char aKey[16])
{
    return siphash24(pIn, cb, aKey);
}

uint64_t __siphash24_keyed(const void *pIn, size_t cb,
                           const unsigned char aKey[16])
    __attribute__((alias("siphash24_keyed")));
