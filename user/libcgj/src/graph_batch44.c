/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44: MD2 (RFC 1319) + SM3 lowercase API
 * (GM/T 0004-2012 / ISO/IEC 10118-3). Integer/pointer only (no SSE).
 * Clean-room public ABI.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const char __libcgj_batch44_marker[] = "libcgj-batch44";

/* ---- shared helpers ----------------------------------------------------- */

static uint32_t
b44_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint32_t
b44_load_be32(const unsigned char *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void
b44_store_be32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)(x >> 24);
    p[1] = (unsigned char)(x >> 16);
    p[2] = (unsigned char)(x >> 8);
    p[3] = (unsigned char)x;
}

static char *
b44_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
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

/* ========================================================================
 * MD2 (RFC 1319) — 128-bit digest, 16-byte blocks
 * ======================================================================== */

/* RFC 1319 PI_SUBST S-box */
static const unsigned char s_b44_md2_s[256] = {
    0x29, 0x2e, 0x43, 0xc9, 0xa2, 0xd8, 0x7c, 0x01, 0x3d, 0x36, 0x54, 0xa1,
    0xec, 0xf0, 0x06, 0x13, 0x62, 0xa7, 0x05, 0xf3, 0xc0, 0xc7, 0x73, 0x8c,
    0x98, 0x93, 0x2b, 0xd9, 0xbc, 0x4c, 0x82, 0xca, 0x1e, 0x9b, 0x57, 0x3c,
    0xfd, 0xd4, 0xe0, 0x16, 0x67, 0x42, 0x6f, 0x18, 0x8a, 0x17, 0xe5, 0x12,
    0xbe, 0x4e, 0xc4, 0xd6, 0xda, 0x9e, 0xde, 0x49, 0xa0, 0xfb, 0xf5, 0x8e,
    0xbb, 0x2f, 0xee, 0x7a, 0xa9, 0x68, 0x79, 0x91, 0x15, 0xb2, 0x07, 0x3f,
    0x94, 0xc2, 0x10, 0x89, 0x0b, 0x22, 0x5f, 0x21, 0x80, 0x7f, 0x5d, 0x9a,
    0x5a, 0x90, 0x32, 0x27, 0x35, 0x3e, 0xcc, 0xe7, 0xbf, 0xf7, 0x97, 0x03,
    0xff, 0x19, 0x30, 0xb3, 0x48, 0xa5, 0xb5, 0xd1, 0xd7, 0x5e, 0x92, 0x2a,
    0xac, 0x56, 0xaa, 0xc6, 0x4f, 0xb8, 0x38, 0xd2, 0x96, 0xa4, 0x7d, 0xb6,
    0x76, 0xfc, 0x6b, 0xe2, 0x9c, 0x74, 0x04, 0xf1, 0x45, 0x9d, 0x70, 0x59,
    0x64, 0x71, 0x87, 0x20, 0x86, 0x5b, 0xcf, 0x65, 0xe6, 0x2d, 0xa8, 0x02,
    0x1b, 0x60, 0x25, 0xad, 0xae, 0xb0, 0xb9, 0xf6, 0x1c, 0x46, 0x61, 0x69,
    0x34, 0x40, 0x7e, 0x0f, 0x55, 0x47, 0xa3, 0x23, 0xdd, 0x51, 0xaf, 0x3a,
    0xc3, 0x5c, 0xf9, 0xce, 0xba, 0xc5, 0xea, 0x26, 0x2c, 0x53, 0x0d, 0x6e,
    0x85, 0x28, 0x84, 0x09, 0xd3, 0xdf, 0xcd, 0xf4, 0x41, 0x81, 0x4d, 0x52,
    0x6a, 0xdc, 0x37, 0xc8, 0x6c, 0xc1, 0xab, 0xfa, 0x24, 0xe1, 0x7b, 0x08,
    0x0c, 0xbd, 0xb1, 0x4a, 0x78, 0x88, 0x95, 0x8b, 0xe3, 0x63, 0xe8, 0x6d,
    0xe9, 0xcb, 0xd5, 0xfe, 0x3b, 0x00, 0x1d, 0x39, 0xf2, 0xef, 0xb7, 0x0e,
    0x66, 0x58, 0xd0, 0xe4, 0xa6, 0x77, 0x72, 0xf8, 0xeb, 0x75, 0x4b, 0x0a,
    0x31, 0x44, 0x50, 0xb4, 0x8f, 0xed, 0x1f, 0x1a, 0xdb, 0x99, 0x8d, 0x33,
    0x9f, 0x11, 0x83, 0x14
};

struct b44_md2 {
    unsigned char aX[48];
    unsigned char aC[16];
    unsigned char aBuf[16];
    unsigned char nL;
    size_t cbBuf;
};

static void
b44_md2_process(struct b44_md2 *p, const unsigned char aBlk[16], int nDoCksum)
{
    unsigned char t;
    unsigned j, k;

    if (nDoCksum) {
        for (j = 0; j < 16u; j++) {
            unsigned char c = aBlk[j];
            p->aC[j] ^= s_b44_md2_s[c ^ p->nL];
            p->nL = p->aC[j];
        }
    }

    for (j = 0; j < 16u; j++) {
        p->aX[16u + j] = aBlk[j];
        p->aX[32u + j] = (unsigned char)(p->aX[16u + j] ^ p->aX[j]);
    }

    t = 0;
    for (j = 0; j < 18u; j++) {
        for (k = 0; k < 48u; k++) {
            t = (unsigned char)(p->aX[k] ^ s_b44_md2_s[t]);
            p->aX[k] = t;
        }
        t = (unsigned char)(t + (unsigned char)j);
    }
}

void
MD2Init(void *pCtx)
{
    struct b44_md2 *p = (struct b44_md2 *)pCtx;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
}

void
MD2Update(void *pCtx, const void *pData, size_t cb)
{
    struct b44_md2 *p = (struct b44_md2 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0u)) {
        return;
    }
    while (cb > 0u) {
        size_t nLeft = 16u - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;

        memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == 16u) {
            b44_md2_process(p, p->aBuf, 1);
            p->cbBuf = 0;
        }
    }
}

void
MD2Final(unsigned char aDig[16], void *pCtx)
{
    struct b44_md2 *p = (struct b44_md2 *)pCtx;
    unsigned char nPad;
    size_t i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    /* Pad with i bytes of value i (i = 16 - len mod 16; always 1..16). */
    nPad = (unsigned char)(16u - p->cbBuf);
    if (nPad == 0u) {
        nPad = 16u;
    }
    {
        unsigned char aPad[16];

        for (i = 0; i < (size_t)nPad; i++) {
            aPad[i] = nPad;
        }
        MD2Update(p, aPad, (size_t)nPad);
    }
    /* Append checksum block (checksum not updated for this block). */
    b44_md2_process(p, p->aC, 0);
    memcpy(aDig, p->aX, 16u);
    memset(p, 0, sizeof(*p));
}

char *
MD2Data(const void *pData, size_t cb, char *szBuf)
{
    unsigned char aCtx[sizeof(struct b44_md2) + 8u];
    unsigned char aDig[16];

    memset(aCtx, 0, sizeof(aCtx));
    MD2Init(aCtx);
    MD2Update(aCtx, pData, cb);
    MD2Final(aDig, aCtx);
    return b44_hex_end(aDig, 16u, szBuf);
}

void __MD2Init(void *p) __attribute__((alias("MD2Init")));
void __MD2Update(void *p, const void *d, size_t n)
    __attribute__((alias("MD2Update")));
void __MD2Final(unsigned char a[16], void *p)
    __attribute__((alias("MD2Final")));
char *__MD2Data(const void *d, size_t n, char *s)
    __attribute__((alias("MD2Data")));

/* ========================================================================
 * SM3 (GM/T 0004-2012) — lowercase API, 256-bit Chinese national hash
 * ======================================================================== */

struct b44_sm3 {
    uint32_t aH[8];
    unsigned char aBuf[64];
    size_t cbBuf;
    uint64_t cbTotal;
};

static uint32_t
b44_sm3_p0(uint32_t x)
{
    return x ^ b44_rol32(x, 9) ^ b44_rol32(x, 17);
}

static uint32_t
b44_sm3_p1(uint32_t x)
{
    return x ^ b44_rol32(x, 15) ^ b44_rol32(x, 23);
}

static uint32_t
b44_sm3_ff(uint32_t x, uint32_t y, uint32_t z, unsigned j)
{
    if (j < 16u) {
        return x ^ y ^ z;
    }
    return (x & y) | (x & z) | (y & z);
}

static uint32_t
b44_sm3_gg(uint32_t x, uint32_t y, uint32_t z, unsigned j)
{
    if (j < 16u) {
        return x ^ y ^ z;
    }
    return (x & y) | ((~x) & z);
}

static uint32_t
b44_sm3_t(unsigned j)
{
    return (j < 16u) ? 0x79cc4519u : 0x7a879d8au;
}

static void
b44_sm3_compress(struct b44_sm3 *p, const unsigned char aBlock[64])
{
    uint32_t w[68];
    uint32_t w1[64];
    uint32_t a, b, c, d, e, f, g, h;
    unsigned j;

    for (j = 0; j < 16u; j++) {
        w[j] = b44_load_be32(aBlock + j * 4u);
    }
    for (j = 16; j < 68u; j++) {
        w[j] = b44_sm3_p1(w[j - 16u] ^ w[j - 9u] ^ b44_rol32(w[j - 3u], 15)) ^
               b44_rol32(w[j - 13u], 7) ^ w[j - 6u];
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
            b44_rol32(b44_rol32(a, 12) + e + b44_rol32(b44_sm3_t(j), j), 7);
        uint32_t ss2 = ss1 ^ b44_rol32(a, 12);
        uint32_t tt1 = b44_sm3_ff(a, b, c, j) + d + ss2 + w1[j];
        uint32_t tt2 = b44_sm3_gg(e, f, g, j) + h + ss1 + w[j];

        d = c;
        c = b44_rol32(b, 9);
        b = a;
        a = tt1;
        h = g;
        g = b44_rol32(f, 19);
        f = e;
        e = b44_sm3_p0(tt2);
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
sm3_init(void *pCtx)
{
    struct b44_sm3 *p = (struct b44_sm3 *)pCtx;

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
sm3_update(void *pCtx, const void *pData, size_t cb)
{
    struct b44_sm3 *p = (struct b44_sm3 *)pCtx;
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
            b44_sm3_compress(p, p->aBuf);
            p->cbBuf = 0;
        }
    }
}

void
sm3_final(unsigned char aDig[32], void *pCtx)
{
    struct b44_sm3 *p = (struct b44_sm3 *)pCtx;
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
        b44_sm3_compress(p, p->aBuf);
        p->cbBuf = 0;
    }
    while (p->cbBuf < 56u) {
        p->aBuf[p->cbBuf++] = 0;
    }
    /* 64-bit length big-endian */
    for (i = 0; i < 8u; i++) {
        p->aBuf[56u + i] = (unsigned char)(cbBits >> (56u - 8u * i));
    }
    b44_sm3_compress(p, p->aBuf);
    for (i = 0; i < 8u; i++) {
        b44_store_be32(aDig + i * 4u, p->aH[i]);
    }
    memset(p, 0, sizeof(*p));
}

void
sm3_hash(const void *pData, size_t cb, unsigned char aDig[32])
{
    unsigned char aCtx[sizeof(struct b44_sm3) + 16u];

    if (aDig == NULL) {
        return;
    }
    sm3_init(aCtx);
    sm3_update(aCtx, pData, cb);
    sm3_final(aDig, aCtx);
}

void __sm3_init(void *p) __attribute__((alias("sm3_init")));
void __sm3_update(void *p, const void *d, size_t n)
    __attribute__((alias("sm3_update")));
void __sm3_final(unsigned char a[32], void *p)
    __attribute__((alias("sm3_final")));
void __sm3_hash(const void *d, size_t n, unsigned char a[32])
    __attribute__((alias("sm3_hash")));

/* ---- soft deepen: SM3_End hex + ctx size (unique; SM3_Data in batch42) -- */

char *
SM3_End(void *pCtx, char *szBuf)
{
    unsigned char aDig[32];

    sm3_final(aDig, pCtx);
    return b44_hex_end(aDig, 32u, szBuf);
}

char *__SM3_End(void *pCtx, char *szBuf) __attribute__((alias("SM3_End")));

size_t
sm3_ctx_size(void)
{
    return sizeof(struct b44_sm3);
}

size_t __sm3_ctx_size(void) __attribute__((alias("sm3_ctx_size")));

size_t
MD2_size(void)
{
    return sizeof(struct b44_md2);
}

size_t __MD2_size(void) __attribute__((alias("MD2_size")));

/* Hex one-shot already as MD2Data; binary one-shot unique name. */
int
MD2_digest(const void *pData, size_t cb, unsigned char aDig[16])
{
    unsigned char aCtx[sizeof(struct b44_md2) + 8u];

    if (aDig == NULL) {
        return -1;
    }
    memset(aCtx, 0, sizeof(aCtx));
    MD2Init(aCtx);
    MD2Update(aCtx, pData, cb);
    MD2Final(aDig, aCtx);
    return 0;
}

int __MD2_digest(const void *pData, size_t cb, unsigned char aDig[16])
    __attribute__((alias("MD2_digest")));

