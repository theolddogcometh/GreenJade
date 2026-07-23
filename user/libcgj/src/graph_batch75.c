/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch75: SipHash key-first variants + HalfSipHash.
 * Clean-room pure C freestanding — integer/pointer only (no SSE).
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   siphash24 / siphash24_buf / __siphash24  → graph_batch36.c
 *     signature: (const void *data, size_t len, const unsigned char key[16])
 *
 * This TU adds only symbols that were missing:
 *   siphash24_with_key / __siphash24_with_key
 *     uint64_t (const unsigned char key[16], const void *data, size_t len)
 *   siphash13_with_key / __siphash13_with_key
 *     uint64_t (const unsigned char key[16], const void *data, size_t len)
 *   halfsiphash24 / __halfsiphash24
 *     uint32_t (const unsigned char key[8], const void *data, size_t len)
 *   __libcgj_batch75_marker = "libcgj-batch75"
 *
 * Algorithms (public):
 *   SipHash-c-d  — Aumasson / Bernstein (c compression, d finalization rounds)
 *   HalfSipHash-2-4 — 32-bit word variant, 64-bit key, 32-bit tag
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: key NULL → zero key; data NULL with len!=0 → tag 0.
 *   Does not redefine siphash24 (batch36). Key-first + HalfSipHash only.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch75_marker[] = "libcgj-batch75";

/* --------------------------------------------------------------------------
 * Load helpers (little-endian, partial OK)
 * -------------------------------------------------------------------------- */

static uint64_t
b75_load_le64(const unsigned char *p, size_t cb)
{
    uint64_t u = 0;
    size_t i;

    for (i = 0; i < cb && i < 8u; i++) {
        u |= (uint64_t)p[i] << (8u * i);
    }
    return u;
}

static uint32_t
b75_load_le32(const unsigned char *p, size_t cb)
{
    uint32_t u = 0;
    size_t i;

    for (i = 0; i < cb && i < 4u; i++) {
        u |= (uint32_t)p[i] << (8u * i);
    }
    return u;
}

static uint64_t
b75_rol64(uint64_t x, unsigned n)
{
    return (x << n) | (x >> (64u - n));
}

static uint32_t
b75_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

/* --------------------------------------------------------------------------
 * SipHash-c-d core (64-bit words, 128-bit key)
 * -------------------------------------------------------------------------- */

static uint64_t
b75_siphash_cd(const unsigned char aKey[16], const unsigned char *pIn,
               size_t cb, unsigned cRounds, unsigned dRounds)
{
    uint64_t k0 = b75_load_le64(aKey, 8u);
    uint64_t k1 = b75_load_le64(aKey + 8, 8u);
    uint64_t v0 = 0x736f6d6570736575ull ^ k0;
    uint64_t v1 = 0x646f72616e646f6dull ^ k1;
    uint64_t v2 = 0x6c7967656e657261ull ^ k0;
    uint64_t v3 = 0x7465646279746573ull ^ k1;
    uint64_t b;
    size_t nLeft = cb;
    const unsigned char *p = pIn;
    unsigned r;

#define B75_SIPROUND                                                          \
    do {                                                                      \
        v0 += v1;                                                             \
        v1 = b75_rol64(v1, 13);                                               \
        v1 ^= v0;                                                             \
        v0 = b75_rol64(v0, 32);                                               \
        v2 += v3;                                                             \
        v3 = b75_rol64(v3, 16);                                               \
        v3 ^= v2;                                                             \
        v0 += v3;                                                             \
        v3 = b75_rol64(v3, 21);                                               \
        v3 ^= v0;                                                             \
        v2 += v1;                                                             \
        v1 = b75_rol64(v1, 17);                                               \
        v1 ^= v2;                                                             \
        v2 = b75_rol64(v2, 32);                                               \
    } while (0)

    while (nLeft >= 8u) {
        uint64_t m = b75_load_le64(p, 8u);

        v3 ^= m;
        for (r = 0; r < cRounds; r++) {
            B75_SIPROUND;
        }
        v0 ^= m;
        p += 8u;
        nLeft -= 8u;
    }

    b = ((uint64_t)cb) << 56;
    b |= b75_load_le64(p, nLeft);
    v3 ^= b;
    for (r = 0; r < cRounds; r++) {
        B75_SIPROUND;
    }
    v0 ^= b;

    v2 ^= 0xffull;
    for (r = 0; r < dRounds; r++) {
        B75_SIPROUND;
    }
#undef B75_SIPROUND

    return v0 ^ v1 ^ v2 ^ v3;
}

/*
 * siphash24_with_key — SipHash-2-4, key first (unlike batch36 siphash24).
 * NULL key → 16 zero bytes. NULL data with len != 0 → 0.
 */
uint64_t
siphash24_with_key(const unsigned char key[16], const void *data, size_t len)
{
    static const unsigned char aZero[16];

    if (key == NULL) {
        key = aZero;
    }
    if (data == NULL && len != 0u) {
        return 0;
    }
    return b75_siphash_cd(key, (const unsigned char *)data, len, 2u, 4u);
}

/*
 * siphash13_with_key — SipHash-1-3 (faster reduced-round variant), key first.
 */
uint64_t
siphash13_with_key(const unsigned char key[16], const void *data, size_t len)
{
    static const unsigned char aZero[16];

    if (key == NULL) {
        key = aZero;
    }
    if (data == NULL && len != 0u) {
        return 0;
    }
    return b75_siphash_cd(key, (const unsigned char *)data, len, 1u, 3u);
}

uint64_t
__siphash24_with_key(const unsigned char key[16], const void *data, size_t len)
    __attribute__((alias("siphash24_with_key")));

uint64_t
__siphash13_with_key(const unsigned char key[16], const void *data, size_t len)
    __attribute__((alias("siphash13_with_key")));

/* --------------------------------------------------------------------------
 * HalfSipHash-2-4 (32-bit words, 64-bit key, 32-bit output)
 * -------------------------------------------------------------------------- */

static uint32_t
b75_halfsiphash24(const unsigned char aKey[8], const unsigned char *pIn,
                  size_t cb)
{
    uint32_t k0 = b75_load_le32(aKey, 4u);
    uint32_t k1 = b75_load_le32(aKey + 4, 4u);
    uint32_t v0 = k0;
    uint32_t v1 = k1;
    uint32_t v2 = 0x6c796765u ^ k0;
    uint32_t v3 = 0x74656462u ^ k1;
    uint32_t b;
    size_t nLeft = cb;
    const unsigned char *p = pIn;

#define B75_HSIPROUND                                                         \
    do {                                                                      \
        v0 += v1;                                                             \
        v1 = b75_rol32(v1, 5);                                                \
        v1 ^= v0;                                                             \
        v0 = b75_rol32(v0, 16);                                               \
        v2 += v3;                                                             \
        v3 = b75_rol32(v3, 8);                                                \
        v3 ^= v2;                                                             \
        v0 += v3;                                                             \
        v3 = b75_rol32(v3, 7);                                                \
        v3 ^= v0;                                                             \
        v2 += v1;                                                             \
        v1 = b75_rol32(v1, 13);                                               \
        v1 ^= v2;                                                             \
        v2 = b75_rol32(v2, 16);                                               \
    } while (0)

    while (nLeft >= 4u) {
        uint32_t m = b75_load_le32(p, 4u);

        v3 ^= m;
        B75_HSIPROUND;
        B75_HSIPROUND;
        v0 ^= m;
        p += 4u;
        nLeft -= 4u;
    }

    b = ((uint32_t)cb) << 24;
    b |= b75_load_le32(p, nLeft);
    v3 ^= b;
    B75_HSIPROUND;
    B75_HSIPROUND;
    v0 ^= b;

    v2 ^= 0xffu;
    B75_HSIPROUND;
    B75_HSIPROUND;
    B75_HSIPROUND;
    B75_HSIPROUND;
#undef B75_HSIPROUND

    return v1 ^ v3;
}

/*
 * halfsiphash24 — HalfSipHash-2-4, key first (8-byte key), 32-bit tag.
 * NULL key → 8 zero bytes. NULL data with len != 0 → 0.
 */
uint32_t
halfsiphash24(const unsigned char key[8], const void *data, size_t len)
{
    static const unsigned char aZero[8];

    if (key == NULL) {
        key = aZero;
    }
    if (data == NULL && len != 0u) {
        return 0;
    }
    return b75_halfsiphash24(key, (const unsigned char *)data, len);
}

uint32_t
__halfsiphash24(const unsigned char key[8], const void *data, size_t len)
    __attribute__((alias("halfsiphash24")));
