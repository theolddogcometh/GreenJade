/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42: SM3 (GM/T 0004-2012), ChaCha20 + Poly1305
 * (RFC 8439), Bitcoin base58, CRC-8/SMBUS, CRC-16/MODBUS, HMAC-SHA3-512,
 * RFC 4648 base32hex. Integer/pointer only (no SSE). Clean-room public ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* SHA3-512 from batch40 for HMAC-SHA3-512 */
void SHA3_512_Init(void *pCtx);
void SHA3_512_Update(void *pCtx, const void *pData, size_t cb);
void SHA3_512_Final(unsigned char aDig[64], void *pCtx);

const char __libcgj_batch42_marker[] = "libcgj-batch42";

/* ---- shared helpers ----------------------------------------------------- */

static char *
b42_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
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

static uint32_t
b42_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint32_t
b42_load_be32(const unsigned char *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void
b42_store_be32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)(x >> 24);
    p[1] = (unsigned char)(x >> 16);
    p[2] = (unsigned char)(x >> 8);
    p[3] = (unsigned char)x;
}

static uint32_t
b42_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b42_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

/* ========================================================================
 * SM3 (GM/T 0004-2012) — 256-bit Chinese national hash
 * ======================================================================== */

struct b42_sm3 {
    uint32_t aH[8];
    unsigned char aBuf[64];
    size_t cbBuf;
    uint64_t cbTotal;
};

static uint32_t
b42_sm3_p0(uint32_t x)
{
    return x ^ b42_rol32(x, 9) ^ b42_rol32(x, 17);
}

static uint32_t
b42_sm3_p1(uint32_t x)
{
    return x ^ b42_rol32(x, 15) ^ b42_rol32(x, 23);
}

static uint32_t
b42_sm3_ff(uint32_t x, uint32_t y, uint32_t z, unsigned j)
{
    if (j < 16u) {
        return x ^ y ^ z;
    }
    return (x & y) | (x & z) | (y & z);
}

static uint32_t
b42_sm3_gg(uint32_t x, uint32_t y, uint32_t z, unsigned j)
{
    if (j < 16u) {
        return x ^ y ^ z;
    }
    return (x & y) | ((~x) & z);
}

static uint32_t
b42_sm3_t(unsigned j)
{
    return (j < 16u) ? 0x79cc4519u : 0x7a879d8au;
}

static void
b42_sm3_compress(struct b42_sm3 *p, const unsigned char aBlock[64])
{
    uint32_t w[68];
    uint32_t w1[64];
    uint32_t a, b, c, d, e, f, g, h;
    unsigned j;

    for (j = 0; j < 16u; j++) {
        w[j] = b42_load_be32(aBlock + j * 4u);
    }
    for (j = 16; j < 68u; j++) {
        w[j] = b42_sm3_p1(w[j - 16u] ^ w[j - 9u] ^ b42_rol32(w[j - 3u], 15)) ^
               b42_rol32(w[j - 13u], 7) ^ w[j - 6u];
    }
    for (j = 0; j < 64u; j++) {
        w1[j] = w[j] ^ w[j + 4u];
    }

    a = p->aH[0];
    b = p->aH[1];
    c = p->aH[2];
    d = p->aH[3];
    e = p->aH[4];
    f = p->aH[5];
    g = p->aH[6];
    h = p->aH[7];

    for (j = 0; j < 64u; j++) {
        uint32_t ss1 =
            b42_rol32(b42_rol32(a, 12) + e + b42_rol32(b42_sm3_t(j), j), 7);
        uint32_t ss2 = ss1 ^ b42_rol32(a, 12);
        uint32_t tt1 = b42_sm3_ff(a, b, c, j) + d + ss2 + w1[j];
        uint32_t tt2 = b42_sm3_gg(e, f, g, j) + h + ss1 + w[j];

        d = c;
        c = b42_rol32(b, 9);
        b = a;
        a = tt1;
        h = g;
        g = b42_rol32(f, 19);
        f = e;
        e = b42_sm3_p0(tt2);
    }

    p->aH[0] ^= a;
    p->aH[1] ^= b;
    p->aH[2] ^= c;
    p->aH[3] ^= d;
    p->aH[4] ^= e;
    p->aH[5] ^= f;
    p->aH[6] ^= g;
    p->aH[7] ^= h;
}

void
SM3_Init(void *pCtx)
{
    struct b42_sm3 *p = (struct b42_sm3 *)pCtx;

    if (p == NULL) {
        return;
    }
    p->aH[0] = 0x7380166fu;
    p->aH[1] = 0x4914b2b9u;
    p->aH[2] = 0x172442d7u;
    p->aH[3] = 0xda8a0600u;
    p->aH[4] = 0xa96f30bcu;
    p->aH[5] = 0x163138aau;
    p->aH[6] = 0xe38dee4du;
    p->aH[7] = 0xb0fb0e4eu;
    p->cbBuf = 0;
    p->cbTotal = 0;
}

void
SM3_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b42_sm3 *p = (struct b42_sm3 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0u)) {
        return;
    }
    p->cbTotal += (uint64_t)cb;
    while (cb > 0u) {
        size_t nLeft = 64u - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;

        memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == 64u) {
            b42_sm3_compress(p, p->aBuf);
            p->cbBuf = 0;
        }
    }
}

void
SM3_Final(unsigned char aDig[32], void *pCtx)
{
    struct b42_sm3 *p = (struct b42_sm3 *)pCtx;
    uint64_t cbBits;
    size_t i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    cbBits = p->cbTotal * 8ull;
    p->aBuf[p->cbBuf++] = 0x80u;
    if (p->cbBuf > 56u) {
        while (p->cbBuf < 64u) {
            p->aBuf[p->cbBuf++] = 0;
        }
        b42_sm3_compress(p, p->aBuf);
        p->cbBuf = 0;
    }
    while (p->cbBuf < 56u) {
        p->aBuf[p->cbBuf++] = 0;
    }
    /* 64-bit length big-endian */
    for (i = 0; i < 8u; i++) {
        p->aBuf[56u + i] = (unsigned char)(cbBits >> (56u - 8u * i));
    }
    b42_sm3_compress(p, p->aBuf);
    for (i = 0; i < 8u; i++) {
        b42_store_be32(aDig + i * 4u, p->aH[i]);
    }
    memset(p, 0, sizeof(*p));
}

int
SM3_Data(const void *pData, size_t cb, unsigned char aDig[32])
{
    unsigned char aCtx[sizeof(struct b42_sm3) + 16u];

    if (aDig == NULL) {
        return -1;
    }
    SM3_Init(aCtx);
    SM3_Update(aCtx, pData, cb);
    SM3_Final(aDig, aCtx);
    return 0;
}

char *
SM3_Hex(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aDig[32];

    if (SM3_Data(pData, cb, aDig) != 0) {
        return NULL;
    }
    return b42_hex_end(aDig, 32u, szBuf);
}

void __SM3_Init(void *p) __attribute__((alias("SM3_Init")));
void __SM3_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SM3_Update")));
void __SM3_Final(unsigned char a[32], void *p)
    __attribute__((alias("SM3_Final")));
int __SM3_Data(const void *d, size_t n, unsigned char a[32])
    __attribute__((alias("SM3_Data")));

/* ========================================================================
 * ChaCha20 (RFC 8439) — 20-round stream cipher
 * ======================================================================== */

static void
b42_chacha_qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a += *b;
    *d ^= *a;
    *d = b42_rol32(*d, 16);
    *c += *d;
    *b ^= *c;
    *b = b42_rol32(*b, 12);
    *a += *b;
    *d ^= *a;
    *d = b42_rol32(*d, 8);
    *c += *d;
    *b ^= *c;
    *b = b42_rol32(*b, 7);
}

static void
b42_chacha20_block(const uint32_t aIn[16], unsigned char aOut[64])
{
    uint32_t x[16];
    unsigned i;

    for (i = 0; i < 16u; i++) {
        x[i] = aIn[i];
    }
    for (i = 0; i < 10u; i++) {
        /* column rounds */
        b42_chacha_qr(&x[0], &x[4], &x[8], &x[12]);
        b42_chacha_qr(&x[1], &x[5], &x[9], &x[13]);
        b42_chacha_qr(&x[2], &x[6], &x[10], &x[14]);
        b42_chacha_qr(&x[3], &x[7], &x[11], &x[15]);
        /* diagonal rounds */
        b42_chacha_qr(&x[0], &x[5], &x[10], &x[15]);
        b42_chacha_qr(&x[1], &x[6], &x[11], &x[12]);
        b42_chacha_qr(&x[2], &x[7], &x[8], &x[13]);
        b42_chacha_qr(&x[3], &x[4], &x[9], &x[14]);
    }
    for (i = 0; i < 16u; i++) {
        b42_store_le32(aOut + i * 4u, x[i] + aIn[i]);
    }
}

static void
b42_chacha20_init_state(uint32_t aSt[16], const unsigned char aKey[32],
                        const unsigned char aNonce[12], uint32_t uCounter)
{
    /* "expand 32-byte k" */
    aSt[0] = 0x61707865u;
    aSt[1] = 0x3320646eu;
    aSt[2] = 0x79622d32u;
    aSt[3] = 0x6b206574u;
    aSt[4] = b42_load_le32(aKey + 0);
    aSt[5] = b42_load_le32(aKey + 4);
    aSt[6] = b42_load_le32(aKey + 8);
    aSt[7] = b42_load_le32(aKey + 12);
    aSt[8] = b42_load_le32(aKey + 16);
    aSt[9] = b42_load_le32(aKey + 20);
    aSt[10] = b42_load_le32(aKey + 24);
    aSt[11] = b42_load_le32(aKey + 28);
    aSt[12] = uCounter;
    aSt[13] = b42_load_le32(aNonce + 0);
    aSt[14] = b42_load_le32(aNonce + 4);
    aSt[15] = b42_load_le32(aNonce + 8);
}

/*
 * chacha20_xor: RFC 8439 §2.4 — XOR keystream over pIn → pOut.
 * aKey[32], aNonce[12], uCounter starts the block counter.
 */
int
chacha20_xor(const unsigned char aKey[32], const unsigned char aNonce[12],
             uint32_t uCounter, const unsigned char *pIn, unsigned char *pOut,
             size_t cb)
{
    uint32_t aSt[16];
    unsigned char aBlock[64];
    size_t nOff;

    if (aKey == NULL || aNonce == NULL) {
        return -1;
    }
    if (cb == 0u) {
        return 0;
    }
    if (pIn == NULL || pOut == NULL) {
        return -1;
    }
    b42_chacha20_init_state(aSt, aKey, aNonce, uCounter);
    nOff = 0;
    while (nOff < cb) {
        size_t nTake;
        size_t i;

        b42_chacha20_block(aSt, aBlock);
        nTake = cb - nOff;
        if (nTake > 64u) {
            nTake = 64u;
        }
        for (i = 0; i < nTake; i++) {
            pOut[nOff + i] = pIn[nOff + i] ^ aBlock[i];
        }
        nOff += nTake;
        aSt[12]++; /* next block counter */
    }
    return 0;
}

/* One 64-byte keystream block (for tests / AEAD). */
int
chacha20_block(const unsigned char aKey[32], const unsigned char aNonce[12],
               uint32_t uCounter, unsigned char aOut[64])
{
    uint32_t aSt[16];

    if (aKey == NULL || aNonce == NULL || aOut == NULL) {
        return -1;
    }
    b42_chacha20_init_state(aSt, aKey, aNonce, uCounter);
    b42_chacha20_block(aSt, aOut);
    return 0;
}

int __chacha20_xor(const unsigned char aKey[32],
                   const unsigned char aNonce[12], uint32_t uCounter,
                   const unsigned char *pIn, unsigned char *pOut, size_t cb)
    __attribute__((alias("chacha20_xor")));
int __chacha20_block(const unsigned char aKey[32],
                     const unsigned char aNonce[12], uint32_t uCounter,
                     unsigned char aOut[64])
    __attribute__((alias("chacha20_block")));

/* ========================================================================
 * Poly1305 (RFC 8439 §2.5) — one-time authenticator
 * ======================================================================== */

/*
 * 130-bit field arithmetic with 5×26-bit limbs (integer only).
 * poly1305_auth: aKey is 32 bytes (r||s); writes 16-byte tag to aOut.
 */
void
poly1305_auth(const unsigned char aKey[32], const unsigned char *pMsg,
              size_t cbMsg, unsigned char aOut[16])
{
    uint32_t r0, r1, r2, r3, r4;
    uint32_t s1, s2, s3, s4;
    uint32_t h0, h1, h2, h3, h4;
    uint64_t d0, d1, d2, d3, d4;
    uint64_t c;
    const unsigned char *p;
    size_t cbLeft;

    if (aKey == NULL || aOut == NULL) {
        return;
    }
    if (pMsg == NULL && cbMsg != 0u) {
        return;
    }

    /* load r and clamp */
    {
        uint32_t t0 = b42_load_le32(aKey + 0);
        uint32_t t1 = b42_load_le32(aKey + 4);
        uint32_t t2 = b42_load_le32(aKey + 8);
        uint32_t t3 = b42_load_le32(aKey + 12);

        r0 = t0 & 0x3ffffffu;
        r1 = ((t0 >> 26) | (t1 << 6)) & 0x3ffff03u;
        r2 = ((t1 >> 20) | (t2 << 12)) & 0x3ffc0ffu;
        r3 = ((t2 >> 14) | (t3 << 18)) & 0x3f03fffu;
        r4 = (t3 >> 8) & 0x00fffffu;
    }
    s1 = r1 * 5u;
    s2 = r2 * 5u;
    s3 = r3 * 5u;
    s4 = r4 * 5u;

    h0 = h1 = h2 = h3 = h4 = 0;

    p = pMsg;
    cbLeft = cbMsg;
    while (cbLeft >= 16u) {
        uint32_t t0 = b42_load_le32(p + 0);
        uint32_t t1 = b42_load_le32(p + 4);
        uint32_t t2 = b42_load_le32(p + 8);
        uint32_t t3 = b42_load_le32(p + 12);

        h0 += t0 & 0x3ffffffu;
        h1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffffu;
        h2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffffu;
        h3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffffu;
        h4 += (t3 >> 8) | (1u << 24);

        d0 = (uint64_t)h0 * r0 + (uint64_t)h1 * s4 + (uint64_t)h2 * s3 +
             (uint64_t)h3 * s2 + (uint64_t)h4 * s1;
        d1 = (uint64_t)h0 * r1 + (uint64_t)h1 * r0 + (uint64_t)h2 * s4 +
             (uint64_t)h3 * s3 + (uint64_t)h4 * s2;
        d2 = (uint64_t)h0 * r2 + (uint64_t)h1 * r1 + (uint64_t)h2 * r0 +
             (uint64_t)h3 * s4 + (uint64_t)h4 * s3;
        d3 = (uint64_t)h0 * r3 + (uint64_t)h1 * r2 + (uint64_t)h2 * r1 +
             (uint64_t)h3 * r0 + (uint64_t)h4 * s4;
        d4 = (uint64_t)h0 * r4 + (uint64_t)h1 * r3 + (uint64_t)h2 * r2 +
             (uint64_t)h3 * r1 + (uint64_t)h4 * r0;

        c = d0 >> 26;
        h0 = (uint32_t)d0 & 0x3ffffffu;
        d1 += c;
        c = d1 >> 26;
        h1 = (uint32_t)d1 & 0x3ffffffu;
        d2 += c;
        c = d2 >> 26;
        h2 = (uint32_t)d2 & 0x3ffffffu;
        d3 += c;
        c = d3 >> 26;
        h3 = (uint32_t)d3 & 0x3ffffffu;
        d4 += c;
        c = d4 >> 26;
        h4 = (uint32_t)d4 & 0x3ffffffu;
        h0 += (uint32_t)c * 5u;
        c = h0 >> 26;
        h0 &= 0x3ffffffu;
        h1 += (uint32_t)c;

        p += 16;
        cbLeft -= 16u;
    }

    if (cbLeft > 0u) {
        unsigned char aBlock[16];
        size_t i;
        uint32_t t0, t1, t2, t3;

        memset(aBlock, 0, 16);
        for (i = 0; i < cbLeft; i++) {
            aBlock[i] = p[i];
        }
        aBlock[cbLeft] = 1u;

        t0 = b42_load_le32(aBlock + 0);
        t1 = b42_load_le32(aBlock + 4);
        t2 = b42_load_le32(aBlock + 8);
        t3 = b42_load_le32(aBlock + 12);

        h0 += t0 & 0x3ffffffu;
        h1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffffu;
        h2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffffu;
        h3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffffu;
        h4 += (t3 >> 8);

        d0 = (uint64_t)h0 * r0 + (uint64_t)h1 * s4 + (uint64_t)h2 * s3 +
             (uint64_t)h3 * s2 + (uint64_t)h4 * s1;
        d1 = (uint64_t)h0 * r1 + (uint64_t)h1 * r0 + (uint64_t)h2 * s4 +
             (uint64_t)h3 * s3 + (uint64_t)h4 * s2;
        d2 = (uint64_t)h0 * r2 + (uint64_t)h1 * r1 + (uint64_t)h2 * r0 +
             (uint64_t)h3 * s4 + (uint64_t)h4 * s3;
        d3 = (uint64_t)h0 * r3 + (uint64_t)h1 * r2 + (uint64_t)h2 * r1 +
             (uint64_t)h3 * r0 + (uint64_t)h4 * s4;
        d4 = (uint64_t)h0 * r4 + (uint64_t)h1 * r3 + (uint64_t)h2 * r2 +
             (uint64_t)h3 * r1 + (uint64_t)h4 * r0;

        c = d0 >> 26;
        h0 = (uint32_t)d0 & 0x3ffffffu;
        d1 += c;
        c = d1 >> 26;
        h1 = (uint32_t)d1 & 0x3ffffffu;
        d2 += c;
        c = d2 >> 26;
        h2 = (uint32_t)d2 & 0x3ffffffu;
        d3 += c;
        c = d3 >> 26;
        h3 = (uint32_t)d3 & 0x3ffffffu;
        d4 += c;
        c = d4 >> 26;
        h4 = (uint32_t)d4 & 0x3ffffffu;
        h0 += (uint32_t)c * 5u;
        c = h0 >> 26;
        h0 &= 0x3ffffffu;
        h1 += (uint32_t)c;
    }

    /* final reduction */
    c = h1 >> 26;
    h1 &= 0x3ffffffu;
    h2 += (uint32_t)c;
    c = h2 >> 26;
    h2 &= 0x3ffffffu;
    h3 += (uint32_t)c;
    c = h3 >> 26;
    h3 &= 0x3ffffffu;
    h4 += (uint32_t)c;
    c = h4 >> 26;
    h4 &= 0x3ffffffu;
    h0 += (uint32_t)c * 5u;
    c = h0 >> 26;
    h0 &= 0x3ffffffu;
    h1 += (uint32_t)c;

    /* h = h < p ? h : h - p  (p = 2^130 - 5) */
    {
        uint32_t g0, g1, g2, g3, g4;
        uint32_t mask;

        g0 = h0 + 5u;
        c = g0 >> 26;
        g0 &= 0x3ffffffu;
        g1 = h1 + (uint32_t)c;
        c = g1 >> 26;
        g1 &= 0x3ffffffu;
        g2 = h2 + (uint32_t)c;
        c = g2 >> 26;
        g2 &= 0x3ffffffu;
        g3 = h3 + (uint32_t)c;
        c = g3 >> 26;
        g3 &= 0x3ffffffu;
        g4 = h4 + (uint32_t)c - (1u << 26);

        mask = (g4 >> 31) - 1u;
        g0 &= mask;
        g1 &= mask;
        g2 &= mask;
        g3 &= mask;
        g4 &= mask;
        mask = ~mask;
        h0 = (h0 & mask) | g0;
        h1 = (h1 & mask) | g1;
        h2 = (h2 & mask) | g2;
        h3 = (h3 & mask) | g3;
        h4 = (h4 & mask) | g4;
    }

    /* pack to 128-bit LE + add s */
    {
        uint64_t f0, f1, f2, f3;
        uint32_t s0 = b42_load_le32(aKey + 16);
        uint32_t s1x = b42_load_le32(aKey + 20);
        uint32_t s2x = b42_load_le32(aKey + 24);
        uint32_t s3x = b42_load_le32(aKey + 28);

        f0 = ((uint64_t)h0) | ((uint64_t)h1 << 26);
        f1 = ((uint64_t)(h1 >> 6)) | ((uint64_t)h2 << 20);
        f2 = ((uint64_t)(h2 >> 12)) | ((uint64_t)h3 << 14);
        f3 = ((uint64_t)(h3 >> 18)) | ((uint64_t)h4 << 8);

        f0 = (f0 & 0xffffffffull) + s0;
        f1 = (f1 & 0xffffffffull) + s1x + (f0 >> 32);
        f2 = (f2 & 0xffffffffull) + s2x + (f1 >> 32);
        f3 = (f3 & 0xffffffffull) + s3x + (f2 >> 32);

        b42_store_le32(aOut + 0, (uint32_t)f0);
        b42_store_le32(aOut + 4, (uint32_t)f1);
        b42_store_le32(aOut + 8, (uint32_t)f2);
        b42_store_le32(aOut + 12, (uint32_t)f3);
    }
}

void __poly1305_auth(const unsigned char aKey[32], const unsigned char *pMsg,
                     size_t cbMsg, unsigned char aOut[16])
    __attribute__((alias("poly1305_auth")));

/* ========================================================================
 * CRC-8/SMBUS (poly 0x07) and CRC-16/MODBUS (poly 0xA001 reflected)
 * ======================================================================== */

uint8_t
crc8_update(uint8_t uCrc, const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    size_t i;

    if (p == NULL && cb != 0u) {
        return uCrc;
    }
    for (i = 0; i < cb; i++) {
        unsigned b;
        uCrc ^= p[i];
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x80u) {
                uCrc = (uint8_t)((uCrc << 1) ^ 0x07u);
            } else {
                uCrc = (uint8_t)(uCrc << 1);
            }
        }
    }
    return uCrc;
}

uint8_t
crc8(const void *pData, size_t cb)
{
    return crc8_update(0u, pData, cb);
}

/* CRC-16/MODBUS: init 0xFFFF, poly 0xA001, xorout 0 */
uint16_t
crc16_modbus_update(uint16_t uCrc, const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    size_t i;

    if (p == NULL && cb != 0u) {
        return uCrc;
    }
    for (i = 0; i < cb; i++) {
        unsigned b;
        uCrc ^= (uint16_t)p[i];
        for (b = 0; b < 8u; b++) {
            if (uCrc & 1u) {
                uCrc = (uint16_t)((uCrc >> 1) ^ 0xA001u);
            } else {
                uCrc = (uint16_t)(uCrc >> 1);
            }
        }
    }
    return uCrc;
}

uint16_t
crc16_modbus(const void *pData, size_t cb)
{
    return crc16_modbus_update(0xFFFFu, pData, cb);
}

uint8_t __crc8(const void *p, size_t n) __attribute__((alias("crc8")));
uint8_t __crc8_update(uint8_t c, const void *p, size_t n)
    __attribute__((alias("crc8_update")));
uint16_t __crc16_modbus(const void *p, size_t n)
    __attribute__((alias("crc16_modbus")));
uint16_t __crc16_modbus_update(uint16_t c, const void *p, size_t n)
    __attribute__((alias("crc16_modbus_update")));

/* ========================================================================
 * Base58 (Bitcoin alphabet) encode / decode
 * ======================================================================== */

static const char s_b42_b58_alpha[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

static int
b42_b58_val(int ch)
{
    const char *p = s_b42_b58_alpha;
    int i;

    for (i = 0; p[i] != '\0'; i++) {
        if (p[i] == ch) {
            return i;
        }
    }
    return -1;
}

/*
 * base58_encode: returns encoded length (no NUL counted) or -1.
 * Writes NUL-terminated string when cbOut allows. Max input ~370 bytes.
 */
ssize_t
base58_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
    unsigned char aBuf[512];
    size_t cZeros = 0;
    size_t i;
    size_t cbSize;
    size_t cbHigh;
    size_t cbLen;
    char *pDst;

    if (pIn == NULL && cbIn != 0u) {
        return -1;
    }
    if (szOut == NULL || cbOut == 0u) {
        return -1;
    }

    while (cZeros < cbIn && pIn[cZeros] == 0u) {
        cZeros++;
    }

    /* log(256)/log(58) ≈ 1.37 → size = cbIn*138/100 + 1 */
    cbSize = cbIn * 138u / 100u + 1u;
    if (cbSize > sizeof(aBuf)) {
        return -1;
    }
    memset(aBuf, 0, cbSize);

    for (i = cZeros; i < cbIn; i++) {
        unsigned carry = pIn[i];
        size_t j;

        for (j = cbSize; j > 0u; j--) {
            carry += 256u * (unsigned)aBuf[j - 1u];
            aBuf[j - 1u] = (unsigned char)(carry % 58u);
            carry /= 58u;
        }
    }

    cbHigh = 0;
    while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
        cbHigh++;
    }

    cbLen = cZeros + (cbSize - cbHigh);
    if (cbLen + 1u > cbOut) {
        return -1;
    }

    pDst = szOut;
    for (i = 0; i < cZeros; i++) {
        *pDst++ = '1';
    }
    for (i = cbHigh; i < cbSize; i++) {
        *pDst++ = s_b42_b58_alpha[aBuf[i]];
    }
    *pDst = '\0';
    return (ssize_t)(pDst - szOut);
}

ssize_t
base58_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
    unsigned char aBuf[512];
    size_t cZeros = 0;
    size_t i;
    size_t cbSize;
    size_t cbHigh;
    size_t cbLen;

    if (szIn == NULL && cbIn != 0u) {
        return -1;
    }
    if (pOut == NULL) {
        return -1;
    }
    if (cbIn == 0u) {
        return 0;
    }

    while (cZeros < cbIn && szIn[cZeros] == '1') {
        cZeros++;
    }

    cbSize = cbIn * 733u / 1000u + 1u; /* log(58)/log(256) */
    if (cbSize > sizeof(aBuf)) {
        return -1;
    }
    memset(aBuf, 0, cbSize);

    for (i = cZeros; i < cbIn; i++) {
        int nVal = b42_b58_val((unsigned char)szIn[i]);
        unsigned carry;
        size_t j;

        if (nVal < 0) {
            return -1;
        }
        carry = (unsigned)nVal;
        for (j = cbSize; j > 0u; j--) {
            carry += 58u * (unsigned)aBuf[j - 1u];
            aBuf[j - 1u] = (unsigned char)(carry & 0xffu);
            carry >>= 8;
        }
        if (carry != 0u) {
            return -1;
        }
    }

    cbHigh = 0;
    while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
        cbHigh++;
    }
    cbLen = cZeros + (cbSize - cbHigh);
    if (cbLen > cbOut) {
        return -1;
    }
    memset(pOut, 0, cZeros);
    memcpy(pOut + cZeros, aBuf + cbHigh, cbSize - cbHigh);
    return (ssize_t)cbLen;
}

ssize_t __base58_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base58_encode")));
ssize_t __base58_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base58_decode")));

/* ========================================================================
 * Base32hex (RFC 4648 §7) encode / decode
 * ======================================================================== */

static const char s_b42_b32hex[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

static int
b42_b32hex_val(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'V') {
        return ch - 'A' + 10;
    }
    if (ch >= 'a' && ch <= 'v') {
        return ch - 'a' + 10;
    }
    return -1;
}

ssize_t
base32hex_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                 size_t cbOut)
{
    size_t i = 0;
    size_t nOut = 0;
    unsigned uBuf = 0;
    int nBits = 0;

    if (pIn == NULL && cbIn != 0u) {
        return -1;
    }
    if (szOut == NULL) {
        return -1;
    }

    while (i < cbIn) {
        uBuf = (uBuf << 8) | pIn[i++];
        nBits += 8;
        while (nBits >= 5) {
            nBits -= 5;
            if (nOut + 1u >= cbOut) {
                return -1;
            }
            szOut[nOut++] = s_b42_b32hex[(uBuf >> nBits) & 0x1fu];
        }
    }
    if (nBits > 0) {
        if (nOut + 1u >= cbOut) {
            return -1;
        }
        szOut[nOut++] = s_b42_b32hex[(uBuf << (5 - nBits)) & 0x1fu];
    }
    /* pad to multiple of 8 */
    while ((nOut % 8u) != 0u) {
        if (nOut + 1u >= cbOut) {
            return -1;
        }
        szOut[nOut++] = '=';
    }
    if (nOut >= cbOut) {
        return -1;
    }
    szOut[nOut] = '\0';
    return (ssize_t)nOut;
}

ssize_t
base32hex_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
                 size_t cbOut)
{
    size_t i;
    size_t nOut = 0;
    unsigned uBuf = 0;
    int nBits = 0;

    if (szIn == NULL && cbIn != 0u) {
        return -1;
    }
    if (pOut == NULL) {
        return -1;
    }

    for (i = 0; i < cbIn; i++) {
        int nVal;
        int ch = (unsigned char)szIn[i];

        if (ch == '=' || ch == '\0') {
            break;
        }
        if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
            continue;
        }
        nVal = b42_b32hex_val(ch);
        if (nVal < 0) {
            return -1;
        }
        uBuf = (uBuf << 5) | (unsigned)nVal;
        nBits += 5;
        if (nBits >= 8) {
            nBits -= 8;
            if (nOut >= cbOut) {
                return -1;
            }
            pOut[nOut++] = (unsigned char)((uBuf >> nBits) & 0xffu);
        }
    }
    return (ssize_t)nOut;
}

ssize_t __base32hex_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base32hex_encode")));
ssize_t __base32hex_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base32hex_decode")));

/* ========================================================================
 * HMAC-SHA3-512 (block = rate = 72)
 * ======================================================================== */

void
hmac_sha3_512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
              unsigned char aOut[64])
{
    const size_t cbBlock = 72u;
    unsigned char aK[72];
    unsigned char aIpad[72];
    unsigned char aOpad[72];
    unsigned char aInner[64];
    unsigned char aCtx[512];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aK, 0, sizeof(aK));
    if (cbKey > cbBlock) {
        SHA3_512_Init(aCtx);
        SHA3_512_Update(aCtx, pKey, cbKey);
        SHA3_512_Final(aK, aCtx);
    } else if (pKey != NULL && cbKey > 0u) {
        memcpy(aK, pKey, cbKey);
    }
    for (i = 0; i < cbBlock; i++) {
        aIpad[i] = (unsigned char)(aK[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aK[i] ^ 0x5cu);
    }
    SHA3_512_Init(aCtx);
    SHA3_512_Update(aCtx, aIpad, cbBlock);
    if (pData != NULL && cbData > 0u) {
        SHA3_512_Update(aCtx, pData, cbData);
    }
    SHA3_512_Final(aInner, aCtx);
    SHA3_512_Init(aCtx);
    SHA3_512_Update(aCtx, aOpad, cbBlock);
    SHA3_512_Update(aCtx, aInner, 64u);
    SHA3_512_Final(aOut, aCtx);
}

void __hmac_sha3_512(const void *k, size_t nk, const void *d, size_t nd,
                     unsigned char o[64])
    __attribute__((alias("hmac_sha3_512")));

/* ========================================================================
 * djb2 / sdbm classic string hashes (desktop graph surface)
 * ======================================================================== */

uint32_t
djb2_hash(const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    uint32_t uHash = 5381u;
    size_t i;

    if (p == NULL && cb != 0u) {
        return 0u;
    }
    for (i = 0; i < cb; i++) {
        uHash = ((uHash << 5) + uHash) + (uint32_t)p[i]; /* hash * 33 + c */
    }
    return uHash;
}

uint32_t
sdbm_hash(const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    uint32_t uHash = 0u;
    size_t i;

    if (p == NULL && cb != 0u) {
        return 0u;
    }
    for (i = 0; i < cb; i++) {
        uHash = (uint32_t)p[i] + (uHash << 6) + (uHash << 16) - uHash;
    }
    return uHash;
}

uint32_t __djb2_hash(const void *p, size_t n)
    __attribute__((alias("djb2_hash")));
uint32_t __sdbm_hash(const void *p, size_t n)
    __attribute__((alias("sdbm_hash")));
