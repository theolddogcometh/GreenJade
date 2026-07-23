/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38: remaining libuuid surface (uuid_time /
 * uuid_type / uuid_variant / uuid_generate_md5 / uuid_generate_sha1),
 * HMAC-SHA1/SHA256, BLAKE2b-512, and SHA3-256 (FIPS 202). Integer/pointer
 * only (no SSE). Clean-room public ABI over existing MD5/SHA1/SHA256
 * Init/Update/Final from batches 31–32.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

/* Existing digests (batch31/32) */
void MD5Init(void *pCtx);
void MD5Update(void *pCtx, const void *pData, size_t cb);
void MD5Final(unsigned char aDig[16], void *pCtx);
void SHA1Init(void *pCtx);
void SHA1Update(void *pCtx, const void *pData, size_t cb);
void SHA1Final(unsigned char aDig[20], void *pCtx);
void SHA256_Init(void *pCtx);
void SHA256_Update(void *pCtx, const void *pData, size_t cb);
void SHA256_Final(unsigned char aDig[32], void *pCtx);

const char __libcgj_batch38_marker[] = "libcgj-batch38";

/* ---- libuuid remainder -------------------------------------------------- */

int
uuid_type(const unsigned char uu[16])
{
    if (uu == NULL) {
        return -1;
    }
    return (int)((uu[6] >> 4) & 0x0fu);
}

int
uuid_variant(const unsigned char uu[16])
{
    unsigned char c;

    if (uu == NULL) {
        return -1;
    }
    c = uu[8];
    if ((c & 0x80u) == 0u) {
        return 0; /* NCS */
    }
    if ((c & 0xc0u) == 0x80u) {
        return 1; /* DCE / RFC 4122 */
    }
    if ((c & 0xe0u) == 0xc0u) {
        return 2; /* Microsoft */
    }
    return 3; /* other */
}

time_t
uuid_time(const unsigned char uu[16], struct timeval *pTv)
{
    uint64_t uTime;
    time_t t;
    /* UUID v1 timestamp: 100-ns intervals since 1582-10-15 */
    static const uint64_t uEpochDiff = 122192928000000000ull;

    if (uu == NULL) {
        errno = EINVAL;
        return (time_t)-1;
    }
    if (uuid_type(uu) != 1) {
        /* Non-time-based: still produce a deterministic clock if requested */
        if (pTv != NULL) {
            pTv->tv_sec = 0;
            pTv->tv_usec = 0;
        }
        return (time_t)0;
    }
    uTime = ((uint64_t)(uu[6] & 0x0fu) << 56) | ((uint64_t)uu[7] << 48) |
            ((uint64_t)uu[4] << 40) | ((uint64_t)uu[5] << 32) |
            ((uint64_t)uu[0] << 24) | ((uint64_t)uu[1] << 16) |
            ((uint64_t)uu[2] << 8) | (uint64_t)uu[3];
    if (uTime < uEpochDiff) {
        t = 0;
    } else {
        uTime = (uTime - uEpochDiff) / 10000000ull;
        t = (time_t)uTime;
    }
    if (pTv != NULL) {
        pTv->tv_sec = t;
        pTv->tv_usec = 0;
    }
    return t;
}

static void
b38_uuid_set_ver_var(unsigned char uu[16], unsigned uVer)
{
    uu[6] = (unsigned char)((uu[6] & 0x0fu) | ((uVer & 0x0fu) << 4));
    uu[8] = (unsigned char)((uu[8] & 0x3fu) | 0x80u);
}

void
uuid_generate_md5(unsigned char out[16], const unsigned char ns[16],
                  const char *szName)
{
    unsigned char aCtx[128];
    unsigned char aDig[16];
    size_t cb;

    if (out == NULL) {
        return;
    }
    memset(out, 0, 16u);
    if (ns == NULL) {
        return;
    }
    cb = (szName != NULL) ? strlen(szName) : 0u;
    memset(aCtx, 0, sizeof(aCtx));
    MD5Init(aCtx);
    MD5Update(aCtx, ns, 16u);
    if (cb > 0u) {
        MD5Update(aCtx, szName, cb);
    }
    MD5Final(aDig, aCtx);
    memcpy(out, aDig, 16u);
    b38_uuid_set_ver_var(out, 3u);
}

void
uuid_generate_sha1(unsigned char out[16], const unsigned char ns[16],
                   const char *szName)
{
    unsigned char aCtx[128];
    unsigned char aDig[20];
    size_t cb;

    if (out == NULL) {
        return;
    }
    memset(out, 0, 16u);
    if (ns == NULL) {
        return;
    }
    cb = (szName != NULL) ? strlen(szName) : 0u;
    memset(aCtx, 0, sizeof(aCtx));
    SHA1Init(aCtx);
    SHA1Update(aCtx, ns, 16u);
    if (cb > 0u) {
        SHA1Update(aCtx, szName, cb);
    }
    SHA1Final(aDig, aCtx);
    memcpy(out, aDig, 16u);
    b38_uuid_set_ver_var(out, 5u);
}

int __uuid_type(const unsigned char uu[16]) __attribute__((alias("uuid_type")));
int __uuid_variant(const unsigned char uu[16])
    __attribute__((alias("uuid_variant")));
time_t __uuid_time(const unsigned char uu[16], struct timeval *pTv)
    __attribute__((alias("uuid_time")));
void __uuid_generate_md5(unsigned char out[16], const unsigned char ns[16],
                         const char *szName)
    __attribute__((alias("uuid_generate_md5")));
void __uuid_generate_sha1(unsigned char out[16], const unsigned char ns[16],
                          const char *szName)
    __attribute__((alias("uuid_generate_sha1")));

/* ---- HMAC-SHA1 / HMAC-SHA256 (RFC 2104) --------------------------------- */

static void
b38_hmac(void (*pfnInit)(void *),
         void (*pfnUpdate)(void *, const void *, size_t),
         void (*pfnFinal)(unsigned char *, void *), size_t cbDig, size_t cbBlk,
         const void *pKey, size_t cbKey, const void *pData, size_t cbData,
         unsigned char *pOut)
{
    unsigned char aKey[128];
    unsigned char aIpad[128];
    unsigned char aOpad[128];
    unsigned char aInner[64];
    unsigned char aCtx[256];
    size_t i;

    if (pOut == NULL || cbBlk > sizeof(aKey) || cbDig > sizeof(aInner)) {
        return;
    }
    memset(aKey, 0, cbBlk);
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > cbBlk) {
            memset(aCtx, 0, sizeof(aCtx));
            pfnInit(aCtx);
            pfnUpdate(aCtx, pKey, cbKey);
            pfnFinal(aKey, aCtx);
        } else {
            memcpy(aKey, pKey, cbKey);
        }
    }
    for (i = 0; i < cbBlk; i++) {
        aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }
    memset(aCtx, 0, sizeof(aCtx));
    pfnInit(aCtx);
    pfnUpdate(aCtx, aIpad, cbBlk);
    if (pData != NULL && cbData > 0u) {
        pfnUpdate(aCtx, pData, cbData);
    }
    pfnFinal(aInner, aCtx);
    memset(aCtx, 0, sizeof(aCtx));
    pfnInit(aCtx);
    pfnUpdate(aCtx, aOpad, cbBlk);
    pfnUpdate(aCtx, aInner, cbDig);
    pfnFinal(pOut, aCtx);
    memset(aKey, 0, sizeof(aKey));
    memset(aIpad, 0, sizeof(aIpad));
    memset(aOpad, 0, sizeof(aOpad));
    memset(aInner, 0, sizeof(aInner));
}

/* SHA1Final / SHA256_Final take (dig, ctx) — wrap for uniform ptr */

static void
b38_sha1_final(unsigned char *pDig, void *pCtx)
{
    SHA1Final(pDig, pCtx);
}

static void
b38_sha256_final(unsigned char *pDig, void *pCtx)
{
    SHA256_Final(pDig, pCtx);
}

void
hmac_sha1(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
          unsigned char aOut[20])
{
    if (aOut == NULL) {
        return;
    }
    b38_hmac(SHA1Init, SHA1Update, b38_sha1_final, 20u, 64u, pKey, cbKey, pData,
             cbData, aOut);
}

void
hmac_sha256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
            unsigned char aOut[32])
{
    if (aOut == NULL) {
        return;
    }
    b38_hmac(SHA256_Init, SHA256_Update, b38_sha256_final, 32u, 64u, pKey, cbKey,
             pData, cbData, aOut);
}

void __hmac_sha1(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[20]) __attribute__((alias("hmac_sha1")));
void __hmac_sha256(const void *pKey, size_t cbKey, const void *pData,
                   size_t cbData, unsigned char aOut[32])
    __attribute__((alias("hmac_sha256")));

/* ---- BLAKE2b-512 (RFC 7693 shape; OpenBSD-style Init/Update/Final) ------ */

struct b38_b2b {
    uint64_t aH[8];
    uint64_t aT[2];
    unsigned char aBuf[128];
    size_t cbBuf;
    size_t cbOut;
};

static const uint64_t s_b38_b2b_iv[8] = {
    0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull, 0x3c6ef372fe94f82bull,
    0xa54ff53a5f1d36f1ull, 0x510e527fade682d1ull, 0x9b05688c2b3e6c1full,
    0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull};

static const unsigned char s_b38_b2b_sigma[12][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

static uint64_t
b38_ror64(uint64_t x, unsigned n)
{
    return (x >> n) | (x << (64u - n));
}

static uint64_t
b38_load_le64(const unsigned char *p)
{
    return (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) | ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) | ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

static void
b38_b2b_g(uint64_t *v, unsigned a, unsigned b, unsigned c, unsigned d,
          uint64_t x, uint64_t y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = b38_ror64(v[d] ^ v[a], 32);
    v[c] = v[c] + v[d];
    v[b] = b38_ror64(v[b] ^ v[c], 24);
    v[a] = v[a] + v[b] + y;
    v[d] = b38_ror64(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = b38_ror64(v[b] ^ v[c], 63);
}

static void
b38_b2b_compress(struct b38_b2b *p, int fLast)
{
    uint64_t v[16];
    uint64_t m[16];
    unsigned i;
    unsigned r;

    for (i = 0; i < 8u; i++) {
        v[i] = p->aH[i];
        v[i + 8u] = s_b38_b2b_iv[i];
    }
    v[12] ^= p->aT[0];
    v[13] ^= p->aT[1];
    if (fLast) {
        v[14] = ~v[14];
    }
    for (i = 0; i < 16u; i++) {
        m[i] = b38_load_le64(p->aBuf + i * 8u);
    }
    for (r = 0; r < 12u; r++) {
        const unsigned char *s = s_b38_b2b_sigma[r];

        b38_b2b_g(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        b38_b2b_g(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        b38_b2b_g(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        b38_b2b_g(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        b38_b2b_g(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        b38_b2b_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        b38_b2b_g(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        b38_b2b_g(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }
    for (i = 0; i < 8u; i++) {
        p->aH[i] ^= v[i] ^ v[i + 8u];
    }
}

void
BLAKE2b_Init(void *pCtx)
{
    struct b38_b2b *p = (struct b38_b2b *)pCtx;
    unsigned i;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    for (i = 0; i < 8u; i++) {
        p->aH[i] = s_b38_b2b_iv[i];
    }
    p->cbOut = 64u;
    /* param: digest_length=64, fanout=1, depth=1 */
    p->aH[0] ^= 0x01010000ull ^ (uint64_t)p->cbOut;
}

void
BLAKE2b_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b38_b2b *p = (struct b38_b2b *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0u)) {
        return;
    }
    if (p->cbOut == 0u) {
        BLAKE2b_Init(p);
    }
    while (cb > 0u) {
        size_t nLeft = 128u - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;

        memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == 128u) {
            p->aT[0] += 128u;
            if (p->aT[0] < 128u) {
                p->aT[1]++;
            }
            b38_b2b_compress(p, 0);
            p->cbBuf = 0;
        }
    }
}

void
BLAKE2b_Final(unsigned char aDig[64], void *pCtx)
{
    struct b38_b2b *p = (struct b38_b2b *)pCtx;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->cbOut == 0u) {
        BLAKE2b_Init(p);
    }
    p->aT[0] += (uint64_t)p->cbBuf;
    if (p->aT[0] < (uint64_t)p->cbBuf) {
        p->aT[1]++;
    }
    memset(p->aBuf + p->cbBuf, 0, 128u - p->cbBuf);
    b38_b2b_compress(p, 1);
    for (i = 0; i < 8u; i++) {
        aDig[i * 8u] = (unsigned char)p->aH[i];
        aDig[i * 8u + 1u] = (unsigned char)(p->aH[i] >> 8);
        aDig[i * 8u + 2u] = (unsigned char)(p->aH[i] >> 16);
        aDig[i * 8u + 3u] = (unsigned char)(p->aH[i] >> 24);
        aDig[i * 8u + 4u] = (unsigned char)(p->aH[i] >> 32);
        aDig[i * 8u + 5u] = (unsigned char)(p->aH[i] >> 40);
        aDig[i * 8u + 6u] = (unsigned char)(p->aH[i] >> 48);
        aDig[i * 8u + 7u] = (unsigned char)(p->aH[i] >> 56);
    }
    memset(p, 0, sizeof(*p));
}

static char *
b38_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
{
    static char aLocal[160];
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
BLAKE2b_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[64];

    BLAKE2b_Final(aDig, pCtx);
    return b38_hex_end(aDig, 64u, szBuf);
}

char *
BLAKE2b_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b38_b2b) + 16u];

    memset(aCtx, 0, sizeof(aCtx));
    BLAKE2b_Init(aCtx);
    BLAKE2b_Update(aCtx, pData, cb);
    return BLAKE2b_End(aCtx, szBuf);
}

/* ---- SHA3-256 (FIPS 202 / Keccak-f[1600], rate 1088) -------------------- */

struct b38_sha3 {
    uint64_t a[25];
    unsigned char aBuf[200];
    size_t cbBuf;
    size_t cbRate; /* 136 for SHA3-256 */
    size_t cbOut;  /* 32 */
};

static const unsigned s_b38_rotc[24] = {1,  3,  6,  10, 15, 21, 28, 36,
                                        45, 55, 2,  14, 27, 41, 56, 8,
                                        25, 43, 62, 18, 39, 61, 20, 44};
static const unsigned s_b38_piln[24] = {10, 7,  11, 17, 18, 3,  5,  16,
                                        8,  21, 24, 4,  15, 23, 19, 13,
                                        12, 2,  20, 14, 22, 9,  6,  1};
static const uint64_t s_b38_rc[24] = {
    0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
    0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
    0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
    0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
    0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
    0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
    0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
    0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull};

static void
b38_keccak_f(uint64_t st[25])
{
    uint64_t bc[5];
    uint64_t t;
    unsigned round;
    unsigned i;
    unsigned j;

    for (round = 0; round < 24u; round++) {
        for (i = 0; i < 5u; i++) {
            bc[i] = st[i] ^ st[i + 5u] ^ st[i + 10u] ^ st[i + 15u] ^
                    st[i + 20u];
        }
        for (i = 0; i < 5u; i++) {
            t = bc[(i + 4u) % 5u] ^ b38_ror64(bc[(i + 1u) % 5u], 63);
            for (j = 0; j < 25u; j += 5u) {
                st[j + i] ^= t;
            }
        }
        t = st[1];
        for (i = 0; i < 24u; i++) {
            j = s_b38_piln[i];
            bc[0] = st[j];
            st[j] = b38_ror64(t, 64u - s_b38_rotc[i]);
            t = bc[0];
        }
        for (j = 0; j < 25u; j += 5u) {
            for (i = 0; i < 5u; i++) {
                bc[i] = st[j + i];
            }
            for (i = 0; i < 5u; i++) {
                st[j + i] ^= (~bc[(i + 1u) % 5u]) & bc[(i + 2u) % 5u];
            }
        }
        st[0] ^= s_b38_rc[round];
    }
}

void
SHA3_256_Init(void *pCtx)
{
    struct b38_sha3 *p = (struct b38_sha3 *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->cbRate = 136u;
    p->cbOut = 32u;
}

void
SHA3_256_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b38_sha3 *p = (struct b38_sha3 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0u)) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_256_Init(p);
    }
    while (cb > 0u) {
        size_t nLeft = p->cbRate - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;
        size_t i;

        memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == p->cbRate) {
            for (i = 0; i < p->cbRate; i++) {
                p->a[i / 8u] ^= (uint64_t)p->aBuf[i] << (8u * (i % 8u));
            }
            b38_keccak_f(p->a);
            p->cbBuf = 0;
        }
    }
}

void
SHA3_256_Final(unsigned char aDig[32], void *pCtx)
{
    struct b38_sha3 *p = (struct b38_sha3 *)pCtx;
    size_t i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_256_Init(p);
    }
    /* SHA3 domain: pad 0x06 ... 0x80 */
    memset(p->aBuf + p->cbBuf, 0, p->cbRate - p->cbBuf);
    p->aBuf[p->cbBuf] ^= 0x06u;
    p->aBuf[p->cbRate - 1u] ^= 0x80u;
    for (i = 0; i < p->cbRate; i++) {
        p->a[i / 8u] ^= (uint64_t)p->aBuf[i] << (8u * (i % 8u));
    }
    b38_keccak_f(p->a);
    for (i = 0; i < 32u; i++) {
        aDig[i] = (unsigned char)(p->a[i / 8u] >> (8u * (i % 8u)));
    }
    memset(p, 0, sizeof(*p));
}

char *
SHA3_256_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[32];

    SHA3_256_Final(aDig, pCtx);
    return b38_hex_end(aDig, 32u, szBuf);
}

char *
SHA3_256_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b38_sha3) + 16u];

    memset(aCtx, 0, sizeof(aCtx));
    SHA3_256_Init(aCtx);
    SHA3_256_Update(aCtx, pData, cb);
    return SHA3_256_End(aCtx, szBuf);
}

/* Underscored aliases */
void __BLAKE2b_Init(void *p) __attribute__((alias("BLAKE2b_Init")));
void __BLAKE2b_Update(void *p, const void *d, size_t n)
    __attribute__((alias("BLAKE2b_Update")));
void __BLAKE2b_Final(unsigned char a[64], void *p)
    __attribute__((alias("BLAKE2b_Final")));
void __SHA3_256_Init(void *p) __attribute__((alias("SHA3_256_Init")));
void __SHA3_256_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA3_256_Update")));
void __SHA3_256_Final(unsigned char a[32], void *p)
    __attribute__((alias("SHA3_256_Final")));

/* ---- soft deepen: HMAC-SHA256 oneshot name (unique) --------------------- */

void hmac_sha256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[32]);

int
hmac_sha256_oneshot(const void *pKey, size_t cbKey, const void *pData,
                    size_t cbData, unsigned char aOut[32])
{
    if (aOut == NULL) {
        return -1;
    }
    hmac_sha256(pKey, cbKey, pData, cbData, aOut);
    return 0;
}

int __hmac_sha256_oneshot(const void *pKey, size_t cbKey, const void *pData,
                          size_t cbData, unsigned char aOut[32])
    __attribute__((alias("hmac_sha256_oneshot")));
