/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40: complete SHA-3 family (224/384/512),
 * SHA-224, SHA-512/224, SHA-512/256, HMAC-SHA384, xxHash XXH32/XXH64,
 * HKDF-SHA256/512, PBKDF2-HMAC-SHA1/256, and base64 encode/decode.
 * Integer/pointer only (no SSE). Clean-room public ABI over batch31/38/39.
 */
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

void SHA256_Init(void *pCtx);
void SHA256_Update(void *pCtx, const void *pData, size_t cb);
void SHA256_Final(unsigned char aDig[32], void *pCtx);
void SHA512_Update(void *pCtx, const void *pData, size_t cb);
void SHA512_Final(unsigned char aDig[64], void *pCtx);
void SHA384_Init(void *pCtx);
void SHA384_Update(void *pCtx, const void *pData, size_t cb);
void SHA384_Final(unsigned char aDig[48], void *pCtx);

const char __libcgj_batch40_marker[] = "libcgj-batch40";

/* ---- shared hex --------------------------------------------------------- */

static char *
b40_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
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

/* ---- SHA-3 family (FIPS 202 / Keccak-f[1600]) --------------------------- */

struct b40_sha3 {
    uint64_t a[25];
    unsigned char aBuf[200];
    size_t cbBuf;
    size_t cbRate;
    size_t cbOut;
};

static const unsigned s_b40_rotc[24] = {1,  3,  6,  10, 15, 21, 28, 36,
                                        45, 55, 2,  14, 27, 41, 56, 8,
                                        25, 43, 62, 18, 39, 61, 20, 44};
static const unsigned s_b40_piln[24] = {10, 7,  11, 17, 18, 3,  5,  16,
                                        8,  21, 24, 4,  15, 23, 19, 13,
                                        12, 2,  20, 14, 22, 9,  6,  1};
static const uint64_t s_b40_rc[24] = {
    0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
    0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
    0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
    0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
    0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
    0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
    0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
    0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull};

static uint64_t
b40_ror64(uint64_t x, unsigned n)
{
    return (x >> n) | (x << (64u - n));
}

static void
b40_keccak_f(uint64_t st[25])
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
            t = bc[(i + 4u) % 5u] ^ b40_ror64(bc[(i + 1u) % 5u], 63);
            for (j = 0; j < 25u; j += 5u) {
                st[j + i] ^= t;
            }
        }
        t = st[1];
        for (i = 0; i < 24u; i++) {
            j = s_b40_piln[i];
            bc[0] = st[j];
            st[j] = b40_ror64(t, 64u - s_b40_rotc[i]);
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
        st[0] ^= s_b40_rc[round];
    }
}

static void
b40_sha3_init(struct b40_sha3 *p, size_t cbRate, size_t cbOut)
{
    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->cbRate = cbRate;
    p->cbOut = cbOut;
}

static void
b40_sha3_update(struct b40_sha3 *p, const void *pData, size_t cb)
{
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0u)) {
        return;
    }
    if (p->cbRate == 0u) {
        return;
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
            b40_keccak_f(p->a);
            p->cbBuf = 0;
        }
    }
}

static void
b40_sha3_final(struct b40_sha3 *p, unsigned char *pDig)
{
    size_t i;

    if (p == NULL || pDig == NULL || p->cbRate == 0u) {
        return;
    }
    /* SHA-3 domain: pad 0x06 ... 0x80 */
    memset(p->aBuf + p->cbBuf, 0, p->cbRate - p->cbBuf);
    p->aBuf[p->cbBuf] ^= 0x06u;
    p->aBuf[p->cbRate - 1u] ^= 0x80u;
    for (i = 0; i < p->cbRate; i++) {
        p->a[i / 8u] ^= (uint64_t)p->aBuf[i] << (8u * (i % 8u));
    }
    b40_keccak_f(p->a);
    for (i = 0; i < p->cbOut; i++) {
        pDig[i] = (unsigned char)(p->a[i / 8u] >> (8u * (i % 8u)));
    }
    memset(p, 0, sizeof(*p));
}

/* SHA3-224: rate 144, out 28 */
void
SHA3_224_Init(void *pCtx)
{
    b40_sha3_init((struct b40_sha3 *)pCtx, 144u, 28u);
}

void
SHA3_224_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_224_Init(p);
    }
    b40_sha3_update(p, pData, cb);
}

void
SHA3_224_Final(unsigned char aDig[28], void *pCtx)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_224_Init(p);
    }
    b40_sha3_final(p, aDig);
}

char *
SHA3_224_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[28];

    SHA3_224_Final(aDig, pCtx);
    return b40_hex_end(aDig, 28u, szBuf);
}

char *
SHA3_224_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha3) + 8u];
    unsigned char aDig[28];

    memset(aCtx, 0, sizeof(aCtx));
    SHA3_224_Init(aCtx);
    SHA3_224_Update(aCtx, pData, cb);
    SHA3_224_Final(aDig, aCtx);
    return b40_hex_end(aDig, 28u, szBuf);
}

/* SHA3-384: rate 104, out 48 */
void
SHA3_384_Init(void *pCtx)
{
    b40_sha3_init((struct b40_sha3 *)pCtx, 104u, 48u);
}

void
SHA3_384_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_384_Init(p);
    }
    b40_sha3_update(p, pData, cb);
}

void
SHA3_384_Final(unsigned char aDig[48], void *pCtx)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_384_Init(p);
    }
    b40_sha3_final(p, aDig);
}

char *
SHA3_384_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[48];

    SHA3_384_Final(aDig, pCtx);
    return b40_hex_end(aDig, 48u, szBuf);
}

char *
SHA3_384_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha3) + 8u];
    unsigned char aDig[48];

    memset(aCtx, 0, sizeof(aCtx));
    SHA3_384_Init(aCtx);
    SHA3_384_Update(aCtx, pData, cb);
    SHA3_384_Final(aDig, aCtx);
    return b40_hex_end(aDig, 48u, szBuf);
}

/* SHA3-512: rate 72, out 64 */
void
SHA3_512_Init(void *pCtx)
{
    b40_sha3_init((struct b40_sha3 *)pCtx, 72u, 64u);
}

void
SHA3_512_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_512_Init(p);
    }
    b40_sha3_update(p, pData, cb);
}

void
SHA3_512_Final(unsigned char aDig[64], void *pCtx)
{
    struct b40_sha3 *p = (struct b40_sha3 *)pCtx;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->cbRate == 0u) {
        SHA3_512_Init(p);
    }
    b40_sha3_final(p, aDig);
}

char *
SHA3_512_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[64];

    SHA3_512_Final(aDig, pCtx);
    return b40_hex_end(aDig, 64u, szBuf);
}

char *
SHA3_512_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha3) + 8u];
    unsigned char aDig[64];

    memset(aCtx, 0, sizeof(aCtx));
    SHA3_512_Init(aCtx);
    SHA3_512_Update(aCtx, pData, cb);
    SHA3_512_Final(aDig, aCtx);
    return b40_hex_end(aDig, 64u, szBuf);
}

void __SHA3_224_Init(void *p) __attribute__((alias("SHA3_224_Init")));
void __SHA3_224_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA3_224_Update")));
void __SHA3_224_Final(unsigned char a[28], void *p)
    __attribute__((alias("SHA3_224_Final")));
void __SHA3_384_Init(void *p) __attribute__((alias("SHA3_384_Init")));
void __SHA3_384_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA3_384_Update")));
void __SHA3_384_Final(unsigned char a[48], void *p)
    __attribute__((alias("SHA3_384_Final")));
void __SHA3_512_Init(void *p) __attribute__((alias("SHA3_512_Init")));
void __SHA3_512_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA3_512_Update")));
void __SHA3_512_Final(unsigned char a[64], void *p)
    __attribute__((alias("SHA3_512_Final")));

/* ---- SHA-224 (SHA-256 compression, FIPS 180-4 IV, 28-byte out) ---------- */

/*
 * Context layout must match batch31 struct b31_sha256 so SHA256_Update/Final
 * may operate after SHA224_Init installs the SHA-224 IV.
 */
struct b40_sha224 {
    uint32_t aState[8];
    uint32_t aCount[2];
    unsigned char aBuf[64];
    unsigned char _pad[8];
};

void
SHA224_Init(void *pCtx)
{
    struct b40_sha224 *p = (struct b40_sha224 *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0xc1059ed8u;
    p->aState[1] = 0x367cd507u;
    p->aState[2] = 0x3070dd17u;
    p->aState[3] = 0xf70e5939u;
    p->aState[4] = 0xffc00b31u;
    p->aState[5] = 0x68581511u;
    p->aState[6] = 0x64f98fa7u;
    p->aState[7] = 0xbefa4fa4u;
}

void
SHA224_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha224 *p = (struct b40_sha224 *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aCount[0] == 0u &&
        p->aCount[1] == 0u) {
        SHA224_Init(p);
    }
    SHA256_Update(pCtx, pData, cb);
}

void
SHA224_Final(unsigned char aDig[28], void *pCtx)
{
    unsigned char aFull[32];
    struct b40_sha224 *p = (struct b40_sha224 *)pCtx;

    if (aDig == NULL || p == NULL) {
        return;
    }
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aCount[0] == 0u &&
        p->aCount[1] == 0u) {
        SHA224_Init(p);
    }
    SHA256_Final(aFull, pCtx);
    memcpy(aDig, aFull, 28u);
}

char *
SHA224_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha224)];
    unsigned char aDig[28];

    memset(aCtx, 0, sizeof(aCtx));
    SHA224_Init(aCtx);
    SHA224_Update(aCtx, pData, cb);
    SHA224_Final(aDig, aCtx);
    return b40_hex_end(aDig, 28u, szBuf);
}

void __SHA224_Init(void *p) __attribute__((alias("SHA224_Init")));
void __SHA224_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA224_Update")));
void __SHA224_Final(unsigned char a[28], void *p)
    __attribute__((alias("SHA224_Final")));
char *__SHA224_Data(const void *d, size_t n, char *s)
    __attribute__((alias("SHA224_Data")));

/* ---- SHA-512/224 and SHA-512/256 ---------------------------------------- */

struct b40_sha512t {
    uint64_t aState[8];
    uint64_t aCount[2];
    unsigned char aBuf[128];
    unsigned char _pad[16];
};

void
SHA512_224_Init(void *pCtx)
{
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0x8c3d37c819544da2ull;
    p->aState[1] = 0x73e1996689dcd4d6ull;
    p->aState[2] = 0x1dfab7ae32ff9c82ull;
    p->aState[3] = 0x679dd514582f9fcfull;
    p->aState[4] = 0x0f6d2b697bd44da8ull;
    p->aState[5] = 0x77e36f7304c48942ull;
    p->aState[6] = 0x3f9d85a86a1d36c8ull;
    p->aState[7] = 0x1112e6ad91d692a1ull;
}

void
SHA512_224_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA512_224_Init(p);
    }
    SHA512_Update(pCtx, pData, cb);
}

void
SHA512_224_Final(unsigned char aDig[28], void *pCtx)
{
    unsigned char aFull[64];
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (aDig == NULL || p == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA512_224_Init(p);
    }
    SHA512_Final(aFull, pCtx);
    memcpy(aDig, aFull, 28u);
}

char *
SHA512_224_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha512t)];
    unsigned char aDig[28];

    memset(aCtx, 0, sizeof(aCtx));
    SHA512_224_Init(aCtx);
    SHA512_224_Update(aCtx, pData, cb);
    SHA512_224_Final(aDig, aCtx);
    return b40_hex_end(aDig, 28u, szBuf);
}

void
SHA512_256_Init(void *pCtx)
{
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0x22312194fc2bf72cull;
    p->aState[1] = 0x9f555fa3c84c64c2ull;
    p->aState[2] = 0x2393b86b6f53b151ull;
    p->aState[3] = 0x963877195940eabdull;
    p->aState[4] = 0x96283ee2a88effe3ull;
    p->aState[5] = 0xbe5e1e2553863992ull;
    p->aState[6] = 0x2b0199fc2c85b8aaull;
    p->aState[7] = 0x0eb72ddc81c52ca2ull;
}

void
SHA512_256_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (p == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA512_256_Init(p);
    }
    SHA512_Update(pCtx, pData, cb);
}

void
SHA512_256_Final(unsigned char aDig[32], void *pCtx)
{
    unsigned char aFull[64];
    struct b40_sha512t *p = (struct b40_sha512t *)pCtx;

    if (aDig == NULL || p == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        SHA512_256_Init(p);
    }
    SHA512_Final(aFull, pCtx);
    memcpy(aDig, aFull, 32u);
}

char *
SHA512_256_Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b40_sha512t)];
    unsigned char aDig[32];

    memset(aCtx, 0, sizeof(aCtx));
    SHA512_256_Init(aCtx);
    SHA512_256_Update(aCtx, pData, cb);
    SHA512_256_Final(aDig, aCtx);
    return b40_hex_end(aDig, 32u, szBuf);
}

void __SHA512_224_Init(void *p) __attribute__((alias("SHA512_224_Init")));
void __SHA512_224_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA512_224_Update")));
void __SHA512_224_Final(unsigned char a[28], void *p)
    __attribute__((alias("SHA512_224_Final")));
void __SHA512_256_Init(void *p) __attribute__((alias("SHA512_256_Init")));
void __SHA512_256_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHA512_256_Update")));
void __SHA512_256_Final(unsigned char a[32], void *p)
    __attribute__((alias("SHA512_256_Final")));

/* ---- HMAC-SHA384 (RFC 2104 over SHA-384) -------------------------------- */

void
hmac_sha384(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
            unsigned char aOut[48])
{
    unsigned char aKey[128];
    unsigned char aIpad[128];
    unsigned char aOpad[128];
    unsigned char aInner[48];
    unsigned char aCtx[256];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aKey, 0, sizeof(aKey));
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > 128u) {
            memset(aCtx, 0, sizeof(aCtx));
            SHA384_Init(aCtx);
            SHA384_Update(aCtx, pKey, cbKey);
            SHA384_Final(aKey, aCtx);
        } else {
            memcpy(aKey, pKey, cbKey);
        }
    }
    for (i = 0; i < 128u; i++) {
        aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }
    memset(aCtx, 0, sizeof(aCtx));
    SHA384_Init(aCtx);
    SHA384_Update(aCtx, aIpad, 128u);
    if (pData != NULL && cbData > 0u) {
        SHA384_Update(aCtx, pData, cbData);
    }
    SHA384_Final(aInner, aCtx);
    memset(aCtx, 0, sizeof(aCtx));
    SHA384_Init(aCtx);
    SHA384_Update(aCtx, aOpad, 128u);
    SHA384_Update(aCtx, aInner, 48u);
    SHA384_Final(aOut, aCtx);
    memset(aKey, 0, sizeof(aKey));
    memset(aIpad, 0, sizeof(aIpad));
    memset(aOpad, 0, sizeof(aOpad));
    memset(aInner, 0, sizeof(aInner));
}

void __hmac_sha384(const void *pKey, size_t cbKey, const void *pData,
                   size_t cbData, unsigned char aOut[48])
    __attribute__((alias("hmac_sha384")));

/* ---- xxHash XXH32 / XXH64 (clean-room, integer only) -------------------- */

static uint32_t
b40_rotl32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint64_t
b40_rotl64(uint64_t x, unsigned n)
{
    return (x << n) | (x >> (64u - n));
}

static uint32_t
b40_read32le(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t
b40_read64le(const unsigned char *p)
{
    return (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) | ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) | ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

#define B40_PRIME32_1 0x9E3779B1u
#define B40_PRIME32_2 0x85EBCA77u
#define B40_PRIME32_3 0xC2B2AE3Du
#define B40_PRIME32_4 0x27D4EB2Fu
#define B40_PRIME32_5 0x165667B1u

#define B40_PRIME64_1 0x9E3779B185EBCA87ull
#define B40_PRIME64_2 0xC2B2AE3D27D4EB4Full
#define B40_PRIME64_3 0x165667B19E3779F9ull
#define B40_PRIME64_4 0x85EBCA77C2B2AE63ull
#define B40_PRIME64_5 0x27D4EB2F165667C5ull

uint32_t
XXH32(const void *pInput, size_t cb, uint32_t uSeed)
{
    const unsigned char *p = (const unsigned char *)pInput;
    uint32_t h;

    if (p == NULL && cb != 0u) {
        return 0u;
    }
    if (cb >= 16u) {
        const unsigned char *pLimit = p + cb - 16u;
        uint32_t v1 = uSeed + B40_PRIME32_1 + B40_PRIME32_2;
        uint32_t v2 = uSeed + B40_PRIME32_2;
        uint32_t v3 = uSeed + 0u;
        uint32_t v4 = uSeed - B40_PRIME32_1;

        do {
            v1 = b40_rotl32(v1 + b40_read32le(p) * B40_PRIME32_2, 13) *
                 B40_PRIME32_1;
            p += 4;
            v2 = b40_rotl32(v2 + b40_read32le(p) * B40_PRIME32_2, 13) *
                 B40_PRIME32_1;
            p += 4;
            v3 = b40_rotl32(v3 + b40_read32le(p) * B40_PRIME32_2, 13) *
                 B40_PRIME32_1;
            p += 4;
            v4 = b40_rotl32(v4 + b40_read32le(p) * B40_PRIME32_2, 13) *
                 B40_PRIME32_1;
            p += 4;
        } while (p <= pLimit);
        h = b40_rotl32(v1, 1) + b40_rotl32(v2, 7) + b40_rotl32(v3, 12) +
            b40_rotl32(v4, 18);
    } else {
        h = uSeed + B40_PRIME32_5;
    }
    h += (uint32_t)cb;
    {
        const unsigned char *pEnd = (const unsigned char *)pInput + cb;

        while (p + 4u <= pEnd) {
            h += b40_read32le(p) * B40_PRIME32_3;
            h = b40_rotl32(h, 17) * B40_PRIME32_4;
            p += 4;
        }
        while (p < pEnd) {
            h += (uint32_t)(*p) * B40_PRIME32_5;
            h = b40_rotl32(h, 11) * B40_PRIME32_1;
            p++;
        }
    }
    h ^= h >> 15;
    h *= B40_PRIME32_2;
    h ^= h >> 13;
    h *= B40_PRIME32_3;
    h ^= h >> 16;
    return h;
}

static uint64_t
b40_xxh64_round(uint64_t acc, uint64_t input)
{
    acc += input * B40_PRIME64_2;
    acc = b40_rotl64(acc, 31);
    acc *= B40_PRIME64_1;
    return acc;
}

static uint64_t
b40_xxh64_merge(uint64_t acc, uint64_t val)
{
    val = b40_xxh64_round(0, val);
    acc ^= val;
    acc = acc * B40_PRIME64_1 + B40_PRIME64_4;
    return acc;
}

uint64_t
XXH64(const void *pInput, size_t cb, uint64_t uSeed)
{
    const unsigned char *p = (const unsigned char *)pInput;
    uint64_t h;

    if (p == NULL && cb != 0u) {
        return 0ull;
    }
    if (cb >= 32u) {
        const unsigned char *pLimit = p + cb - 32u;
        uint64_t v1 = uSeed + B40_PRIME64_1 + B40_PRIME64_2;
        uint64_t v2 = uSeed + B40_PRIME64_2;
        uint64_t v3 = uSeed + 0ull;
        uint64_t v4 = uSeed - B40_PRIME64_1;

        do {
            v1 = b40_xxh64_round(v1, b40_read64le(p));
            p += 8;
            v2 = b40_xxh64_round(v2, b40_read64le(p));
            p += 8;
            v3 = b40_xxh64_round(v3, b40_read64le(p));
            p += 8;
            v4 = b40_xxh64_round(v4, b40_read64le(p));
            p += 8;
        } while (p <= pLimit);
        h = b40_rotl64(v1, 1) + b40_rotl64(v2, 7) + b40_rotl64(v3, 12) +
            b40_rotl64(v4, 18);
        h = b40_xxh64_merge(h, v1);
        h = b40_xxh64_merge(h, v2);
        h = b40_xxh64_merge(h, v3);
        h = b40_xxh64_merge(h, v4);
    } else {
        h = uSeed + B40_PRIME64_5;
    }
    h += (uint64_t)cb;
    {
        const unsigned char *pEnd = (const unsigned char *)pInput + cb;

        while (p + 8u <= pEnd) {
            uint64_t k1 = b40_xxh64_round(0, b40_read64le(p));

            h ^= k1;
            h = b40_rotl64(h, 27) * B40_PRIME64_1 + B40_PRIME64_4;
            p += 8;
        }
        if (p + 4u <= pEnd) {
            h ^= (uint64_t)b40_read32le(p) * B40_PRIME64_1;
            h = b40_rotl64(h, 23) * B40_PRIME64_2 + B40_PRIME64_3;
            p += 4;
        }
        while (p < pEnd) {
            h ^= (uint64_t)(*p) * B40_PRIME64_5;
            h = b40_rotl64(h, 11) * B40_PRIME64_1;
            p++;
        }
    }
    h ^= h >> 33;
    h *= B40_PRIME64_2;
    h ^= h >> 29;
    h *= B40_PRIME64_3;
    h ^= h >> 32;
    return h;
}

uint32_t __XXH32(const void *p, size_t n, uint32_t s)
    __attribute__((alias("XXH32")));
uint64_t __XXH64(const void *p, size_t n, uint64_t s)
    __attribute__((alias("XXH64")));

/* ---- HKDF / PBKDF2 / base64 (map + dyn surface) -------------------------- */


void hmac_sha1(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
               unsigned char aOut[20]);
void hmac_sha256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[32]);
void hmac_sha512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[64]);
void SHA512_Init(void *pCtx);

static void
b40_hmac_sha512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                unsigned char aOut[64])
{
    hmac_sha512(pKey, cbKey, pData, cbData, aOut);
}

int
hkdf_sha256(const void *pSalt, size_t cbSalt, const void *pIkm, size_t cbIkm,
            const void *pInfo, size_t cbInfo, unsigned char *pOkm, size_t cbOkm)
{
    unsigned char aPrk[32];
    unsigned char aT[32];
    unsigned char aBuf[32 + 256 + 1];
    size_t cbT = 0;
    size_t nOff = 0;
    unsigned char uCtr = 1;
    static const unsigned char aZeros[32] = {0};

    if (pOkm == NULL || cbOkm == 0u || cbOkm > 255u * 32u || cbInfo > 256u) {
        errno = EINVAL;
        return -1;
    }
    if (pSalt == NULL || cbSalt == 0u) {
        hmac_sha256(aZeros, 32u, pIkm, cbIkm, aPrk);
    } else {
        hmac_sha256(pSalt, cbSalt, pIkm, cbIkm, aPrk);
    }
    while (nOff < cbOkm) {
        size_t n = 0;
        size_t cbCopy;

        if (cbT > 0u) {
            memcpy(aBuf + n, aT, cbT);
            n += cbT;
        }
        if (pInfo != NULL && cbInfo > 0u) {
            memcpy(aBuf + n, pInfo, cbInfo);
            n += cbInfo;
        }
        aBuf[n++] = uCtr;
        hmac_sha256(aPrk, 32u, aBuf, n, aT);
        cbT = 32u;
        cbCopy = (cbOkm - nOff < 32u) ? (cbOkm - nOff) : 32u;
        memcpy(pOkm + nOff, aT, cbCopy);
        nOff += cbCopy;
        uCtr++;
    }
    memset(aPrk, 0, sizeof(aPrk));
    memset(aT, 0, sizeof(aT));
    return 0;
}

int
hkdf_sha512(const void *pSalt, size_t cbSalt, const void *pIkm, size_t cbIkm,
            const void *pInfo, size_t cbInfo, unsigned char *pOkm, size_t cbOkm)
{
    unsigned char aPrk[64];
    unsigned char aT[64];
    unsigned char aBuf[64 + 256 + 1];
    size_t cbT = 0;
    size_t nOff = 0;
    unsigned char uCtr = 1;
    static const unsigned char aZeros[64] = {0};

    if (pOkm == NULL || cbOkm == 0u || cbOkm > 255u * 64u || cbInfo > 256u) {
        errno = EINVAL;
        return -1;
    }
    if (pSalt == NULL || cbSalt == 0u) {
        b40_hmac_sha512(aZeros, 64u, pIkm, cbIkm, aPrk);
    } else {
        b40_hmac_sha512(pSalt, cbSalt, pIkm, cbIkm, aPrk);
    }
    while (nOff < cbOkm) {
        size_t n = 0;
        size_t cbCopy;

        if (cbT > 0u) {
            memcpy(aBuf + n, aT, cbT);
            n += cbT;
        }
        if (pInfo != NULL && cbInfo > 0u) {
            memcpy(aBuf + n, pInfo, cbInfo);
            n += cbInfo;
        }
        aBuf[n++] = uCtr;
        b40_hmac_sha512(aPrk, 64u, aBuf, n, aT);
        cbT = 64u;
        cbCopy = (cbOkm - nOff < 64u) ? (cbOkm - nOff) : 64u;
        memcpy(pOkm + nOff, aT, cbCopy);
        nOff += cbCopy;
        uCtr++;
    }
    memset(aPrk, 0, sizeof(aPrk));
    memset(aT, 0, sizeof(aT));
    return 0;
}

int __hkdf_sha256(const void *pSalt, size_t cbSalt, const void *pIkm,
                  size_t cbIkm, const void *pInfo, size_t cbInfo,
                  unsigned char *pOkm, size_t cbOkm)
    __attribute__((alias("hkdf_sha256")));
int __hkdf_sha512(const void *pSalt, size_t cbSalt, const void *pIkm,
                  size_t cbIkm, const void *pInfo, size_t cbInfo,
                  unsigned char *pOkm, size_t cbOkm)
    __attribute__((alias("hkdf_sha512")));

static int
b40_pbkdf2(void (*pfnHmac)(const void *, size_t, const void *, size_t,
                           unsigned char *),
           size_t cbHmac, const void *pPass, size_t cbPass,
           const unsigned char *pSalt, size_t cbSalt, unsigned uRounds,
           unsigned char *pOut, size_t cbOut)
{
    unsigned char aU[64];
    unsigned char aT[64];
    unsigned char aBlock[256 + 4];
    unsigned uBlk = 1;
    size_t nOff = 0;

    if (pOut == NULL || cbOut == 0u || uRounds == 0u || cbHmac > 64u ||
        cbSalt > 256u) {
        errno = EINVAL;
        return -1;
    }
    if (pPass == NULL) {
        pPass = "";
        cbPass = 0;
    }
    if (pSalt == NULL) {
        pSalt = (const unsigned char *)"";
        cbSalt = 0;
    }
    while (nOff < cbOut) {
        size_t i;
        size_t cbCopy;
        unsigned r;

        memcpy(aBlock, pSalt, cbSalt);
        aBlock[cbSalt] = (unsigned char)(uBlk >> 24);
        aBlock[cbSalt + 1u] = (unsigned char)(uBlk >> 16);
        aBlock[cbSalt + 2u] = (unsigned char)(uBlk >> 8);
        aBlock[cbSalt + 3u] = (unsigned char)uBlk;
        pfnHmac(pPass, cbPass, aBlock, cbSalt + 4u, aU);
        memcpy(aT, aU, cbHmac);
        for (r = 1; r < uRounds; r++) {
            pfnHmac(pPass, cbPass, aU, cbHmac, aU);
            for (i = 0; i < cbHmac; i++) {
                aT[i] ^= aU[i];
            }
        }
        cbCopy = (cbOut - nOff < cbHmac) ? (cbOut - nOff) : cbHmac;
        memcpy(pOut + nOff, aT, cbCopy);
        nOff += cbCopy;
        uBlk++;
    }
    memset(aU, 0, sizeof(aU));
    memset(aT, 0, sizeof(aT));
    return 0;
}

/* Wrapper to match generic pfn that writes into unsigned char * */
static void
b40_hmac_sha1_w(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                unsigned char *pOut)
{
    hmac_sha1(pKey, cbKey, pData, cbData, pOut);
}

static void
b40_hmac_sha256_w(const void *pKey, size_t cbKey, const void *pData,
                  size_t cbData, unsigned char *pOut)
{
    hmac_sha256(pKey, cbKey, pData, cbData, pOut);
}

int
pbkdf2_hmac_sha1(const void *pPass, size_t cbPass, const unsigned char *pSalt,
                 size_t cbSalt, unsigned uRounds, unsigned char *pOut,
                 size_t cbOut)
{
    return b40_pbkdf2(b40_hmac_sha1_w, 20u, pPass, cbPass, pSalt, cbSalt,
                      uRounds, pOut, cbOut);
}

int
pbkdf2_hmac_sha256(const void *pPass, size_t cbPass, const unsigned char *pSalt,
                   size_t cbSalt, unsigned uRounds, unsigned char *pOut,
                   size_t cbOut)
{
    return b40_pbkdf2(b40_hmac_sha256_w, 32u, pPass, cbPass, pSalt, cbSalt,
                      uRounds, pOut, pOut != NULL ? cbOut : 0u);
}

int __pbkdf2_hmac_sha1(const void *pPass, size_t cbPass,
                       const unsigned char *pSalt, size_t cbSalt,
                       unsigned uRounds, unsigned char *pOut, size_t cbOut)
    __attribute__((alias("pbkdf2_hmac_sha1")));
int __pbkdf2_hmac_sha256(const void *pPass, size_t cbPass,
                         const unsigned char *pSalt, size_t cbSalt,
                         unsigned uRounds, unsigned char *pOut, size_t cbOut)
    __attribute__((alias("pbkdf2_hmac_sha256")));

/* Standard base64 (RFC 4648) encode/decode */
static const char s_b40_b64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ssize_t
base64_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
    size_t i = 0;
    size_t j = 0;
    size_t cbNeed = ((cbIn + 2u) / 3u) * 4u + 1u;

    if (pIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (szOut == NULL) {
        return (ssize_t)cbNeed;
    }
    if (cbOut < cbNeed) {
        errno = ENOSPC;
        return -1;
    }
    while (i < cbIn) {
        size_t nLeft = cbIn - i;
        unsigned u0 = pIn[i++];
        unsigned u1 = (nLeft > 1u) ? pIn[i++] : 0u;
        unsigned u2 = (nLeft > 2u) ? pIn[i++] : 0u;
        unsigned uTrip = (u0 << 16) | (u1 << 8) | u2;

        szOut[j++] = s_b40_b64[(uTrip >> 18) & 63u];
        szOut[j++] = s_b40_b64[(uTrip >> 12) & 63u];
        if (nLeft > 1u) {
            szOut[j++] = s_b40_b64[(uTrip >> 6) & 63u];
        } else {
            szOut[j++] = '=';
        }
        if (nLeft > 2u) {
            szOut[j++] = s_b40_b64[uTrip & 63u];
        } else {
            szOut[j++] = '=';
        }
    }
    szOut[j] = '\0';
    return (ssize_t)j;
}

static int
b40_b64_val(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    }
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 26;
    }
    if (ch >= '0' && ch <= '9') {
        return ch - '0' + 52;
    }
    if (ch == '+') {
        return 62;
    }
    if (ch == '/') {
        return 63;
    }
    return -1;
}

ssize_t
base64_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
    size_t i = 0;
    size_t j = 0;

    if (szIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }
    while (i < cbIn) {
        int v0, v1, v2, v3;
        unsigned uTrip;
        int nOut;
        char c2, c3;

        while (i < cbIn && (szIn[i] == ' ' || szIn[i] == '\n' ||
                            szIn[i] == '\r' || szIn[i] == '\t')) {
            i++;
        }
        if (i >= cbIn) {
            break;
        }
        if (i + 4u > cbIn) {
            errno = EINVAL;
            return -1;
        }
        v0 = b40_b64_val((unsigned char)szIn[i]);
        v1 = b40_b64_val((unsigned char)szIn[i + 1u]);
        c2 = szIn[i + 2u];
        c3 = szIn[i + 3u];
        i += 4u;
        if (v0 < 0 || v1 < 0) {
            errno = EINVAL;
            return -1;
        }
        if (c2 == '=') {
            v2 = 0;
            nOut = 1;
            if (c3 != '=') {
                errno = EINVAL;
                return -1;
            }
            v3 = 0;
        } else {
            v2 = b40_b64_val((unsigned char)c2);
            if (v2 < 0) {
                errno = EINVAL;
                return -1;
            }
            if (c3 == '=') {
                v3 = 0;
                nOut = 2;
            } else {
                v3 = b40_b64_val((unsigned char)c3);
                if (v3 < 0) {
                    errno = EINVAL;
                    return -1;
                }
                nOut = 3;
            }
        }
        uTrip = ((unsigned)v0 << 18) | ((unsigned)v1 << 12) |
                ((unsigned)v2 << 6) | (unsigned)v3;
        if (pOut == NULL) {
            j += (size_t)nOut;
            continue;
        }
        if (j + (size_t)nOut > cbOut) {
            errno = ENOSPC;
            return -1;
        }
        pOut[j++] = (unsigned char)(uTrip >> 16);
        if (nOut > 1) {
            pOut[j++] = (unsigned char)(uTrip >> 8);
        }
        if (nOut > 2) {
            pOut[j++] = (unsigned char)uTrip;
        }
    }
    return (ssize_t)j;
}

ssize_t __base64_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base64_encode")));
ssize_t __base64_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base64_decode")));

/* ---- soft deepen: base64 length helper (unique) ------------------------- */

size_t
base64_encode_len(size_t cbIn)
{
    /* 4 * ceil(n/3) without including NUL */
    return ((cbIn + 2u) / 3u) * 4u;
}

size_t __base64_encode_len(size_t cbIn)
    __attribute__((alias("base64_encode_len")));

size_t
base64_decode_len(size_t cbB64)
{
    return (cbB64 / 4u) * 3u;
}

size_t __base64_decode_len(size_t cbB64)
    __attribute__((alias("base64_decode_len")));
