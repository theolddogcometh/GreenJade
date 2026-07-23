/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31: OpenBSD-style libmd digest completion
 * (MD5/SHA1/SHA256/SHA512 Update/Final/End/File/Data), BSD unvis family
 * (strunvis/unvis/strnvisx/strnunvisx). Integer/pointer only (no SSE).
 * Unique symbols — does NOT re-define batch30/32 GI aliases (see
 * graph_batch31.c.broken). Clean-room public ABI.
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Existing surface */
void MD5Init(void *pCtx);
void SHA1Init(void *pCtx);
void SHA256_Init(void *pCtx);
void *malloc(size_t cb);
void free(void *p);
int open(const char *szPath, int nFlags, ...);
int close(int nFd);
ssize_t read(int nFd, void *pBuf, size_t cb);
int strnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags);

/* ---- marker ------------------------------------------------------------- */

const char __libcgj_batch31_marker[] = "libcgj-batch31";

/* ---- context layouts (opaque to callers; size >= batch32 Init stubs) ---- */

struct b31_md5 {
    uint32_t aState[4];
    uint32_t aCount[2];
    unsigned char aBuf[64];
    unsigned char _pad[12]; /* total 92 */
};

struct b31_sha1 {
    uint32_t aState[5];
    uint32_t aCount[2];
    unsigned char aBuf[64];
    unsigned char _pad[8]; /* total 96 */
};

struct b31_sha256 {
    uint32_t aState[8];
    uint32_t aCount[2];
    unsigned char aBuf[64];
    unsigned char _pad[8]; /* total 112 */
};

struct b31_sha512 {
    uint64_t aState[8];
    uint64_t aCount[2];
    unsigned char aBuf[128];
    unsigned char _pad[16]; /* total 224 */
};

/* ---- endian helpers ----------------------------------------------------- */

static uint32_t
b31_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint64_t
b31_ror64(uint64_t x, unsigned n)
{
    return (x >> n) | (x << (64u - n));
}

static void
b31_put_be32(unsigned char *p, uint32_t v)
{
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}

static void
b31_put_be64(unsigned char *p, uint64_t v)
{
    p[0] = (unsigned char)(v >> 56);
    p[1] = (unsigned char)(v >> 48);
    p[2] = (unsigned char)(v >> 40);
    p[3] = (unsigned char)(v >> 32);
    p[4] = (unsigned char)(v >> 24);
    p[5] = (unsigned char)(v >> 16);
    p[6] = (unsigned char)(v >> 8);
    p[7] = (unsigned char)v;
}

static uint32_t
b31_get_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint32_t
b31_get_be32(const unsigned char *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static uint64_t
b31_get_be64(const unsigned char *p)
{
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

/* ---- ensure Init IVs if batch32 zero-stub was used ---------------------- */

static void
b31_md5_ensure(struct b31_md5 *p)
{
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aState[2] == 0u &&
        p->aState[3] == 0u && p->aCount[0] == 0u && p->aCount[1] == 0u) {
        p->aState[0] = 0x67452301u;
        p->aState[1] = 0xefcdab89u;
        p->aState[2] = 0x98badcfeu;
        p->aState[3] = 0x10325476u;
    }
}

static void
b31_sha1_ensure(struct b31_sha1 *p)
{
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aState[2] == 0u &&
        p->aState[3] == 0u && p->aState[4] == 0u && p->aCount[0] == 0u &&
        p->aCount[1] == 0u) {
        p->aState[0] = 0x67452301u;
        p->aState[1] = 0xefcdab89u;
        p->aState[2] = 0x98badcfeu;
        p->aState[3] = 0x10325476u;
        p->aState[4] = 0xc3d2e1f0u;
    }
}

static void
b31_sha256_ensure(struct b31_sha256 *p)
{
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aState[2] == 0u &&
        p->aState[3] == 0u && p->aState[4] == 0u && p->aState[5] == 0u &&
        p->aState[6] == 0u && p->aState[7] == 0u && p->aCount[0] == 0u &&
        p->aCount[1] == 0u) {
        p->aState[0] = 0x6a09e667u;
        p->aState[1] = 0xbb67ae85u;
        p->aState[2] = 0x3c6ef372u;
        p->aState[3] = 0xa54ff53au;
        p->aState[4] = 0x510e527fu;
        p->aState[5] = 0x9b05688cu;
        p->aState[6] = 0x1f83d9abu;
        p->aState[7] = 0x5be0cd19u;
    }
}

static void
b31_sha512_init(struct b31_sha512 *p)
{
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0x6a09e667f3bcc908ull;
    p->aState[1] = 0xbb67ae8584caa73bull;
    p->aState[2] = 0x3c6ef372fe94f82bull;
    p->aState[3] = 0xa54ff53a5f1d36f1ull;
    p->aState[4] = 0x510e527fade682d1ull;
    p->aState[5] = 0x9b05688c2b3e6c1full;
    p->aState[6] = 0x1f83d9abfb41bd6bull;
    p->aState[7] = 0x5be0cd19137e2179ull;
}

/* ---- MD5 (RFC 1321, clean-room) ----------------------------------------- */

static void
b31_md5_block(struct b31_md5 *p, const unsigned char *pBlk)
{
    uint32_t a = p->aState[0], b = p->aState[1], c = p->aState[2],
             d = p->aState[3];
    uint32_t m[16];
    unsigned i;

    for (i = 0; i < 16u; i++) {
        m[i] = b31_get_le32(pBlk + i * 4u);
    }

#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~(z))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~(z))))
#define STEP(f, w, x, y, z, k, s, t)                                          \
    do {                                                                      \
        (w) += f((x), (y), (z)) + (k) + (uint32_t)(t);                        \
        (w) = b31_rol32((w), (s));                                            \
        (w) += (x);                                                           \
    } while (0)

    STEP(F, a, b, c, d, m[0], 7, 0xd76aa478);
    STEP(F, d, a, b, c, m[1], 12, 0xe8c7b756);
    STEP(F, c, d, a, b, m[2], 17, 0x242070db);
    STEP(F, b, c, d, a, m[3], 22, 0xc1bdceee);
    STEP(F, a, b, c, d, m[4], 7, 0xf57c0faf);
    STEP(F, d, a, b, c, m[5], 12, 0x4787c62a);
    STEP(F, c, d, a, b, m[6], 17, 0xa8304613);
    STEP(F, b, c, d, a, m[7], 22, 0xfd469501);
    STEP(F, a, b, c, d, m[8], 7, 0x698098d8);
    STEP(F, d, a, b, c, m[9], 12, 0x8b44f7af);
    STEP(F, c, d, a, b, m[10], 17, 0xffff5bb1);
    STEP(F, b, c, d, a, m[11], 22, 0x895cd7be);
    STEP(F, a, b, c, d, m[12], 7, 0x6b901122);
    STEP(F, d, a, b, c, m[13], 12, 0xfd987193);
    STEP(F, c, d, a, b, m[14], 17, 0xa679438e);
    STEP(F, b, c, d, a, m[15], 22, 0x49b40821);

    STEP(G, a, b, c, d, m[1], 5, 0xf61e2562);
    STEP(G, d, a, b, c, m[6], 9, 0xc040b340);
    STEP(G, c, d, a, b, m[11], 14, 0x265e5a51);
    STEP(G, b, c, d, a, m[0], 20, 0xe9b6c7aa);
    STEP(G, a, b, c, d, m[5], 5, 0xd62f105d);
    STEP(G, d, a, b, c, m[10], 9, 0x02441453);
    STEP(G, c, d, a, b, m[15], 14, 0xd8a1e681);
    STEP(G, b, c, d, a, m[4], 20, 0xe7d3fbc8);
    STEP(G, a, b, c, d, m[9], 5, 0x21e1cde6);
    STEP(G, d, a, b, c, m[14], 9, 0xc33707d6);
    STEP(G, c, d, a, b, m[3], 14, 0xf4d50d87);
    STEP(G, b, c, d, a, m[8], 20, 0x455a14ed);
    STEP(G, a, b, c, d, m[13], 5, 0xa9e3e905);
    STEP(G, d, a, b, c, m[2], 9, 0xfcefa3f8);
    STEP(G, c, d, a, b, m[7], 14, 0x676f02d9);
    STEP(G, b, c, d, a, m[12], 20, 0x8d2a4c8a);

    STEP(H, a, b, c, d, m[5], 4, 0xfffa3942);
    STEP(H, d, a, b, c, m[8], 11, 0x8771f681);
    STEP(H, c, d, a, b, m[11], 16, 0x6d9d6122);
    STEP(H, b, c, d, a, m[14], 23, 0xfde5380c);
    STEP(H, a, b, c, d, m[1], 4, 0xa4beea44);
    STEP(H, d, a, b, c, m[4], 11, 0x4bdecfa9);
    STEP(H, c, d, a, b, m[7], 16, 0xf6bb4b60);
    STEP(H, b, c, d, a, m[10], 23, 0xbebfbc70);
    STEP(H, a, b, c, d, m[13], 4, 0x289b7ec6);
    STEP(H, d, a, b, c, m[0], 11, 0xeaa127fa);
    STEP(H, c, d, a, b, m[3], 16, 0xd4ef3085);
    STEP(H, b, c, d, a, m[6], 23, 0x04881d05);
    STEP(H, a, b, c, d, m[9], 4, 0xd9d4d039);
    STEP(H, d, a, b, c, m[12], 11, 0xe6db99e5);
    STEP(H, c, d, a, b, m[15], 16, 0x1fa27cf8);
    STEP(H, b, c, d, a, m[2], 23, 0xc4ac5665);

    STEP(I, a, b, c, d, m[0], 6, 0xf4292244);
    STEP(I, d, a, b, c, m[7], 10, 0x432aff97);
    STEP(I, c, d, a, b, m[14], 15, 0xab9423a7);
    STEP(I, b, c, d, a, m[5], 21, 0xfc93a039);
    STEP(I, a, b, c, d, m[12], 6, 0x655b59c3);
    STEP(I, d, a, b, c, m[3], 10, 0x8f0ccc92);
    STEP(I, c, d, a, b, m[10], 15, 0xffeff47d);
    STEP(I, b, c, d, a, m[1], 21, 0x85845dd1);
    STEP(I, a, b, c, d, m[8], 6, 0x6fa87e4f);
    STEP(I, d, a, b, c, m[15], 10, 0xfe2ce6e0);
    STEP(I, c, d, a, b, m[6], 15, 0xa3014314);
    STEP(I, b, c, d, a, m[13], 21, 0x4e0811a1);
    STEP(I, a, b, c, d, m[4], 6, 0xf7537e82);
    STEP(I, d, a, b, c, m[11], 10, 0xbd3af235);
    STEP(I, c, d, a, b, m[2], 15, 0x2ad7d2bb);
    STEP(I, b, c, d, a, m[9], 21, 0xeb86d391);

#undef F
#undef G
#undef H
#undef I
#undef STEP

    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
}

void
MD5Update(void *pCtx, const void *pData, size_t cb)
{
    struct b31_md5 *p = (struct b31_md5 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    uint32_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    b31_md5_ensure(p);
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    p->aCount[0] += (uint32_t)(cb << 3);
    if (p->aCount[0] < (uint32_t)(cb << 3)) {
        p->aCount[1]++;
    }
    p->aCount[1] += (uint32_t)(cb >> 29);
    nPart = 64u - (size_t)nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b31_md5_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 64u) {
            b31_md5_block(p, q);
            q += 64u;
            cb -= 64u;
        }
        nIndex = 0;
    }
    if (cb > 0u) {
        memcpy(p->aBuf + nIndex, q, cb);
    }
}

void
MD5Final(unsigned char aDig[16], void *pCtx)
{
    struct b31_md5 *p = (struct b31_md5 *)pCtx;
    unsigned char aBits[8];
    uint32_t nIndex;
    uint32_t nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    b31_md5_ensure(p);
    for (i = 0; i < 4u; i++) {
        aBits[i] = (unsigned char)(p->aCount[0] >> (i * 8u));
        aBits[i + 4u] = (unsigned char)(p->aCount[1] >> (i * 8u));
    }
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    nPad = (nIndex < 56u) ? (56u - nIndex) : (120u - nIndex);
    {
        unsigned char aPad[64];

        memset(aPad, 0, sizeof(aPad));
        aPad[0] = 0x80u;
        MD5Update(p, aPad, nPad);
    }
    MD5Update(p, aBits, 8u);
    for (i = 0; i < 4u; i++) {
        aDig[i * 4u] = (unsigned char)p->aState[i];
        aDig[i * 4u + 1u] = (unsigned char)(p->aState[i] >> 8);
        aDig[i * 4u + 2u] = (unsigned char)(p->aState[i] >> 16);
        aDig[i * 4u + 3u] = (unsigned char)(p->aState[i] >> 24);
    }
    memset(p, 0, sizeof(*p));
}

static char *
b31_hex_end(const unsigned char *pDig, size_t cbDig, char *szBuf)
{
    static const char aHex[] = "0123456789abcdef";
    size_t i;

    if (szBuf == NULL) {
        szBuf = (char *)malloc(cbDig * 2u + 1u);
        if (szBuf == NULL) {
            return NULL;
        }
    }
    for (i = 0; i < cbDig; i++) {
        szBuf[i * 2u] = aHex[(pDig[i] >> 4) & 0xfu];
        szBuf[i * 2u + 1u] = aHex[pDig[i] & 0xfu];
    }
    szBuf[cbDig * 2u] = '\0';
    return szBuf;
}

char *
MD5End(void *pCtx, char *szBuf)
{
    unsigned char aDig[16];

    MD5Final(aDig, pCtx);
    return b31_hex_end(aDig, 16u, szBuf);
}

char *
MD5Data(const void *pData, size_t cb, char *szBuf)
{
    struct b31_md5 ctx;

    memset(&ctx, 0, sizeof(ctx));
    MD5Init(&ctx);
    MD5Update(&ctx, pData, cb);
    return MD5End(&ctx, szBuf);
}

char *
MD5File(const char *szPath, char *szBuf)
{
    int nFd;
    struct b31_md5 ctx;
    unsigned char aBuf[512];
    ssize_t n;

    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    nFd = open(szPath, O_RDONLY);
    if (nFd < 0) {
        return NULL;
    }
    memset(&ctx, 0, sizeof(ctx));
    MD5Init(&ctx);
    while ((n = read(nFd, aBuf, sizeof(aBuf))) > 0) {
        MD5Update(&ctx, aBuf, (size_t)n);
    }
    (void)close(nFd);
    if (n < 0) {
        return NULL;
    }
    return MD5End(&ctx, szBuf);
}

/* ---- SHA-1 (FIPS 180-1, clean-room) ------------------------------------- */

static void
b31_sha1_block(struct b31_sha1 *p, const unsigned char *pBlk)
{
    uint32_t w[80];
    uint32_t a, b, c, d, e;
    unsigned i;

    for (i = 0; i < 16u; i++) {
        w[i] = b31_get_be32(pBlk + i * 4u);
    }
    for (i = 16; i < 80u; i++) {
        w[i] = b31_rol32(w[i - 3u] ^ w[i - 8u] ^ w[i - 14u] ^ w[i - 16u], 1u);
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    for (i = 0; i < 80u; i++) {
        uint32_t f, k, t;

        if (i < 20u) {
            f = (b & c) | ((~b) & d);
            k = 0x5a827999u;
        } else if (i < 40u) {
            f = b ^ c ^ d;
            k = 0x6ed9eba1u;
        } else if (i < 60u) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8f1bbcdcu;
        } else {
            f = b ^ c ^ d;
            k = 0xca62c1d6u;
        }
        t = b31_rol32(a, 5u) + f + e + k + w[i];
        e = d;
        d = c;
        c = b31_rol32(b, 30u);
        b = a;
        a = t;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
}

void
SHA1Update(void *pCtx, const void *pData, size_t cb)
{
    struct b31_sha1 *p = (struct b31_sha1 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    uint32_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    b31_sha1_ensure(p);
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    p->aCount[0] += (uint32_t)(cb << 3);
    if (p->aCount[0] < (uint32_t)(cb << 3)) {
        p->aCount[1]++;
    }
    p->aCount[1] += (uint32_t)(cb >> 29);
    nPart = 64u - (size_t)nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b31_sha1_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 64u) {
            b31_sha1_block(p, q);
            q += 64u;
            cb -= 64u;
        }
        nIndex = 0;
    }
    if (cb > 0u) {
        memcpy(p->aBuf + nIndex, q, cb);
    }
}

void
SHA1Final(unsigned char aDig[20], void *pCtx)
{
    struct b31_sha1 *p = (struct b31_sha1 *)pCtx;
    unsigned char aBits[8];
    uint32_t nIndex, nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    b31_sha1_ensure(p);
    b31_put_be32(aBits, p->aCount[1]);
    b31_put_be32(aBits + 4, p->aCount[0]);
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    nPad = (nIndex < 56u) ? (56u - nIndex) : (120u - nIndex);
    {
        unsigned char aPad[64];

        memset(aPad, 0, sizeof(aPad));
        aPad[0] = 0x80u;
        SHA1Update(p, aPad, nPad);
    }
    SHA1Update(p, aBits, 8u);
    for (i = 0; i < 5u; i++) {
        b31_put_be32(aDig + i * 4u, p->aState[i]);
    }
    memset(p, 0, sizeof(*p));
}

char *
SHA1End(void *pCtx, char *szBuf)
{
    unsigned char aDig[20];

    SHA1Final(aDig, pCtx);
    return b31_hex_end(aDig, 20u, szBuf);
}

char *
SHA1Data(const void *pData, size_t cb, char *szBuf)
{
    struct b31_sha1 ctx;

    memset(&ctx, 0, sizeof(ctx));
    SHA1Init(&ctx);
    SHA1Update(&ctx, pData, cb);
    return SHA1End(&ctx, szBuf);
}

/* ---- SHA-256 (FIPS 180-4, clean-room) ----------------------------------- */

static const uint32_t s_b31_k256[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

static uint32_t
b31_ror32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void
b31_sha256_block(struct b31_sha256 *p, const unsigned char *pBlk)
{
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    unsigned i;

    for (i = 0; i < 16u; i++) {
        w[i] = b31_get_be32(pBlk + i * 4u);
    }
    for (i = 16; i < 64u; i++) {
        uint32_t s0 = b31_ror32(w[i - 15u], 7u) ^ b31_ror32(w[i - 15u], 18u) ^
                      (w[i - 15u] >> 3);
        uint32_t s1 = b31_ror32(w[i - 2u], 17u) ^ b31_ror32(w[i - 2u], 19u) ^
                      (w[i - 2u] >> 10);
        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    f = p->aState[5];
    g = p->aState[6];
    h = p->aState[7];
    for (i = 0; i < 64u; i++) {
        uint32_t S1 = b31_ror32(e, 6u) ^ b31_ror32(e, 11u) ^ b31_ror32(e, 25u);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t t1 = h + S1 + ch + s_b31_k256[i] + w[i];
        uint32_t S0 = b31_ror32(a, 2u) ^ b31_ror32(a, 13u) ^ b31_ror32(a, 22u);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
    p->aState[5] += f;
    p->aState[6] += g;
    p->aState[7] += h;
}

void
SHA256_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b31_sha256 *p = (struct b31_sha256 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    uint32_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    b31_sha256_ensure(p);
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    p->aCount[0] += (uint32_t)(cb << 3);
    if (p->aCount[0] < (uint32_t)(cb << 3)) {
        p->aCount[1]++;
    }
    p->aCount[1] += (uint32_t)(cb >> 29);
    nPart = 64u - (size_t)nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b31_sha256_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 64u) {
            b31_sha256_block(p, q);
            q += 64u;
            cb -= 64u;
        }
        nIndex = 0;
    }
    if (cb > 0u) {
        memcpy(p->aBuf + nIndex, q, cb);
    }
}

void
SHA256_Final(unsigned char aDig[32], void *pCtx)
{
    struct b31_sha256 *p = (struct b31_sha256 *)pCtx;
    unsigned char aBits[8];
    uint32_t nIndex, nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    b31_sha256_ensure(p);
    b31_put_be32(aBits, p->aCount[1]);
    b31_put_be32(aBits + 4, p->aCount[0]);
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    nPad = (nIndex < 56u) ? (56u - nIndex) : (120u - nIndex);
    {
        unsigned char aPad[64];

        memset(aPad, 0, sizeof(aPad));
        aPad[0] = 0x80u;
        SHA256_Update(p, aPad, nPad);
    }
    SHA256_Update(p, aBits, 8u);
    for (i = 0; i < 8u; i++) {
        b31_put_be32(aDig + i * 4u, p->aState[i]);
    }
    memset(p, 0, sizeof(*p));
}

char *
SHA256_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[32];

    SHA256_Final(aDig, pCtx);
    return b31_hex_end(aDig, 32u, szBuf);
}

char *
SHA256_Data(const void *pData, size_t cb, char *szBuf)
{
    struct b31_sha256 ctx;

    memset(&ctx, 0, sizeof(ctx));
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, pData, cb);
    return SHA256_End(&ctx, szBuf);
}

/* ---- SHA-512 (FIPS 180-4, clean-room) ----------------------------------- */

static const uint64_t s_b31_k512[80] = {
    0x428a2f98d728ae22ull, 0x7137449123ef65cdull, 0xb5c0fbcfec4d3b2full,
    0xe9b5dba58189dbbcull, 0x3956c25bf348b538ull, 0x59f111f1b605d019ull,
    0x923f82a4af194f9bull, 0xab1c5ed5da6d8118ull, 0xd807aa98a3030242ull,
    0x12835b0145706fbeull, 0x243185be4ee4b28cull, 0x550c7dc3d5ffb4e2ull,
    0x72be5d74f27b896full, 0x80deb1fe3b1696b1ull, 0x9bdc06a725c71235ull,
    0xc19bf174cf692694ull, 0xe49b69c19ef14ad2ull, 0xefbe4786384f25e3ull,
    0x0fc19dc68b8cd5b5ull, 0x240ca1cc77ac9c65ull, 0x2de92c6f592b0275ull,
    0x4a7484aa6ea6e483ull, 0x5cb0a9dcbd41fbd4ull, 0x76f988da831153b5ull,
    0x983e5152ee66dfabull, 0xa831c66d2db43210ull, 0xb00327c898fb213full,
    0xbf597fc7beef0ee4ull, 0xc6e00bf33da88fc2ull, 0xd5a79147930aa725ull,
    0x06ca6351e003826full, 0x142929670a0e6e70ull, 0x27b70a8546d22ffcull,
    0x2e1b21385c26c926ull, 0x4d2c6dfc5ac42aedull, 0x53380d139d95b3dfull,
    0x650a73548baf63deull, 0x766a0abb3c77b2a8ull, 0x81c2c92e47edaee6ull,
    0x92722c851482353bull, 0xa2bfe8a14cf10364ull, 0xa81a664bbc423001ull,
    0xc24b8b70d0f89791ull, 0xc76c51a30654be30ull, 0xd192e819d6ef5218ull,
    0xd69906245565a910ull, 0xf40e35855771202aull, 0x106aa07032bbd1b8ull,
    0x19a4c116b8d2d0c8ull, 0x1e376c085141ab53ull, 0x2748774cdf8eeb99ull,
    0x34b0bcb5e19b48a8ull, 0x391c0cb3c5c95a63ull, 0x4ed8aa4ae3418acbull,
    0x5b9cca4f7763e373ull, 0x682e6ff3d6b2b8a3ull, 0x748f82ee5defb2fcull,
    0x78a5636f43172f60ull, 0x84c87814a1f0ab72ull, 0x8cc702081a6439ecull,
    0x90befffa23631e28ull, 0xa4506cebde82bde9ull, 0xbef9a3f7b2c67915ull,
    0xc67178f2e372532bull, 0xca273eceea26619cull, 0xd186b8c721c0c207ull,
    0xeada7dd6cde0eb1eull, 0xf57d4f7fee6ed178ull, 0x06f067aa72176fbaull,
    0x0a637dc5a2c898a6ull, 0x113f9804bef90daeull, 0x1b710b35131c471bull,
    0x28db77f523047d84ull, 0x32caab7b40c72493ull, 0x3c9ebe0a15c9bebcull,
    0x431d67c49c100d4cull, 0x4cc5d4becb3e42b6ull, 0x597f299cfc657e2aull,
    0x5fcb6fab3ad6faecull, 0x6c44198c4a475817ull};

static void
b31_sha512_block(struct b31_sha512 *p, const unsigned char *pBlk)
{
    uint64_t w[80];
    uint64_t a, b, c, d, e, f, g, h;
    unsigned i;

    for (i = 0; i < 16u; i++) {
        w[i] = b31_get_be64(pBlk + i * 8u);
    }
    for (i = 16; i < 80u; i++) {
        uint64_t s0 = b31_ror64(w[i - 15u], 1u) ^ b31_ror64(w[i - 15u], 8u) ^
                      (w[i - 15u] >> 7);
        uint64_t s1 = b31_ror64(w[i - 2u], 19u) ^ b31_ror64(w[i - 2u], 61u) ^
                      (w[i - 2u] >> 6);
        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    f = p->aState[5];
    g = p->aState[6];
    h = p->aState[7];
    for (i = 0; i < 80u; i++) {
        uint64_t S1 =
            b31_ror64(e, 14u) ^ b31_ror64(e, 18u) ^ b31_ror64(e, 41u);
        uint64_t ch = (e & f) ^ ((~e) & g);
        uint64_t t1 = h + S1 + ch + s_b31_k512[i] + w[i];
        uint64_t S0 =
            b31_ror64(a, 28u) ^ b31_ror64(a, 34u) ^ b31_ror64(a, 39u);
        uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint64_t t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
    p->aState[5] += f;
    p->aState[6] += g;
    p->aState[7] += h;
}

void
SHA512_Init(void *pCtx)
{
    if (pCtx != NULL) {
        b31_sha512_init((struct b31_sha512 *)pCtx);
    }
}

void
SHA512_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b31_sha512 *p = (struct b31_sha512 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    size_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        b31_sha512_init(p);
    }
    nIndex = (size_t)((p->aCount[1] >> 3) & 0x7full);
    {
        uint64_t nBits = (uint64_t)cb << 3;

        p->aCount[1] += nBits;
        if (p->aCount[1] < nBits) {
            p->aCount[0]++;
        }
        p->aCount[0] += (uint64_t)cb >> 61;
    }
    nPart = 128u - nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b31_sha512_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 128u) {
            b31_sha512_block(p, q);
            q += 128u;
            cb -= 128u;
        }
        nIndex = 0;
    }
    if (cb > 0u) {
        memcpy(p->aBuf + nIndex, q, cb);
    }
}

void
SHA512_Final(unsigned char aDig[64], void *pCtx)
{
    struct b31_sha512 *p = (struct b31_sha512 *)pCtx;
    unsigned char aBits[16];
    size_t nIndex, nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->aState[0] == 0ull && p->aState[1] == 0ull && p->aCount[0] == 0ull &&
        p->aCount[1] == 0ull) {
        b31_sha512_init(p);
    }
    b31_put_be64(aBits, p->aCount[0]);
    b31_put_be64(aBits + 8, p->aCount[1]);
    nIndex = (size_t)((p->aCount[1] >> 3) & 0x7full);
    nPad = (nIndex < 112u) ? (112u - nIndex) : (240u - nIndex);
    {
        unsigned char aPad[128];

        memset(aPad, 0, sizeof(aPad));
        aPad[0] = 0x80u;
        SHA512_Update(p, aPad, nPad);
    }
    SHA512_Update(p, aBits, 16u);
    for (i = 0; i < 8u; i++) {
        b31_put_be64(aDig + i * 8u, p->aState[i]);
    }
    memset(p, 0, sizeof(*p));
}

char *
SHA512_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[64];

    SHA512_Final(aDig, pCtx);
    return b31_hex_end(aDig, 64u, szBuf);
}

char *
SHA512_Data(const void *pData, size_t cb, char *szBuf)
{
    struct b31_sha512 ctx;

    SHA512_Init(&ctx);
    SHA512_Update(&ctx, pData, cb);
    return SHA512_End(&ctx, szBuf);
}

/* ---- BSD unvis / strnvisx ----------------------------------------------- */

int
unvis(char *pCp, int nChar, int *pState, int nFlag)
{
    (void)nFlag;
    if (pCp == NULL || pState == NULL) {
        errno = EINVAL;
        return -1;
    }
    /* Minimal: pass-through single-byte tokens (full unvis is stateful). */
    if (*pState == 0) {
        if (nChar == '\\') {
            *pState = 1;
            return 0; /* need more */
        }
        *pCp = (char)nChar;
        return 1; /* valid */
    }
    /* after backslash: accept next char literally */
    *pCp = (char)nChar;
    *pState = 0;
    return 1;
}

int
strunvis(char *pDst, const char *szSrc)
{
    size_t i = 0;
    size_t j = 0;
    int nState = 0;

    if (pDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szSrc == NULL) {
        pDst[0] = '\0';
        return 0;
    }
    while (szSrc[i] != '\0') {
        char ch = 0;
        int n = unvis(&ch, (unsigned char)szSrc[i], &nState, 0);

        i++;
        if (n == 1) {
            pDst[j++] = ch;
        } else if (n < 0) {
            return -1;
        }
    }
    pDst[j] = '\0';
    return (int)j;
}

int
strnvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc, int nFlags)
{
    char aTmp[512];
    size_t nCopy;
    char aSrc[512];

    (void)nFlags;
    if (pDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    nCopy = cbSrc;
    if (nCopy >= sizeof(aSrc)) {
        nCopy = sizeof(aSrc) - 1u;
    }
    if (szSrc != NULL && nCopy > 0u) {
        memcpy(aSrc, szSrc, nCopy);
    }
    aSrc[nCopy] = '\0';
    if (strnvis(aTmp, sizeof(aTmp), aSrc, nFlags) < 0) {
        return -1;
    }
    {
        size_t n = strlen(aTmp);
        size_t nOut = (n + 1u < cbDst) ? n : (cbDst - 1u);

        memcpy(pDst, aTmp, nOut);
        pDst[nOut] = '\0';
        return (int)nOut;
    }
}

int
strnunvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc)
{
    char aSrc[512];
    char aOut[512];
    size_t nCopy;
    int nLen;

    if (pDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    nCopy = cbSrc;
    if (nCopy >= sizeof(aSrc)) {
        nCopy = sizeof(aSrc) - 1u;
    }
    if (szSrc != NULL && nCopy > 0u) {
        memcpy(aSrc, szSrc, nCopy);
    }
    aSrc[nCopy] = '\0';
    nLen = strunvis(aOut, aSrc);
    if (nLen < 0) {
        return -1;
    }
    if ((size_t)nLen + 1u > cbDst) {
        nLen = (int)(cbDst - 1u);
    }
    memcpy(pDst, aOut, (size_t)nLen);
    pDst[nLen] = '\0';
    return nLen;
}

/* ---- soft deepen: FileChunk digesters (unique; full File above) --------- */

char *
MD5FileChunk(const char *szPath, char *szBuf, off_t off, off_t cb)
{
    int nFd;
    struct b31_md5 ctx;
    unsigned char aBuf[512];
    ssize_t n;
    off_t cbLeft;

    if (szPath == NULL || cb < 0) {
        errno = EINVAL;
        return NULL;
    }
    nFd = open(szPath, O_RDONLY);
    if (nFd < 0) {
        return NULL;
    }
    if (off > 0 && lseek(nFd, off, SEEK_SET) != off) {
        (void)close(nFd);
        return NULL;
    }
    memset(&ctx, 0, sizeof(ctx));
    MD5Init(&ctx);
    cbLeft = cb;
    while (cbLeft > 0) {
        size_t cbWant = (size_t)((cbLeft > (off_t)sizeof(aBuf))
                                     ? (off_t)sizeof(aBuf)
                                     : cbLeft);
        n = read(nFd, aBuf, cbWant);
        if (n <= 0) {
            break;
        }
        MD5Update(&ctx, aBuf, (size_t)n);
        cbLeft -= (off_t)n;
    }
    (void)close(nFd);
    if (n < 0) {
        return NULL;
    }
    return MD5End(&ctx, szBuf);
}

char *__MD5FileChunk(const char *szPath, char *szBuf, off_t off, off_t cb)
    __attribute__((alias("MD5FileChunk")));
