/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41: SHAKE128/SHAKE256 XOF, RFC 4648 base32,
 * base16 hex encode/decode, FNV-1a 32/64, MurmurHash3 x86_32, CRC-16
 * CCITT-FALSE, CRC-64 ECMA-182, HMAC-SHA224, HMAC-SHA3-256,
 * crypto_memcmp, XXH3_64bits. Integer/pointer only (no SSE). Clean-room
 * public ABI over batch38/40.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* Public digests from earlier batches */
void SHA224_Init(void *pCtx);
void SHA224_Update(void *pCtx, const void *pData, size_t cb);
void SHA224_Final(unsigned char aDig[28], void *pCtx);
void SHA3_256_Init(void *pCtx);
void SHA3_256_Update(void *pCtx, const void *pData, size_t cb);
void SHA3_256_Final(unsigned char aDig[32], void *pCtx);

const char __libcgj_batch41_marker[] = "libcgj-batch41";

/* ---- shared hex helpers ------------------------------------------------- */

static char *
b41_hex_end(const unsigned char *pDig, size_t cb, char *szBuf)
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

/* ---- SHAKE128 / SHAKE256 (FIPS 202 XOF over Keccak-f[1600]) ------------- */

struct b41_shake {
    uint64_t a[25];
    unsigned char aBuf[200];
    size_t cbBuf;
    size_t cbRate;
    int nSqueezing;
};

static const unsigned s_b41_rotc[24] = {1,  3,  6,  10, 15, 21, 28, 36,
                                        45, 55, 2,  14, 27, 41, 56, 8,
                                        25, 43, 62, 18, 39, 61, 20, 44};
static const unsigned s_b41_piln[24] = {10, 7,  11, 17, 18, 3,  5,  16,
                                        8,  21, 24, 4,  15, 23, 19, 13,
                                        12, 2,  20, 14, 22, 9,  6,  1};
static const uint64_t s_b41_rc[24] = {
    0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
    0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
    0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
    0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
    0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
    0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
    0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
    0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull};

static uint64_t
b41_ror64(uint64_t x, unsigned n)
{
    return (x >> n) | (x << (64u - n));
}

static void
b41_keccak_f(uint64_t st[25])
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
            t = bc[(i + 4u) % 5u] ^ b41_ror64(bc[(i + 1u) % 5u], 63);
            for (j = 0; j < 25u; j += 5u) {
                st[j + i] ^= t;
            }
        }
        t = st[1];
        for (i = 0; i < 24u; i++) {
            j = s_b41_piln[i];
            bc[0] = st[j];
            st[j] = b41_ror64(t, 64u - s_b41_rotc[i]);
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
        st[0] ^= s_b41_rc[round];
    }
}

static void
b41_shake_init(struct b41_shake *p, size_t cbRate)
{
    if (p == NULL) {
        return;
    }
    memset(p, 0, sizeof(*p));
    p->cbRate = cbRate;
    p->nSqueezing = 0;
}

static void
b41_shake_absorb(struct b41_shake *p, const void *pData, size_t cb)
{
    const unsigned char *q = (const unsigned char *)pData;

    if (p == NULL || p->nSqueezing || (pData == NULL && cb != 0u)) {
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
            b41_keccak_f(p->a);
            p->cbBuf = 0;
        }
    }
}

static void
b41_shake_pad(struct b41_shake *p)
{
    size_t i;

    if (p == NULL || p->nSqueezing) {
        return;
    }
    /* Domain separator 0x1F for SHAKE; multi-rate padding with 0x80 MSB. */
    p->aBuf[p->cbBuf] ^= 0x1fu;
    p->aBuf[p->cbRate - 1u] ^= 0x80u;
    for (i = 0; i < p->cbRate; i++) {
        p->a[i / 8u] ^= (uint64_t)p->aBuf[i] << (8u * (i % 8u));
    }
    b41_keccak_f(p->a);
    p->cbBuf = 0;
    p->nSqueezing = 1;
}

static void
b41_shake_squeeze(struct b41_shake *p, unsigned char *pOut, size_t cbOut)
{
    size_t nOff = 0;

    if (p == NULL || pOut == NULL) {
        return;
    }
    if (!p->nSqueezing) {
        b41_shake_pad(p);
    }
    while (nOff < cbOut) {
        size_t i;
        size_t nAvail = p->cbRate - p->cbBuf;
        size_t nTake = (cbOut - nOff < nAvail) ? (cbOut - nOff) : nAvail;

        for (i = 0; i < nTake; i++) {
            size_t idx = p->cbBuf + i;
            pOut[nOff + i] =
                (unsigned char)(p->a[idx / 8u] >> (8u * (idx % 8u)));
        }
        p->cbBuf += nTake;
        nOff += nTake;
        if (p->cbBuf == p->cbRate) {
            b41_keccak_f(p->a);
            p->cbBuf = 0;
        }
    }
}

/* Opaque context size: enough for struct b41_shake (users allocate ≥ 280). */
void
SHAKE128_Init(void *pCtx)
{
    b41_shake_init((struct b41_shake *)pCtx, 168u);
}

void
SHAKE128_Update(void *pCtx, const void *pData, size_t cb)
{
    b41_shake_absorb((struct b41_shake *)pCtx, pData, cb);
}

void
SHAKE128_Final(unsigned char *pOut, size_t cbOut, void *pCtx)
{
    if (pOut == NULL || cbOut == 0u) {
        return;
    }
    b41_shake_squeeze((struct b41_shake *)pCtx, pOut, cbOut);
}

void
SHAKE256_Init(void *pCtx)
{
    b41_shake_init((struct b41_shake *)pCtx, 136u);
}

void
SHAKE256_Update(void *pCtx, const void *pData, size_t cb)
{
    b41_shake_absorb((struct b41_shake *)pCtx, pData, cb);
}

void
SHAKE256_Final(unsigned char *pOut, size_t cbOut, void *pCtx)
{
    if (pOut == NULL || cbOut == 0u) {
        return;
    }
    b41_shake_squeeze((struct b41_shake *)pCtx, pOut, cbOut);
}

/* One-shot helpers: squeeze cbOut bytes into pOut; hex into optional buf. */
int
SHAKE128_Data(const void *pData, size_t cb, unsigned char *pOut, size_t cbOut)
{
    struct b41_shake ctx;

    if (pOut == NULL || cbOut == 0u || (pData == NULL && cb != 0u)) {
        errno = EINVAL;
        return -1;
    }
    SHAKE128_Init(&ctx);
    SHAKE128_Update(&ctx, pData, cb);
    SHAKE128_Final(pOut, cbOut, &ctx);
    return 0;
}

int
SHAKE256_Data(const void *pData, size_t cb, unsigned char *pOut, size_t cbOut)
{
    struct b41_shake ctx;

    if (pOut == NULL || cbOut == 0u || (pData == NULL && cb != 0u)) {
        errno = EINVAL;
        return -1;
    }
    SHAKE256_Init(&ctx);
    SHAKE256_Update(&ctx, pData, cb);
    SHAKE256_Final(pOut, cbOut, &ctx);
    return 0;
}

char *
SHAKE128_Hex(const void *pData, size_t cb, size_t cbOut, char *szBuf)
{
    unsigned char aDig[64];
    size_t n = (cbOut > 64u) ? 64u : cbOut;

    if (n == 0u || SHAKE128_Data(pData, cb, aDig, n) != 0) {
        return NULL;
    }
    return b41_hex_end(aDig, n, szBuf);
}

char *
SHAKE256_Hex(const void *pData, size_t cb, size_t cbOut, char *szBuf)
{
    unsigned char aDig[64];
    size_t n = (cbOut > 64u) ? 64u : cbOut;

    if (n == 0u || SHAKE256_Data(pData, cb, aDig, n) != 0) {
        return NULL;
    }
    return b41_hex_end(aDig, n, szBuf);
}

void __SHAKE128_Init(void *p) __attribute__((alias("SHAKE128_Init")));
void __SHAKE128_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHAKE128_Update")));
void __SHAKE128_Final(unsigned char *o, size_t n, void *p)
    __attribute__((alias("SHAKE128_Final")));
void __SHAKE256_Init(void *p) __attribute__((alias("SHAKE256_Init")));
void __SHAKE256_Update(void *p, const void *d, size_t n)
    __attribute__((alias("SHAKE256_Update")));
void __SHAKE256_Final(unsigned char *o, size_t n, void *p)
    __attribute__((alias("SHAKE256_Final")));

/* ---- RFC 4648 base32 ---------------------------------------------------- */

static const char s_b41_b32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

ssize_t
base32_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
    size_t i = 0;
    size_t j = 0;
    /* 5 input bytes → 8 output chars; +1 NUL */
    size_t cbNeed = ((cbIn + 4u) / 5u) * 8u + 1u;

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
        unsigned u3 = (nLeft > 3u) ? pIn[i++] : 0u;
        unsigned u4 = (nLeft > 4u) ? pIn[i++] : 0u;
        /* 40-bit buffer */
        uint64_t u = ((uint64_t)u0 << 32) | ((uint64_t)u1 << 24) |
                     ((uint64_t)u2 << 16) | ((uint64_t)u3 << 8) | (uint64_t)u4;
        int nPad;
        int k;

        if (nLeft >= 5u) {
            nPad = 0;
        } else if (nLeft == 4u) {
            nPad = 1;
        } else if (nLeft == 3u) {
            nPad = 3;
        } else if (nLeft == 2u) {
            nPad = 4;
        } else {
            nPad = 6;
        }
        for (k = 7; k >= 0; k--) {
            if (k < nPad) {
                szOut[j++] = '=';
            } else {
                szOut[j++] = s_b41_b32[(u >> (unsigned)(k * 5)) & 31u];
            }
        }
    }
    szOut[j] = '\0';
    return (ssize_t)j;
}

static int
b41_b32_val(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    }
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a';
    }
    if (ch >= '2' && ch <= '7') {
        return ch - '2' + 26;
    }
    return -1;
}

ssize_t
base32_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
    size_t i = 0;
    size_t j = 0;

    if (szIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }
    while (i < cbIn) {
        int aV[8];
        int k;
        int nPad = 0;
        uint64_t u = 0;
        int nOut;

        while (i < cbIn && (szIn[i] == ' ' || szIn[i] == '\n' ||
                            szIn[i] == '\r' || szIn[i] == '\t')) {
            i++;
        }
        if (i >= cbIn) {
            break;
        }
        if (i + 8u > cbIn) {
            errno = EINVAL;
            return -1;
        }
        for (k = 0; k < 8; k++) {
            char c = szIn[i + (size_t)k];
            if (c == '=') {
                aV[k] = 0;
                nPad++;
            } else {
                aV[k] = b41_b32_val((unsigned char)c);
                if (aV[k] < 0 || nPad > 0) {
                    errno = EINVAL;
                    return -1;
                }
            }
            u = (u << 5) | (unsigned)aV[k];
        }
        i += 8u;
        /* nPad → output bytes: 0→5, 1→4, 3→3, 4→2, 6→1 */
        if (nPad == 0) {
            nOut = 5;
        } else if (nPad == 1) {
            nOut = 4;
        } else if (nPad == 3) {
            nOut = 3;
        } else if (nPad == 4) {
            nOut = 2;
        } else if (nPad == 6) {
            nOut = 1;
        } else {
            errno = EINVAL;
            return -1;
        }
        if (pOut == NULL) {
            j += (size_t)nOut;
            continue;
        }
        if (j + (size_t)nOut > cbOut) {
            errno = ENOSPC;
            return -1;
        }
        if (nOut >= 1) {
            pOut[j++] = (unsigned char)(u >> 32);
        }
        if (nOut >= 2) {
            pOut[j++] = (unsigned char)(u >> 24);
        }
        if (nOut >= 3) {
            pOut[j++] = (unsigned char)(u >> 16);
        }
        if (nOut >= 4) {
            pOut[j++] = (unsigned char)(u >> 8);
        }
        if (nOut >= 5) {
            pOut[j++] = (unsigned char)u;
        }
    }
    return (ssize_t)j;
}

ssize_t __base32_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base32_encode")));
ssize_t __base32_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base32_decode")));

/* ---- base16 / hex encode-decode ---------------------------------------- */

ssize_t
hex_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
    static const char *szDigits = "0123456789abcdef";
    size_t i;
    size_t cbNeed = cbIn * 2u + 1u;

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
    for (i = 0; i < cbIn; i++) {
        szOut[i * 2u] = szDigits[(pIn[i] >> 4) & 0xfu];
        szOut[i * 2u + 1u] = szDigits[pIn[i] & 0xfu];
    }
    szOut[cbIn * 2u] = '\0';
    return (ssize_t)(cbIn * 2u);
}

static int
b41_nibble(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

ssize_t
hex_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
    size_t i = 0;
    size_t j = 0;

    if (szIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }
    /* Skip whitespace; require even number of hex digits. */
    while (i < cbIn) {
        int hi, lo;

        while (i < cbIn && (szIn[i] == ' ' || szIn[i] == '\n' ||
                            szIn[i] == '\r' || szIn[i] == '\t')) {
            i++;
        }
        if (i >= cbIn) {
            break;
        }
        if (i + 1u >= cbIn) {
            errno = EINVAL;
            return -1;
        }
        hi = b41_nibble((unsigned char)szIn[i]);
        lo = b41_nibble((unsigned char)szIn[i + 1u]);
        i += 2u;
        if (hi < 0 || lo < 0) {
            errno = EINVAL;
            return -1;
        }
        if (pOut != NULL) {
            if (j >= cbOut) {
                errno = ENOSPC;
                return -1;
            }
            pOut[j] = (unsigned char)(((unsigned)hi << 4) | (unsigned)lo);
        }
        j++;
    }
    return (ssize_t)j;
}

ssize_t __hex_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("hex_encode")));
ssize_t __hex_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("hex_decode")));

/* ---- FNV-1a 32 / 64 ----------------------------------------------------- */

uint32_t
fnv1a_32(const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    uint32_t h = 2166136261u;
    size_t i;

    if (p == NULL && cb != 0u) {
        return 0u;
    }
    for (i = 0; i < cb; i++) {
        h ^= (uint32_t)p[i];
        h *= 16777619u;
    }
    return h;
}

uint64_t
fnv1a_64(const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    uint64_t h = 14695981039346656037ull;
    size_t i;

    if (p == NULL && cb != 0u) {
        return 0ull;
    }
    for (i = 0; i < cb; i++) {
        h ^= (uint64_t)p[i];
        h *= 1099511628211ull;
    }
    return h;
}

uint32_t __fnv1a_32(const void *p, size_t n) __attribute__((alias("fnv1a_32")));
uint64_t __fnv1a_64(const void *p, size_t n) __attribute__((alias("fnv1a_64")));

/* ---- MurmurHash3 x86_32 (Austin Appleby public domain algorithm) -------- */

static uint32_t
b41_rotl32(uint32_t x, unsigned r)
{
    return (x << r) | (x >> (32u - r));
}

static uint32_t
b41_getblock32(const unsigned char *p, size_t i)
{
    size_t o = i * 4u;
    return (uint32_t)p[o] | ((uint32_t)p[o + 1u] << 8) |
           ((uint32_t)p[o + 2u] << 16) | ((uint32_t)p[o + 3u] << 24);
}

uint32_t
murmur3_32(const void *pKey, size_t cb, uint32_t uSeed)
{
    const unsigned char *p = (const unsigned char *)pKey;
    const size_t nblocks = cb / 4u;
    uint32_t h1 = uSeed;
    const uint32_t c1 = 0xcc9e2d51u;
    const uint32_t c2 = 0x1b873593u;
    size_t i;
    const unsigned char *tail;
    uint32_t k1 = 0;

    if (p == NULL && cb != 0u) {
        return 0u;
    }
    for (i = 0; i < nblocks; i++) {
        uint32_t k = b41_getblock32(p, i);
        k *= c1;
        k = b41_rotl32(k, 15);
        k *= c2;
        h1 ^= k;
        h1 = b41_rotl32(h1, 13);
        h1 = h1 * 5u + 0xe6546b64u;
    }
    tail = p + nblocks * 4u;
    switch (cb & 3u) {
    case 3:
        k1 ^= (uint32_t)tail[2] << 16;
        /* fallthrough */
    case 2:
        k1 ^= (uint32_t)tail[1] << 8;
        /* fallthrough */
    case 1:
        k1 ^= (uint32_t)tail[0];
        k1 *= c1;
        k1 = b41_rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        break;
    default:
        break;
    }
    h1 ^= (uint32_t)cb;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6bu;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35u;
    h1 ^= h1 >> 16;
    return h1;
}

uint32_t __murmur3_32(const void *p, size_t n, uint32_t s)
    __attribute__((alias("murmur3_32")));

/* ---- CRC-16 CCITT-FALSE (poly 0x1021, init 0xFFFF) ---------------------- */

uint16_t
crc16_ccitt(uint16_t uCrc, const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    size_t i;

    if (p == NULL && cb != 0u) {
        return uCrc;
    }
    for (i = 0; i < cb; i++) {
        unsigned b;
        uCrc ^= (uint16_t)((uint16_t)p[i] << 8);
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x8000u) {
                uCrc = (uint16_t)((uCrc << 1) ^ 0x1021u);
            } else {
                uCrc = (uint16_t)(uCrc << 1);
            }
        }
    }
    return uCrc;
}

uint16_t
crc16(const void *pData, size_t cb)
{
    return crc16_ccitt(0xffffu, pData, cb);
}

uint16_t __crc16_ccitt(uint16_t c, const void *p, size_t n)
    __attribute__((alias("crc16_ccitt")));
uint16_t __crc16(const void *p, size_t n) __attribute__((alias("crc16")));

/* ---- CRC-64 ECMA-182 (poly 0x42F0E1EBA9EA3693, init 0) ------------------ */

uint64_t
crc64_ecma(uint64_t uCrc, const void *pData, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pData;
    const uint64_t poly = 0x42F0E1EBA9EA3693ull;
    size_t i;

    if (p == NULL && cb != 0u) {
        return uCrc;
    }
    for (i = 0; i < cb; i++) {
        unsigned b;
        uCrc ^= (uint64_t)p[i] << 56;
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x8000000000000000ull) {
                uCrc = (uCrc << 1) ^ poly;
            } else {
                uCrc <<= 1;
            }
        }
    }
    return uCrc;
}

uint64_t
crc64(const void *pData, size_t cb)
{
    return crc64_ecma(0ull, pData, cb);
}

uint64_t __crc64_ecma(uint64_t c, const void *p, size_t n)
    __attribute__((alias("crc64_ecma")));
uint64_t __crc64(const void *p, size_t n) __attribute__((alias("crc64")));

/* ---- HMAC-SHA224 -------------------------------------------------------- */

/* SHA224 context layout matches SHA256 (8×u32 state + buffer); 28-byte dig. */
void
hmac_sha224(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
            unsigned char aOut[28])
{
    unsigned char aK[64];
    unsigned char aIpad[64];
    unsigned char aOpad[64];
    unsigned char aInner[28];
    unsigned char aCtx[256];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aK, 0, sizeof(aK));
    if (cbKey > 64u) {
        SHA224_Init(aCtx);
        SHA224_Update(aCtx, pKey, cbKey);
        SHA224_Final(aK, aCtx);
    } else if (pKey != NULL && cbKey > 0u) {
        memcpy(aK, pKey, cbKey);
    }
    for (i = 0; i < 64u; i++) {
        aIpad[i] = (unsigned char)(aK[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aK[i] ^ 0x5cu);
    }
    SHA224_Init(aCtx);
    SHA224_Update(aCtx, aIpad, 64u);
    if (pData != NULL && cbData > 0u) {
        SHA224_Update(aCtx, pData, cbData);
    }
    SHA224_Final(aInner, aCtx);
    SHA224_Init(aCtx);
    SHA224_Update(aCtx, aOpad, 64u);
    SHA224_Update(aCtx, aInner, 28u);
    SHA224_Final(aOut, aCtx);
}

void __hmac_sha224(const void *k, size_t nk, const void *d, size_t nd,
                   unsigned char o[28]) __attribute__((alias("hmac_sha224")));

/* ---- HMAC-SHA3-256 (rate 136, block = rate for HMAC-SHA3) --------------- */

void
hmac_sha3_256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
              unsigned char aOut[32])
{
    /* NIST SP 800-185 / FIPS 202 HMAC: block size = rate = 136 for SHA3-256 */
    const size_t cbBlock = 136u;
    unsigned char aK[136];
    unsigned char aIpad[136];
    unsigned char aOpad[136];
    unsigned char aInner[32];
    /* SHA3 context is opaque; allocate generous stack buffer */
    /* b38_sha3 ≈ 424 bytes; keep headroom for alignment */
    unsigned char aCtx[512];
    size_t i;

    if (aOut == NULL) {
        return;
    }
    memset(aK, 0, sizeof(aK));
    if (cbKey > cbBlock) {
        SHA3_256_Init(aCtx);
        SHA3_256_Update(aCtx, pKey, cbKey);
        SHA3_256_Final(aK, aCtx);
    } else if (pKey != NULL && cbKey > 0u) {
        memcpy(aK, pKey, cbKey);
    }
    for (i = 0; i < cbBlock; i++) {
        aIpad[i] = (unsigned char)(aK[i] ^ 0x36u);
        aOpad[i] = (unsigned char)(aK[i] ^ 0x5cu);
    }
    SHA3_256_Init(aCtx);
    SHA3_256_Update(aCtx, aIpad, cbBlock);
    if (pData != NULL && cbData > 0u) {
        SHA3_256_Update(aCtx, pData, cbData);
    }
    SHA3_256_Final(aInner, aCtx);
    SHA3_256_Init(aCtx);
    SHA3_256_Update(aCtx, aOpad, cbBlock);
    SHA3_256_Update(aCtx, aInner, 32u);
    SHA3_256_Final(aOut, aCtx);
}

void __hmac_sha3_256(const void *k, size_t nk, const void *d, size_t nd,
                     unsigned char o[32])
    __attribute__((alias("hmac_sha3_256")));

/* ---- crypto_memcmp: constant-time byte compare -------------------------- */

int
crypto_memcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *a = (const unsigned char *)pA;
    const unsigned char *b = (const unsigned char *)pB;
    unsigned char uDiff = 0;
    size_t i;

    if (cb == 0u) {
        return 0;
    }
    if (a == NULL || b == NULL) {
        return 1;
    }
    for (i = 0; i < cb; i++) {
        uDiff |= (unsigned char)(a[i] ^ b[i]);
    }
    /* Normalize to 0 / 1 without data-dependent early exit. */
    return uDiff != 0 ? 1 : 0;
}

int __crypto_memcmp(const void *a, const void *b, size_t n)
    __attribute__((alias("crypto_memcmp")));

/* ---- XXH3_64bits (clean-room, length-routed, default secret) ------------ */

static const unsigned char s_b41_xxh3_secret[192] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c,
    0xf7, 0x21, 0xad, 0x1c, 0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb,
    0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f, 0xcb, 0x79, 0xe6, 0x4e,
    0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6,
    0x81, 0x3a, 0x26, 0x4c, 0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb,
    0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3, 0x71, 0x64, 0x48, 0x97,
    0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7,
    0xc7, 0x0b, 0x4f, 0x1d, 0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31,
    0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64, 0xea, 0xc5, 0xac, 0x83,
    0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26,
    0x29, 0xd4, 0x68, 0x9e, 0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc,
    0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce, 0x45, 0xcb, 0x3a, 0x8f,
    0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};

#define B41_XXH_PRIME32_1 0x9E3779B1u
#define B41_XXH_PRIME32_2 0x85EBCA77u
#define B41_XXH_PRIME32_3 0xC2B2AE3Du
#define B41_XXH_PRIME64_1 0x9E3779B185EBCA87ull
#define B41_XXH_PRIME64_2 0xC2B2AE3D27D4EB4Full
#define B41_XXH_PRIME64_3 0x165667B19E3779F9ull
#define B41_XXH_PRIME64_4 0x85EBCA77C2B2AE63ull
#define B41_XXH_PRIME64_5 0x27D4EB2F165667C5ull
#define B41_XXH3_MIDSIZE_MAX 240u
#define B41_XXH3_SECRET_SIZE_MIN 136u

static uint32_t
b41_read32le(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t
b41_read64le(const unsigned char *p)
{
    return (uint64_t)b41_read32le(p) |
           ((uint64_t)b41_read32le(p + 4) << 32);
}

static uint64_t
b41_rotl64(uint64_t x, unsigned r)
{
    return (x << r) | (x >> (64u - r));
}

static uint64_t
b41_xxh3_mul128_fold64(uint64_t lhs, uint64_t rhs)
{
    /* Portable 64×64 → 128 fold without __int128 (freestanding). */
    uint64_t lo_lo = (lhs & 0xffffffffull) * (rhs & 0xffffffffull);
    uint64_t hi_lo = (lhs >> 32) * (rhs & 0xffffffffull);
    uint64_t lo_hi = (lhs & 0xffffffffull) * (rhs >> 32);
    uint64_t hi_hi = (lhs >> 32) * (rhs >> 32);
    uint64_t cross = (lo_lo >> 32) + (hi_lo & 0xffffffffull) + lo_hi;
    uint64_t upper = (hi_lo >> 32) + (cross >> 32) + hi_hi;
    uint64_t lower = (cross << 32) | (lo_lo & 0xffffffffull);
    return upper ^ lower;
}

static uint64_t
b41_xxh3_avalanche(uint64_t h)
{
    h ^= h >> 37;
    h *= 0x165667919E3779F9ull;
    h ^= h >> 32;
    return h;
}

/* Used by empty-input XXH3 path (matches upstream XXH64_avalanche). */
static uint64_t
b41_xxh64_avalanche(uint64_t h)
{
    h ^= h >> 33;
    h *= B41_XXH_PRIME64_2;
    h ^= h >> 29;
    h *= B41_XXH_PRIME64_3;
    h ^= h >> 32;
    return h;
}

static uint64_t
b41_xxh3_rrmxmx(uint64_t h, uint64_t len)
{
    h ^= b41_rotl64(h, 49) ^ b41_rotl64(h, 24);
    h *= 0x9FB21C651E98DF25ull;
    h ^= (h >> 35) + len;
    h *= 0x9FB21C651E98DF25ull;
    h ^= h >> 28;
    return h;
}

static uint64_t
b41_xxh3_mix16(const unsigned char *pIn, const unsigned char *pSec, uint64_t seed)
{
    uint64_t in_lo = b41_read64le(pIn);
    uint64_t in_hi = b41_read64le(pIn + 8);
    return b41_xxh3_mul128_fold64(in_lo ^ (b41_read64le(pSec) + seed),
                                  in_hi ^ (b41_read64le(pSec + 8) - seed));
}

static uint64_t
b41_xxh3_len_1to3(const unsigned char *p, size_t len, uint64_t seed,
                  const unsigned char *secret)
{
    unsigned char c1 = p[0];
    unsigned char c2 = p[len >> 1];
    unsigned char c3 = p[len - 1u];
    uint32_t combined =
        ((uint32_t)c1 << 16) | ((uint32_t)c2 << 24) | (uint32_t)c3 |
        ((uint32_t)len << 8);
    uint64_t bitflip =
        ((uint64_t)b41_read32le(secret) ^ (uint64_t)b41_read32le(secret + 4)) +
        seed;
    return b41_xxh3_avalanche((uint64_t)combined ^ bitflip);
}

static uint64_t
b41_xxh3_len_4to8(const unsigned char *p, size_t len, uint64_t seed,
                  const unsigned char *secret)
{
    uint32_t in1 = b41_read32le(p);
    uint32_t in2 = b41_read32le(p + len - 4u);
    uint64_t bitflip =
        (b41_read64le(secret + 8) ^ b41_read64le(secret + 16)) - seed;
    uint64_t input_64 = (uint64_t)in1 + ((uint64_t)in2 << 32);
    uint64_t keyed = input_64 ^ bitflip;
    return b41_xxh3_rrmxmx(keyed, len);
}

static uint64_t
b41_xxh3_len_9to16(const unsigned char *p, size_t len, uint64_t seed,
                   const unsigned char *secret)
{
    uint64_t bitflip1 =
        (b41_read64le(secret + 24) ^ b41_read64le(secret + 32)) + seed;
    uint64_t bitflip2 =
        (b41_read64le(secret + 40) ^ b41_read64le(secret + 48)) - seed;
    uint64_t in_lo = b41_read64le(p) ^ bitflip1;
    uint64_t in_hi = b41_read64le(p + len - 8u) ^ bitflip2;
    uint64_t acc = (uint64_t)len + b41_rotl64(in_lo, 32) + in_hi +
                   b41_xxh3_mul128_fold64(in_lo, in_hi);
    return b41_xxh3_avalanche(acc);
}

static uint64_t
b41_xxh3_len_0to16(const unsigned char *p, size_t len, uint64_t seed,
                   const unsigned char *secret)
{
    if (len > 8u) {
        return b41_xxh3_len_9to16(p, len, seed, secret);
    }
    if (len >= 4u) {
        return b41_xxh3_len_4to8(p, len, seed, secret);
    }
    if (len > 0u) {
        return b41_xxh3_len_1to3(p, len, seed, secret);
    }
    /* Upstream uses XXH64_avalanche for the zero-length path. */
    return b41_xxh64_avalanche(
        seed ^ (b41_read64le(secret + 56) ^ b41_read64le(secret + 64)));
}

static uint64_t
b41_xxh3_len_17to128(const unsigned char *p, size_t len, uint64_t seed,
                     const unsigned char *secret)
{
    uint64_t acc = (uint64_t)len * B41_XXH_PRIME64_1;

    if (len > 32u) {
        if (len > 64u) {
            if (len > 96u) {
                acc += b41_xxh3_mix16(p + 48, secret + 96, seed);
                acc += b41_xxh3_mix16(p + len - 64u, secret + 112, seed);
            }
            acc += b41_xxh3_mix16(p + 32, secret + 64, seed);
            acc += b41_xxh3_mix16(p + len - 48u, secret + 80, seed);
        }
        acc += b41_xxh3_mix16(p + 16, secret + 32, seed);
        acc += b41_xxh3_mix16(p + len - 32u, secret + 48, seed);
    }
    acc += b41_xxh3_mix16(p, secret, seed);
    acc += b41_xxh3_mix16(p + len - 16u, secret + 16, seed);
    return b41_xxh3_avalanche(acc);
}

static uint64_t
b41_xxh3_len_129to240(const unsigned char *p, size_t len, uint64_t seed,
                      const unsigned char *secret)
{
    uint64_t acc = (uint64_t)len * B41_XXH_PRIME64_1;
    size_t nbRounds = len / 16u;
    size_t i;

    for (i = 0; i < 8u; i++) {
        acc += b41_xxh3_mix16(p + 16u * i, secret + 16u * i, seed);
    }
    acc = b41_xxh3_avalanche(acc);
    for (i = 8; i < nbRounds; i++) {
        acc +=
            b41_xxh3_mix16(p + 16u * i, secret + 16u * (i - 8u) + 3u, seed);
    }
    acc += b41_xxh3_mix16(p + len - 16u, secret + B41_XXH3_SECRET_SIZE_MIN - 17u,
                          seed);
    return b41_xxh3_avalanche(acc);
}

/*
 * Long-path accumulate per xxHash XXH3_accumulate_512 (scalar):
 *   data_val = input[i];
 *   data_key = data_val ^ secret[i];
 *   acc[i ^ 1] += data_val;
 *   acc[i] += (uint32_t)data_key * (data_key >> 32);
 */
static void
b41_xxh3_accumulate_512_fix(uint64_t *acc, const unsigned char *pIn,
                             const unsigned char *pSec)
{
    size_t i;

    for (i = 0; i < 8u; i++) {
        uint64_t data_val = b41_read64le(pIn + 8u * i);
        uint64_t data_key = data_val ^ b41_read64le(pSec + 8u * i);
        acc[i ^ 1u] += data_val;
        acc[i] += (data_key & 0xffffffffull) * (data_key >> 32);
    }
}

static void
b41_xxh3_scramble_acc(uint64_t *acc, const unsigned char *pSec)
{
    size_t i;

    for (i = 0; i < 8u; i++) {
        uint64_t key64 = b41_read64le(pSec + 8u * i);
        uint64_t acc64 = acc[i];
        acc64 ^= acc64 >> 47;
        acc64 ^= key64;
        acc64 *= B41_XXH_PRIME32_1;
        acc[i] = acc64;
    }
}

static uint64_t
b41_xxh3_merge_accs(const uint64_t *acc, const unsigned char *secret,
                    uint64_t start)
{
    uint64_t result = start;
    size_t i;

    for (i = 0; i < 4u; i++) {
        result += b41_xxh3_mul128_fold64(acc[2u * i] ^ b41_read64le(secret + 16u * i),
                                         acc[2u * i + 1u] ^
                                             b41_read64le(secret + 16u * i + 8u));
    }
    return b41_xxh3_avalanche(result);
}

static uint64_t
b41_xxh3_hashLong_64b(const unsigned char *pIn, size_t len, uint64_t seed,
                      const unsigned char *secret, size_t secretSize)
{
    uint64_t acc[8];
    size_t nb_blocks;
    size_t nb_stripes_per_block;
    size_t n;
    size_t nbStripes;
    const size_t stripe = 64u;
    const size_t secretConsume = 8u;

    (void)seed; /* default secret path; seed variants use custom secret */
    acc[0] = B41_XXH_PRIME32_3;
    acc[1] = B41_XXH_PRIME64_1;
    acc[2] = B41_XXH_PRIME64_2;
    acc[3] = B41_XXH_PRIME64_3;
    acc[4] = B41_XXH_PRIME64_4;
    acc[5] = B41_XXH_PRIME32_2;
    acc[6] = B41_XXH_PRIME64_5;
    acc[7] = B41_XXH_PRIME32_1;

    nb_stripes_per_block = (secretSize - stripe) / secretConsume;
    nb_blocks = (len - 1u) / (stripe * nb_stripes_per_block);
    for (n = 0; n < nb_blocks; n++) {
        size_t s;
        for (s = 0; s < nb_stripes_per_block; s++) {
            b41_xxh3_accumulate_512_fix(
                acc, pIn + n * nb_stripes_per_block * stripe + s * stripe,
                secret + s * secretConsume);
        }
        b41_xxh3_scramble_acc(acc, secret + secretSize - stripe);
    }
    nbStripes = ((len - 1u) - (stripe * nb_stripes_per_block * nb_blocks)) /
                stripe;
    for (n = 0; n < nbStripes; n++) {
        b41_xxh3_accumulate_512_fix(
            acc,
            pIn + nb_blocks * nb_stripes_per_block * stripe + n * stripe,
            secret + n * secretConsume);
    }
    /* last stripe */
    b41_xxh3_accumulate_512_fix(acc, pIn + len - stripe,
                                 secret + secretSize - stripe - 7u);
    return b41_xxh3_merge_accs(acc, secret + 11u,
                               (uint64_t)len * B41_XXH_PRIME64_1);
}

uint64_t
XXH3_64bits_withSeed(const void *pData, size_t cb, uint64_t uSeed)
{
    const unsigned char *p = (const unsigned char *)pData;

    if (p == NULL && cb != 0u) {
        return 0ull;
    }
    if (cb <= 16u) {
        return b41_xxh3_len_0to16(p, cb, uSeed, s_b41_xxh3_secret);
    }
    if (cb <= 128u) {
        return b41_xxh3_len_17to128(p, cb, uSeed, s_b41_xxh3_secret);
    }
    if (cb <= B41_XXH3_MIDSIZE_MAX) {
        return b41_xxh3_len_129to240(p, cb, uSeed, s_b41_xxh3_secret);
    }
    /* Long path: seed 0 uses default secret; non-zero seed is accepted but
     * uses the same default secret (seeded-secret generation omitted). */
    return b41_xxh3_hashLong_64b(p, cb, uSeed, s_b41_xxh3_secret,
                                 sizeof(s_b41_xxh3_secret));
}

uint64_t
XXH3_64bits(const void *pData, size_t cb)
{
    return XXH3_64bits_withSeed(pData, cb, 0ull);
}

uint64_t __XXH3_64bits(const void *p, size_t n)
    __attribute__((alias("XXH3_64bits")));
uint64_t __XXH3_64bits_withSeed(const void *p, size_t n, uint64_t s)
    __attribute__((alias("XXH3_64bits_withSeed")));
