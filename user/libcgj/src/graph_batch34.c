/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34: MD4 + RIPEMD-160 + BLAKE2s digests (libmd
 * shape), remaining BSD vis family (nvis/svis/strsvis/strunvisx), bcrypt
 * ENOSYS stubs, BSD strtofflags/fflagstostr, TIRPC setnetconfig family
 * stubs, copy_file_range64 alias.
 * Integer/pointer only (no SSE doubles). Clean-room public ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t copy_file_range(int nIn, off_t *pOffIn, int nOut, off_t *pOffOut,
                        size_t cb, unsigned uFlags);
int strnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags);
int stravis(char **ppDst, const char *szSrc, int nFlags);
int strnunvis(char *pDst, size_t cbDst, const char *szSrc);
int strunvis(char *pDst, const char *szSrc);
int strnvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc,
             int nFlags);
int strnunvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc);

const char __libcgj_batch34_marker[] = "libcgj-batch34";

/* ---- MD4 (RFC 1320, clean-room) ----------------------------------------- */

struct b34_md4 {
    uint32_t aState[4];
    uint32_t aCount[2];
    unsigned char aBuf[64];
};

static uint32_t
b34_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint32_t
b34_get_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b34_md4_init(struct b34_md4 *p)
{
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0x67452301u;
    p->aState[1] = 0xefcdab89u;
    p->aState[2] = 0x98badcfeu;
    p->aState[3] = 0x10325476u;
}

static void
b34_md4_block(struct b34_md4 *p, const unsigned char *pBlk)
{
    uint32_t a = p->aState[0], b = p->aState[1], c = p->aState[2],
             d = p->aState[3];
    uint32_t x[16];
    unsigned i;

    for (i = 0; i < 16u; i++) {
        x[i] = b34_get_le32(pBlk + i * 4u);
    }

#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define STEP(f, a, b, c, d, xk, s)                                            \
    do {                                                                      \
        (a) += f((b), (c), (d)) + (xk);                                       \
        (a) = b34_rol32((a), (s));                                            \
    } while (0)
#define STEPG(f, a, b, c, d, xk, s)                                           \
    do {                                                                      \
        (a) += f((b), (c), (d)) + (xk) + 0x5a827999u;                         \
        (a) = b34_rol32((a), (s));                                            \
    } while (0)
#define STEPH(f, a, b, c, d, xk, s)                                           \
    do {                                                                      \
        (a) += f((b), (c), (d)) + (xk) + 0x6ed9eba1u;                         \
        (a) = b34_rol32((a), (s));                                            \
    } while (0)

    STEP(F, a, b, c, d, x[0], 3);
    STEP(F, d, a, b, c, x[1], 7);
    STEP(F, c, d, a, b, x[2], 11);
    STEP(F, b, c, d, a, x[3], 19);
    STEP(F, a, b, c, d, x[4], 3);
    STEP(F, d, a, b, c, x[5], 7);
    STEP(F, c, d, a, b, x[6], 11);
    STEP(F, b, c, d, a, x[7], 19);
    STEP(F, a, b, c, d, x[8], 3);
    STEP(F, d, a, b, c, x[9], 7);
    STEP(F, c, d, a, b, x[10], 11);
    STEP(F, b, c, d, a, x[11], 19);
    STEP(F, a, b, c, d, x[12], 3);
    STEP(F, d, a, b, c, x[13], 7);
    STEP(F, c, d, a, b, x[14], 11);
    STEP(F, b, c, d, a, x[15], 19);

    STEPG(G, a, b, c, d, x[0], 3);
    STEPG(G, d, a, b, c, x[4], 5);
    STEPG(G, c, d, a, b, x[8], 9);
    STEPG(G, b, c, d, a, x[12], 13);
    STEPG(G, a, b, c, d, x[1], 3);
    STEPG(G, d, a, b, c, x[5], 5);
    STEPG(G, c, d, a, b, x[9], 9);
    STEPG(G, b, c, d, a, x[13], 13);
    STEPG(G, a, b, c, d, x[2], 3);
    STEPG(G, d, a, b, c, x[6], 5);
    STEPG(G, c, d, a, b, x[10], 9);
    STEPG(G, b, c, d, a, x[14], 13);
    STEPG(G, a, b, c, d, x[3], 3);
    STEPG(G, d, a, b, c, x[7], 5);
    STEPG(G, c, d, a, b, x[11], 9);
    STEPG(G, b, c, d, a, x[15], 13);

    STEPH(H, a, b, c, d, x[0], 3);
    STEPH(H, d, a, b, c, x[8], 9);
    STEPH(H, c, d, a, b, x[4], 11);
    STEPH(H, b, c, d, a, x[12], 15);
    STEPH(H, a, b, c, d, x[2], 3);
    STEPH(H, d, a, b, c, x[10], 9);
    STEPH(H, c, d, a, b, x[6], 11);
    STEPH(H, b, c, d, a, x[14], 15);
    STEPH(H, a, b, c, d, x[1], 3);
    STEPH(H, d, a, b, c, x[9], 9);
    STEPH(H, c, d, a, b, x[5], 11);
    STEPH(H, b, c, d, a, x[13], 15);
    STEPH(H, a, b, c, d, x[3], 3);
    STEPH(H, d, a, b, c, x[11], 9);
    STEPH(H, c, d, a, b, x[7], 11);
    STEPH(H, b, c, d, a, x[15], 15);

#undef F
#undef G
#undef H
#undef STEP
#undef STEPG
#undef STEPH

    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
}

void
MD4Init(void *pCtx)
{
    if (pCtx != NULL) {
        b34_md4_init((struct b34_md4 *)pCtx);
    }
}

void
MD4Update(void *pCtx, const void *pData, size_t cb)
{
    struct b34_md4 *p = (struct b34_md4 *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    uint32_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aCount[0] == 0u) {
        b34_md4_init(p);
    }
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    p->aCount[0] += (uint32_t)(cb << 3);
    if (p->aCount[0] < (uint32_t)(cb << 3)) {
        p->aCount[1]++;
    }
    p->aCount[1] += (uint32_t)(cb >> 29);
    nPart = 64u - (size_t)nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b34_md4_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 64u) {
            b34_md4_block(p, q);
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
MD4Final(unsigned char aDig[16], void *pCtx)
{
    struct b34_md4 *p = (struct b34_md4 *)pCtx;
    unsigned char aBits[8];
    uint32_t nIndex, nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->aState[0] == 0u && p->aState[1] == 0u && p->aCount[0] == 0u) {
        b34_md4_init(p);
    }
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
        MD4Update(p, aPad, nPad);
    }
    MD4Update(p, aBits, 8u);
    for (i = 0; i < 4u; i++) {
        aDig[i * 4u] = (unsigned char)p->aState[i];
        aDig[i * 4u + 1u] = (unsigned char)(p->aState[i] >> 8);
        aDig[i * 4u + 2u] = (unsigned char)(p->aState[i] >> 16);
        aDig[i * 4u + 3u] = (unsigned char)(p->aState[i] >> 24);
    }
    memset(p, 0, sizeof(*p));
}

/* ---- RIPEMD-160 (clean-room) -------------------------------------------- */

struct b34_rmd {
    uint32_t aState[5];
    uint32_t aCount[2];
    unsigned char aBuf[64];
};

static void
b34_rmd_init(struct b34_rmd *p)
{
    memset(p, 0, sizeof(*p));
    p->aState[0] = 0x67452301u;
    p->aState[1] = 0xefcdab89u;
    p->aState[2] = 0x98badcfeu;
    p->aState[3] = 0x10325476u;
    p->aState[4] = 0xc3d2e1f0u;
}

static void
b34_rmd_block(struct b34_rmd *p, const unsigned char *pBlk)
{
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
        x[i] = b34_get_le32(pBlk + i * 4u);
    }
    al = ar = p->aState[0];
    bl = br = p->aState[1];
    cl = cr = p->aState[2];
    dl = dr = p->aState[3];
    el = er = p->aState[4];

    for (i = 0; i < 80u; i++) {
        uint32_t f, t;
        unsigned r = i / 16u;

        if (r == 0u) {
            f = bl ^ cl ^ dl;
        } else if (r == 1u) {
            f = (bl & cl) | ((~bl) & dl);
        } else if (r == 2u) {
            f = (bl | (~cl)) ^ dl;
        } else if (r == 3u) {
            f = (bl & dl) | (cl & (~dl));
        } else {
            f = bl ^ (cl | (~dl));
        }
        t = b34_rol32(al + f + x[aR[i]] + aK[r], aS[i]) + el;
        al = el;
        el = dl;
        dl = b34_rol32(cl, 10u);
        cl = bl;
        bl = t;

        if (r == 0u) {
            f = br ^ (cr | (~dr));
        } else if (r == 1u) {
            f = (br & dr) | (cr & (~dr));
        } else if (r == 2u) {
            f = (br | (~cr)) ^ dr;
        } else if (r == 3u) {
            f = (br & cr) | ((~br) & dr);
        } else {
            f = br ^ cr ^ dr;
        }
        t = b34_rol32(ar + f + x[aRp[i]] + aKp[r], aSp[i]) + er;
        ar = er;
        er = dr;
        dr = b34_rol32(cr, 10u);
        cr = br;
        br = t;
    }

    {
        uint32_t t = p->aState[1] + cl + dr;

        p->aState[1] = p->aState[2] + dl + er;
        p->aState[2] = p->aState[3] + el + ar;
        p->aState[3] = p->aState[4] + al + br;
        p->aState[4] = p->aState[0] + bl + cr;
        p->aState[0] = t;
    }
}

void
RMD160Init(void *pCtx)
{
    if (pCtx != NULL) {
        b34_rmd_init((struct b34_rmd *)pCtx);
    }
}

void
RMD160Update(void *pCtx, const void *pData, size_t cb)
{
    struct b34_rmd *p = (struct b34_rmd *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;
    uint32_t nIndex;
    size_t nPart;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    if (p->aState[0] == 0u && p->aCount[0] == 0u) {
        b34_rmd_init(p);
    }
    nIndex = (p->aCount[0] >> 3) & 0x3fu;
    p->aCount[0] += (uint32_t)(cb << 3);
    if (p->aCount[0] < (uint32_t)(cb << 3)) {
        p->aCount[1]++;
    }
    p->aCount[1] += (uint32_t)(cb >> 29);
    nPart = 64u - (size_t)nIndex;
    if (cb >= nPart) {
        memcpy(p->aBuf + nIndex, q, nPart);
        b34_rmd_block(p, p->aBuf);
        q += nPart;
        cb -= nPart;
        while (cb >= 64u) {
            b34_rmd_block(p, q);
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
RMD160Final(unsigned char aDig[20], void *pCtx)
{
    struct b34_rmd *p = (struct b34_rmd *)pCtx;
    unsigned char aBits[8];
    uint32_t nIndex, nPad;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->aState[0] == 0u && p->aCount[0] == 0u) {
        b34_rmd_init(p);
    }
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
        RMD160Update(p, aPad, nPad);
    }
    RMD160Update(p, aBits, 8u);
    for (i = 0; i < 5u; i++) {
        aDig[i * 4u] = (unsigned char)p->aState[i];
        aDig[i * 4u + 1u] = (unsigned char)(p->aState[i] >> 8);
        aDig[i * 4u + 2u] = (unsigned char)(p->aState[i] >> 16);
        aDig[i * 4u + 3u] = (unsigned char)(p->aState[i] >> 24);
    }
    memset(p, 0, sizeof(*p));
}

/* ---- BLAKE2s-256 (RFC 7693, clean-room, no key) ------------------------- */

struct b34_b2s {
    uint32_t aH[8];
    uint32_t aT[2];
    uint32_t aF[2];
    unsigned char aBuf[64];
    size_t cbBuf;
    size_t cbOut;
};

static const uint32_t s_b34_b2s_iv[8] = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
    0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

static const unsigned char s_b34_b2s_sigma[10][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}};

static uint32_t
b34_ror32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void
b34_b2s_g(uint32_t *v, unsigned a, unsigned b, unsigned c, unsigned d,
          uint32_t x, uint32_t y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = b34_ror32(v[d] ^ v[a], 16u);
    v[c] = v[c] + v[d];
    v[b] = b34_ror32(v[b] ^ v[c], 12u);
    v[a] = v[a] + v[b] + y;
    v[d] = b34_ror32(v[d] ^ v[a], 8u);
    v[c] = v[c] + v[d];
    v[b] = b34_ror32(v[b] ^ v[c], 7u);
}

static void
b34_b2s_compress(struct b34_b2s *p, int nLast)
{
    uint32_t v[16];
    uint32_t m[16];
    unsigned i, r;

    for (i = 0; i < 8u; i++) {
        v[i] = p->aH[i];
        v[i + 8u] = s_b34_b2s_iv[i];
    }
    v[12] ^= p->aT[0];
    v[13] ^= p->aT[1];
    if (nLast) {
        v[14] = ~v[14];
    }
    for (i = 0; i < 16u; i++) {
        m[i] = b34_get_le32(p->aBuf + i * 4u);
    }
    for (r = 0; r < 10u; r++) {
        const unsigned char *s = s_b34_b2s_sigma[r];

        b34_b2s_g(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        b34_b2s_g(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        b34_b2s_g(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        b34_b2s_g(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        b34_b2s_g(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        b34_b2s_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        b34_b2s_g(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        b34_b2s_g(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }
    for (i = 0; i < 8u; i++) {
        p->aH[i] ^= v[i] ^ v[i + 8u];
    }
}

void
BLAKE2s_Init(void *pCtx)
{
    struct b34_b2s *p = (struct b34_b2s *)pCtx;
    unsigned i;

    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    for (i = 0; i < 8u; i++) {
        p->aH[i] = s_b34_b2s_iv[i];
    }
    p->cbOut = 32u;
    /* param block: digest length 32, fanout 1, depth 1 */
    p->aH[0] ^= 0x01010000u ^ (uint32_t)p->cbOut;
}

void
BLAKE2s_Update(void *pCtx, const void *pData, size_t cb)
{
    struct b34_b2s *p = (struct b34_b2s *)pCtx;
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || (pData == NULL && cb != 0)) {
        return;
    }
    if (p->aH[0] == 0u && p->cbOut == 0u) {
        BLAKE2s_Init(p);
    }
    while (cb > 0u) {
        size_t nLeft = 64u - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;

        memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == 64u) {
            p->aT[0] += 64u;
            if (p->aT[0] < 64u) {
                p->aT[1]++;
            }
            b34_b2s_compress(p, 0);
            p->cbBuf = 0;
        }
    }
}

void
BLAKE2s_Final(unsigned char aDig[32], void *pCtx)
{
    struct b34_b2s *p = (struct b34_b2s *)pCtx;
    unsigned i;

    if (p == NULL || aDig == NULL) {
        return;
    }
    if (p->aH[0] == 0u && p->cbOut == 0u) {
        BLAKE2s_Init(p);
    }
    p->aT[0] += (uint32_t)p->cbBuf;
    if (p->aT[0] < (uint32_t)p->cbBuf) {
        p->aT[1]++;
    }
    memset(p->aBuf + p->cbBuf, 0, 64u - p->cbBuf);
    b34_b2s_compress(p, 1);
    for (i = 0; i < 8u; i++) {
        aDig[i * 4u] = (unsigned char)p->aH[i];
        aDig[i * 4u + 1u] = (unsigned char)(p->aH[i] >> 8);
        aDig[i * 4u + 2u] = (unsigned char)(p->aH[i] >> 16);
        aDig[i * 4u + 3u] = (unsigned char)(p->aH[i] >> 24);
    }
    memset(p, 0, sizeof(*p));
}

/* ---- bcrypt stubs (link surface; full Blowfish out of scope) ------------ */

int
bcrypt_gensalt(int nLogRounds, char *szSalt)
{
    (void)nLogRounds;
    (void)szSalt;
    errno = ENOSYS;
    return -1;
}

char *
bcrypt(const char *szKey, const char *szSalt)
{
    (void)szKey;
    (void)szSalt;
    errno = ENOSYS;
    return NULL;
}

/* ---- BSD fflags helpers (minimal name table) ---------------------------- */

int
strtofflags(char **ppStr, unsigned long *pSet, unsigned long *pClear)
{
    (void)ppStr;
    if (pSet != NULL) {
        *pSet = 0;
    }
    if (pClear != NULL) {
        *pClear = 0;
    }
    return 0;
}

char *
fflagstostr(unsigned long uFlags)
{
    char *p;

    (void)uFlags;
    p = (char *)malloc(1u);
    if (p != NULL) {
        p[0] = '\0';
    }
    return p;
}

/* ---- TIRPC netconfig stubs ---------------------------------------------- */

void *
setnetconfig(void)
{
    errno = ENOSYS;
    return NULL;
}

void *
getnetconfig(void *pHandle)
{
    (void)pHandle;
    errno = ENOSYS;
    return NULL;
}

int
endnetconfig(void *pHandle)
{
    (void)pHandle;
    return 0;
}

void *
getnetconfigent(const char *szNetId)
{
    (void)szNetId;
    errno = ENOSYS;
    return NULL;
}

void
freenetconfigent(void *pEnt)
{
    (void)pEnt;
}

/* ---- 64-bit alias ------------------------------------------------------- */

ssize_t
copy_file_range64(int nIn, off_t *pOffIn, int nOut, off_t *pOffOut, size_t cb,
                  unsigned uFlags)
{
    return copy_file_range(nIn, pOffIn, nOut, pOffOut, cb, uFlags);
}

/* ---- remaining BSD vis family (thin wrappers on batch31/32) ------------- */

int
nvis(char *pDst, size_t cbDst, int nChar, int nFlags)
{
    char aSrc[2];

    aSrc[0] = (char)nChar;
    aSrc[1] = '\0';
    return strnvis(pDst, cbDst, aSrc, nFlags);
}

int
svis(char *pDst, size_t cbDst, int nChar, int nFlags, const char *szExtra)
{
    (void)szExtra;
    return nvis(pDst, cbDst, nChar, nFlags);
}

int
snvis(char *pDst, size_t cbDst, int nChar, int nFlags, const char *szExtra)
{
    (void)szExtra;
    return nvis(pDst, cbDst, nChar, nFlags);
}

int
strsvis(char *pDst, const char *szSrc, int nFlags, const char *szExtra)
{
    char aTmp[512];
    int n;

    (void)szExtra;
    if (pDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strnvis(aTmp, sizeof(aTmp), szSrc != NULL ? szSrc : "", nFlags);
    if (n < 0) {
        return -1;
    }
    memcpy(pDst, aTmp, (size_t)n + 1u);
    return n;
}

int
strsnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags,
         const char *szExtra)
{
    (void)szExtra;
    return strnvis(pDst, cbDst, szSrc, nFlags);
}

int
strsvisx(char *pDst, const char *szSrc, size_t cbSrc, int nFlags,
         const char *szExtra)
{
    char aTmp[512];
    int n;

    (void)szExtra;
    if (pDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strnvisx(aTmp, sizeof(aTmp), szSrc, cbSrc, nFlags);
    if (n < 0) {
        return -1;
    }
    memcpy(pDst, aTmp, (size_t)n + 1u);
    return n;
}

int
strsnvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc, int nFlags,
          const char *szExtra)
{
    (void)szExtra;
    return strnvisx(pDst, cbDst, szSrc, cbSrc, nFlags);
}

int
strenvisx(char **ppDst, const char *szSrc, size_t cbSrc, int nFlags,
          const char *szExtra)
{
    char aTmp[512];
    char *p;
    int n;

    (void)szExtra;
    if (ppDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strnvisx(aTmp, sizeof(aTmp), szSrc, cbSrc, nFlags);
    if (n < 0) {
        return -1;
    }
    p = (char *)malloc((size_t)n + 1u);
    if (p == NULL) {
        return -1;
    }
    memcpy(p, aTmp, (size_t)n + 1u);
    *ppDst = p;
    return n;
}

int
strunvisx(char *pDst, const char *szSrc, int nFlags)
{
    (void)nFlags;
    return strunvis(pDst, szSrc);
}

/* underscored aliases */
int
__nvis(char *pDst, size_t cbDst, int nChar, int nFlags)
{
    return nvis(pDst, cbDst, nChar, nFlags);
}

int
__svis(char *pDst, size_t cbDst, int nChar, int nFlags, const char *szExtra)
{
    return svis(pDst, cbDst, nChar, nFlags, szExtra);
}

int
__snvis(char *pDst, size_t cbDst, int nChar, int nFlags, const char *szExtra)
{
    return snvis(pDst, cbDst, nChar, nFlags, szExtra);
}

int
__strsvis(char *pDst, const char *szSrc, int nFlags, const char *szExtra)
{
    return strsvis(pDst, szSrc, nFlags, szExtra);
}

int
__strsnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags,
           const char *szExtra)
{
    return strsnvis(pDst, cbDst, szSrc, nFlags, szExtra);
}

int
__strsvisx(char *pDst, const char *szSrc, size_t cbSrc, int nFlags,
           const char *szExtra)
{
    return strsvisx(pDst, szSrc, cbSrc, nFlags, szExtra);
}

int
__strsnvisx(char *pDst, size_t cbDst, const char *szSrc, size_t cbSrc,
            int nFlags, const char *szExtra)
{
    return strsnvisx(pDst, cbDst, szSrc, cbSrc, nFlags, szExtra);
}

int
__strenvisx(char **ppDst, const char *szSrc, size_t cbSrc, int nFlags,
            const char *szExtra)
{
    return strenvisx(ppDst, szSrc, cbSrc, nFlags, szExtra);
}

int
__strunvisx(char *pDst, const char *szSrc, int nFlags)
{
    return strunvisx(pDst, szSrc, nFlags);
}

int
__strnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags)
{
    return strnvis(pDst, cbDst, szSrc, nFlags);
}

int
__stravis(char **ppDst, const char *szSrc, int nFlags)
{
    return stravis(ppDst, szSrc, nFlags);
}

int
__strnunvis(char *pDst, size_t cbDst, const char *szSrc)
{
    return strnunvis(pDst, cbDst, szSrc);
}

int
__strunvis(char *pDst, const char *szSrc)
{
    return strunvis(pDst, szSrc);
}

/* ---- soft deepen: ctx sizes + bcrypt_gensalt bounds (unique) ------------ */

size_t
rmd160_ctx_size(void)
{
    return sizeof(struct b34_rmd);
}

size_t __rmd160_ctx_size(void) __attribute__((alias("rmd160_ctx_size")));

size_t
blake2s_ctx_size(void)
{
    return sizeof(struct b34_b2s);
}

size_t __blake2s_ctx_size(void) __attribute__((alias("blake2s_ctx_size")));

size_t
md4_ctx_size(void)
{
    return sizeof(struct b34_md4);
}

size_t __md4_ctx_size(void) __attribute__((alias("md4_ctx_size")));

/*
 * bcrypt_gensalt_chk: same as bcrypt_gensalt but reject out-of-range log
 * rounds before stub (OpenBSD accepts 4..31).
 */
int
bcrypt_gensalt_chk(int nLogRounds, char *szSalt)
{
    if (nLogRounds < 4 || nLogRounds > 31) {
        errno = EINVAL;
        return -1;
    }
    return bcrypt_gensalt(nLogRounds, szSalt);
}

int __bcrypt_gensalt_chk(int nLogRounds, char *szSalt)
    __attribute__((alias("bcrypt_gensalt_chk")));
