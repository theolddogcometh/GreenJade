/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch54: OpenSSL-shaped RIPEMD-160 (160-bit digest).
 * Integer/pointer only (no SSE). Clean-room from public RIPEMD-160 / ISO
 * 10118-3 algorithm description (Dobbertin, Bosselaers, Preneel).
 *
 * Empty-string digest (known value):
 *   RIPEMD160("") =
 *     9c1185a5c5e9fc54612808977ee8f548b2258d31
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch54_marker[] = "libcgj-batch54";

/*
 * Opaque layout for a caller-supplied char ctx[128] (or larger) buffer.
 * Fixed private structure; size is well under 128 bytes.
 */
typedef struct {
    uint32_t h[5];
    uint32_t nl; /* bit length low */
    uint32_t nh; /* bit length high */
    unsigned char data[64];
    unsigned int num; /* bytes pending in data[] */
} RIPEMD160_CTX;

/* Static assert-ish: layout must fit the documented 128-byte opaque buffer. */
typedef char b54_ctx_fits_128[(sizeof(RIPEMD160_CTX) <= 128u) ? 1 : -1];

static void
b54_memzero(void *p, size_t n)
{
    unsigned char *q = (unsigned char *)p;
    size_t i;

    for (i = 0; i < n; i++) {
        q[i] = 0;
    }
}

static void
b54_memcpy(void *pDst, const void *pSrc, size_t n)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

static uint32_t
b54_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint32_t
b54_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b54_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

/* Boolean functions for the five rounds (left line order). */
static uint32_t
b54_f(unsigned r, uint32_t x, uint32_t y, uint32_t z)
{
    if (r == 0u) {
        return x ^ y ^ z;
    }
    if (r == 1u) {
        return (x & y) | ((~x) & z);
    }
    if (r == 2u) {
        return (x | (~y)) ^ z;
    }
    if (r == 3u) {
        return (x & z) | (y & (~z));
    }
    return x ^ (y | (~z));
}

static void
b54_compress(RIPEMD160_CTX *p, const unsigned char *pBlk)
{
    /* Message word selection (left / right) and rotate amounts — public tables. */
    static const unsigned char aR[80] = {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
        7,  4,  13, 1,  10, 6,  15, 3,  12, 0,  9,  5,  2,  14, 11, 8,
        3,  10, 14, 4,  9,  15, 8,  1,  2,  7,  0,  6,  13, 11, 5,  12,
        1,  9,  11, 10, 0,  8,  12, 4,  13, 3,  7,  15, 14, 5,  6,  2,
        4,  0,  5,  9,  7,  12, 2,  10, 14, 1,  3,  8,  11, 6,  15, 13};
    static const unsigned char aRp[80] = {
        5,  14, 7,  0,  9,  2,  11, 4,  13, 6,  15, 8,  1,  10, 3,  12,
        6,  11, 3,  7,  0,  13, 5,  10, 14, 15, 8,  12, 4,  9,  1,  2,
        15, 5,  1,  3,  7,  14, 6,  9,  11, 8,  12, 2,  10, 0,  4,  13,
        8,  6,  4,  1,  3,  11, 15, 0,  5,  12, 2,  13, 9,  7,  10, 14,
        12, 15, 10, 4,  1,  5,  8,  7,  6,  2,  13, 14, 0,  3,  9,  11};
    static const unsigned char aS[80] = {
        11, 14, 15, 12, 5,  8,  7,  9,  11, 13, 14, 15, 6,  7,  9,  8,
        7,  6,  8,  13, 11, 9,  7,  15, 7,  12, 15, 9,  11, 7,  13, 12,
        11, 13, 6,  7,  14, 9,  13, 15, 14, 8,  13, 6,  5,  12, 7,  5,
        11, 12, 14, 15, 14, 15, 9,  8,  9,  14, 5,  6,  8,  6,  5,  12,
        9,  15, 5,  11, 6,  8,  13, 12, 5,  12, 13, 14, 11, 8,  5,  6};
    static const unsigned char aSp[80] = {
        8,  9,  9,  11, 13, 15, 15, 5,  7,  7,  8,  11, 14, 14, 12, 6,
        9,  13, 15, 7,  12, 8,  9,  11, 7,  7,  12, 7,  6,  15, 13, 11,
        9,  7,  15, 11, 8,  6,  6,  14, 12, 13, 5,  14, 13, 13, 7,  5,
        15, 5,  8,  11, 14, 14, 6,  14, 6,  9,  12, 9,  12, 5,  15, 8,
        8,  5,  12, 9,  12, 5,  14, 6,  8,  13, 6,  5,  15, 13, 11, 11};
    static const uint32_t aK[5] = {0x00000000u, 0x5a827999u, 0x6ed9eba1u,
                                   0x8f1bbcdcu, 0xa953fd4eu};
    static const uint32_t aKp[5] = {0x50a28be6u, 0x5c4dd124u, 0x6d703ef3u,
                                    0x7a6d76e9u, 0x00000000u};

    uint32_t x[16];
    uint32_t al, bl, cl, dl, el;
    uint32_t ar, br, cr, dr, er;
    unsigned i;

    for (i = 0; i < 16u; i++) {
        x[i] = b54_load_le32(pBlk + i * 4u);
    }

    al = ar = p->h[0];
    bl = br = p->h[1];
    cl = cr = p->h[2];
    dl = dr = p->h[3];
    el = er = p->h[4];

    for (i = 0; i < 80u; i++) {
        unsigned r = i / 16u;
        uint32_t f, t;

        /* Left line: rounds use f0..f4 in order. */
        f = b54_f(r, bl, cl, dl);
        t = b54_rol32(al + f + x[aR[i]] + aK[r], aS[i]) + el;
        al = el;
        el = dl;
        dl = b54_rol32(cl, 10u);
        cl = bl;
        bl = t;

        /* Right line: rounds use f4..f0 (reverse order of left). */
        f = b54_f(4u - r, br, cr, dr);
        t = b54_rol32(ar + f + x[aRp[i]] + aKp[r], aSp[i]) + er;
        ar = er;
        er = dr;
        dr = b54_rol32(cr, 10u);
        cr = br;
        br = t;
    }

    {
        uint32_t t = p->h[1] + cl + dr;

        p->h[1] = p->h[2] + dl + er;
        p->h[2] = p->h[3] + el + ar;
        p->h[3] = p->h[4] + al + br;
        p->h[4] = p->h[0] + bl + cr;
        p->h[0] = t;
    }
}

void
RIPEMD160_Init(void *ctx)
{
    RIPEMD160_CTX *p = (RIPEMD160_CTX *)ctx;

    if (p == NULL) {
        return;
    }
    b54_memzero(p, sizeof(*p));
    p->h[0] = 0x67452301u;
    p->h[1] = 0xefcdab89u;
    p->h[2] = 0x98badcfeu;
    p->h[3] = 0x10325476u;
    p->h[4] = 0xc3d2e1f0u;
}

void
RIPEMD160_Update(void *ctx, const void *data, size_t len)
{
    RIPEMD160_CTX *p = (RIPEMD160_CTX *)ctx;
    const unsigned char *q = (const unsigned char *)data;
    unsigned int nIndex;
    size_t nPart;
    uint32_t nBitsLow;

    if (p == NULL || (data == NULL && len != 0u)) {
        return;
    }

    nIndex = p->num;
    nBitsLow = (uint32_t)(len << 3);
    p->nl += nBitsLow;
    if (p->nl < nBitsLow) {
        p->nh++;
    }
    p->nh += (uint32_t)(len >> 29);

    nPart = 64u - (size_t)nIndex;
    if (len >= nPart) {
        b54_memcpy(p->data + nIndex, q, nPart);
        b54_compress(p, p->data);
        q += nPart;
        len -= nPart;
        while (len >= 64u) {
            b54_compress(p, q);
            q += 64u;
            len -= 64u;
        }
        nIndex = 0;
    }
    if (len > 0u) {
        b54_memcpy(p->data + nIndex, q, len);
        nIndex += (unsigned int)len;
    }
    p->num = nIndex;
}

void
RIPEMD160_Final(unsigned char md[20], void *ctx)
{
    RIPEMD160_CTX *p = (RIPEMD160_CTX *)ctx;
    unsigned char aBits[8];
    unsigned char aPad[64];
    unsigned int nIndex, nPad;
    unsigned i;

    if (p == NULL || md == NULL) {
        return;
    }

    for (i = 0; i < 4u; i++) {
        aBits[i] = (unsigned char)(p->nl >> (i * 8u));
        aBits[i + 4u] = (unsigned char)(p->nh >> (i * 8u));
    }

    nIndex = p->num;
    nPad = (nIndex < 56u) ? (56u - nIndex) : (120u - nIndex);
    b54_memzero(aPad, sizeof(aPad));
    aPad[0] = 0x80u;
    RIPEMD160_Update(p, aPad, (size_t)nPad);
    RIPEMD160_Update(p, aBits, 8u);

    for (i = 0; i < 5u; i++) {
        b54_store_le32(md + i * 4u, p->h[i]);
    }
    b54_memzero(p, sizeof(*p));
}

unsigned char *
RIPEMD160(const unsigned char *d, size_t n, unsigned char md[20])
{
    unsigned char aCtx[128];

    if (md == NULL) {
        return NULL;
    }
    if (d == NULL && n != 0u) {
        return NULL;
    }
    RIPEMD160_Init(aCtx);
    RIPEMD160_Update(aCtx, d, n);
    RIPEMD160_Final(md, aCtx);
    return md;
}

void __RIPEMD160_Init(void *ctx) __attribute__((alias("RIPEMD160_Init")));
void __RIPEMD160_Update(void *ctx, const void *data, size_t len)
    __attribute__((alias("RIPEMD160_Update")));
void __RIPEMD160_Final(unsigned char md[20], void *ctx)
    __attribute__((alias("RIPEMD160_Final")));
unsigned char *__RIPEMD160(const unsigned char *d, size_t n,
                           unsigned char md[20])
    __attribute__((alias("RIPEMD160")));
