/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch61: CityHash64 / CityHash128.
 * Clean-room from the public Google CityHash algorithm description
 * (Pike / Alakuijala). Integer/pointer only (no SSE). No multi-def with
 * batch36 siphash24 — this file exports cityhash* only.
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: data NULL with len!=0 → defined soft zero result path.
 *   CityHash64/128 pure integer port; does not redefine siphash/xxhash.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch61_marker[] = "libcgj-batch61";

/* CityHash magic primes (public algorithm constants). */
#define B61_K0 0xc3a5c85c97cb3127ull
#define B61_K1 0xb492b66fbe98f273ull
#define B61_K2 0x9ae16a3b2f90404full
#define B61_K_MUL 0x9ddfea08eb382d69ull

/* CityHash Rotate is a right-rotate (public algorithm). */
static uint64_t
b61_rotr64(uint64_t x, unsigned n)
{
    n &= 63u;
    if (n == 0u) {
        return x;
    }
    return (x >> n) | (x << (64u - n));
}

static uint64_t
b61_bswap64(uint64_t x)
{
    return ((x & 0x00000000000000ffull) << 56) |
           ((x & 0x000000000000ff00ull) << 40) |
           ((x & 0x0000000000ff0000ull) << 24) |
           ((x & 0x00000000ff000000ull) << 8) |
           ((x & 0x000000ff00000000ull) >> 8) |
           ((x & 0x0000ff0000000000ull) >> 24) |
           ((x & 0x00ff000000000000ull) >> 40) |
           ((x & 0xff00000000000000ull) >> 56);
}

static uint64_t
b61_shiftmix(uint64_t v)
{
    return v ^ (v >> 47);
}

static uint32_t
b61_fetch32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t
b61_fetch64(const unsigned char *p)
{
    return (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) | ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) | ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

/* Hash 128 bits down to 64 (Murmur-inspired). */
static uint64_t
b61_hash128to64(uint64_t lo, uint64_t hi)
{
    uint64_t a = (lo ^ hi) * B61_K_MUL;

    a ^= a >> 47;
    {
        uint64_t b = (hi ^ a) * B61_K_MUL;

        b ^= b >> 47;
        b *= B61_K_MUL;
        return b;
    }
}

static uint64_t
b61_hashlen16(uint64_t u, uint64_t v)
{
    return b61_hash128to64(u, v);
}

static uint64_t
b61_hashlen16_mul(uint64_t u, uint64_t v, uint64_t mul)
{
    uint64_t a = (u ^ v) * mul;

    a ^= a >> 47;
    {
        uint64_t b = (v ^ a) * mul;

        b ^= b >> 47;
        b *= mul;
        return b;
    }
}

static uint64_t
b61_hashlen0to16(const unsigned char *s, size_t len)
{
    if (len >= 8u) {
        uint64_t mul = B61_K2 + len * 2u;
        uint64_t a = b61_fetch64(s) + B61_K2;
        uint64_t b = b61_fetch64(s + len - 8u);
        uint64_t c = b61_rotr64(b, 37) * mul + a;
        uint64_t d = (b61_rotr64(a, 25) + b) * mul;

        return b61_hashlen16_mul(c, d, mul);
    }
    if (len >= 4u) {
        uint64_t mul = B61_K2 + len * 2u;
        uint64_t a = b61_fetch32(s);

        return b61_hashlen16_mul(len + (a << 3), b61_fetch32(s + len - 4u),
                                 mul);
    }
    if (len > 0u) {
        uint8_t a = s[0];
        uint8_t b = s[len >> 1];
        uint8_t c = s[len - 1u];
        uint32_t y = (uint32_t)a + ((uint32_t)b << 8);
        uint32_t z = (uint32_t)len + ((uint32_t)c << 2);

        return b61_shiftmix((uint64_t)y * B61_K2 ^ (uint64_t)z * B61_K0) *
               B61_K2;
    }
    return B61_K2;
}

static uint64_t
b61_hashlen17to32(const unsigned char *s, size_t len)
{
    uint64_t mul = B61_K2 + len * 2u;
    uint64_t a = b61_fetch64(s) * B61_K1;
    uint64_t b = b61_fetch64(s + 8);
    uint64_t c = b61_fetch64(s + len - 8u) * mul;
    uint64_t d = b61_fetch64(s + len - 16u) * B61_K2;

    return b61_hashlen16_mul(b61_rotr64(a + b, 43) + b61_rotr64(c, 30) + d,
                             a + b61_rotr64(b + B61_K2, 18) + c, mul);
}

static uint64_t
b61_hashlen33to64(const unsigned char *s, size_t len)
{
    uint64_t mul = B61_K2 + len * 2u;
    uint64_t a = b61_fetch64(s) * B61_K2;
    uint64_t b = b61_fetch64(s + 8);
    uint64_t c = b61_fetch64(s + len - 24u);
    uint64_t d = b61_fetch64(s + len - 32u);
    uint64_t e = b61_fetch64(s + 16) * B61_K2;
    uint64_t f = b61_fetch64(s + 24) * 9ull;
    uint64_t g = b61_fetch64(s + len - 8u);
    uint64_t h = b61_fetch64(s + len - 16u) * mul;
    uint64_t u = b61_rotr64(a + g, 43) + (b61_rotr64(b, 30) + c) * 9ull;
    uint64_t v = ((a + g) ^ d) + f + 1ull;
    uint64_t w = b61_bswap64((u + v) * mul) + h;
    uint64_t x = b61_rotr64(e + f, 42) + c;
    uint64_t y = (b61_bswap64((v + w) * mul) + g) * mul;
    uint64_t z = e + f + c;

    a = b61_bswap64((x + z) * mul + y) + b;
    b = b61_shiftmix((z + a) * mul + d + h) * mul;
    return b + x;
}

/* Weak 32-byte hash; returns (first, second) via out[0], out[1]. */
static void
b61_weak_hash32_seeds(uint64_t w, uint64_t x, uint64_t y, uint64_t z,
                      uint64_t a, uint64_t b, uint64_t out[2])
{
    a += w;
    b = b61_rotr64(b + a + z, 21);
    {
        uint64_t c = a;

        a += x;
        a += y;
        b += b61_rotr64(a, 44);
        out[0] = a + z;
        out[1] = b + c;
    }
}

static void
b61_weak_hash32_seeds_s(const unsigned char *s, uint64_t a, uint64_t b,
                        uint64_t out[2])
{
    b61_weak_hash32_seeds(b61_fetch64(s), b61_fetch64(s + 8),
                          b61_fetch64(s + 16), b61_fetch64(s + 24), a, b,
                          out);
}

/*
 * CityHash64 — public algorithm for all lengths.
 * s may be NULL only when len == 0.
 */
uint64_t
cityhash64(const char *s, size_t len)
{
    const unsigned char *p = (const unsigned char *)s;
    uint64_t x, y, z;
    uint64_t v[2], w[2];
    size_t n;

    if (p == NULL && len != 0u) {
        return 0ull;
    }
    if (len <= 32u) {
        if (len <= 16u) {
            return b61_hashlen0to16(p, len);
        }
        return b61_hashlen17to32(p, len);
    }
    if (len <= 64u) {
        return b61_hashlen33to64(p, len);
    }

    /* len > 64: hash end first, keep 56 bytes of state. */
    x = b61_fetch64(p + len - 40u);
    y = b61_fetch64(p + len - 16u) + b61_fetch64(p + len - 56u);
    z = b61_hashlen16(b61_fetch64(p + len - 48u) + len,
                      b61_fetch64(p + len - 24u));
    b61_weak_hash32_seeds_s(p + len - 64u, len, z, v);
    b61_weak_hash32_seeds_s(p + len - 32u, y + B61_K1, x, w);
    x = x * B61_K1 + b61_fetch64(p);

    /* Nearest lower multiple of 64 (from len-1). */
    n = (len - 1u) & ~(size_t)63u;
    do {
        x = b61_rotr64(x + y + v[0] + b61_fetch64(p + 8), 37) * B61_K1;
        y = b61_rotr64(y + v[1] + b61_fetch64(p + 48), 42) * B61_K1;
        x ^= w[1];
        y += v[0] + b61_fetch64(p + 40);
        z = b61_rotr64(z + w[0], 33) * B61_K1;
        b61_weak_hash32_seeds_s(p, v[1] * B61_K1, x + w[0], v);
        b61_weak_hash32_seeds_s(p + 32, z + w[1], y + b61_fetch64(p + 16), w);
        {
            uint64_t t = z;

            z = x;
            x = t;
        }
        p += 64;
        n -= 64u;
    } while (n != 0u);

    return b61_hashlen16(b61_hashlen16(v[0], w[0]) + b61_shiftmix(y) * B61_K1 + z,
                         b61_hashlen16(v[1], w[1]) + x);
}

/* ---- CityHash128 -------------------------------------------------------- */

static void
b61_citymurmur(const unsigned char *s, size_t len, uint64_t seed0,
               uint64_t seed1, uint64_t out[2])
{
    uint64_t a = seed0;
    uint64_t b = seed1;
    uint64_t c = 0;
    uint64_t d = 0;
    /* signed: algorithm uses len - 16 and loops while l > 0 */
    long l = (long)len - 16;

    if (l <= 0) { /* len <= 16 */
        a = b61_shiftmix(a * B61_K1) * B61_K1;
        c = b * B61_K1 + b61_hashlen0to16(s, len);
        d = b61_shiftmix(a + (len >= 8u ? b61_fetch64(s) : c));
    } else {
        c = b61_hashlen16(b61_fetch64(s + len - 8u) + B61_K1, a);
        d = b61_hashlen16(b + len, c + b61_fetch64(s + len - 16u));
        a += d;
        do {
            a ^= b61_shiftmix(b61_fetch64(s) * B61_K1) * B61_K1;
            a *= B61_K1;
            b ^= a;
            c ^= b61_shiftmix(b61_fetch64(s + 8) * B61_K1) * B61_K1;
            c *= B61_K1;
            d ^= c;
            s += 16;
            l -= 16;
        } while (l > 0);
    }
    a = b61_hashlen16(a, c);
    b = b61_hashlen16(d, b);
    out[0] = a ^ b;
    out[1] = b61_hashlen16(b, a);
}

static void
b61_cityhash128_with_seed(const unsigned char *s, size_t len, uint64_t seed0,
                          uint64_t seed1, uint64_t out[2])
{
    uint64_t v[2], w[2];
    uint64_t x, y, z;
    size_t tail_done;

    if (len < 128u) {
        b61_citymurmur(s, len, seed0, seed1, out);
        return;
    }

    /* len >= 128: keep 56 bytes of state (v, w, x, y, z). */
    x = seed0;
    y = seed1;
    z = len * B61_K1;
    v[0] = b61_rotr64(y ^ B61_K1, 49) * B61_K1 + b61_fetch64(s);
    v[1] = b61_rotr64(v[0], 42) * B61_K1 + b61_fetch64(s + 8);
    w[0] = b61_rotr64(y + z, 35) * B61_K1 + x;
    w[1] = b61_rotr64(x + b61_fetch64(s + 88), 53) * B61_K1;

    /* Same inner loop as CityHash64, manually unrolled (2 x 64 bytes). */
    do {
        x = b61_rotr64(x + y + v[0] + b61_fetch64(s + 8), 37) * B61_K1;
        y = b61_rotr64(y + v[1] + b61_fetch64(s + 48), 42) * B61_K1;
        x ^= w[1];
        y += v[0] + b61_fetch64(s + 40);
        z = b61_rotr64(z + w[0], 33) * B61_K1;
        b61_weak_hash32_seeds_s(s, v[1] * B61_K1, x + w[0], v);
        b61_weak_hash32_seeds_s(s + 32, z + w[1], y + b61_fetch64(s + 16), w);
        {
            uint64_t t = z;

            z = x;
            x = t;
        }
        s += 64;
        x = b61_rotr64(x + y + v[0] + b61_fetch64(s + 8), 37) * B61_K1;
        y = b61_rotr64(y + v[1] + b61_fetch64(s + 48), 42) * B61_K1;
        x ^= w[1];
        y += v[0] + b61_fetch64(s + 40);
        z = b61_rotr64(z + w[0], 33) * B61_K1;
        b61_weak_hash32_seeds_s(s, v[1] * B61_K1, x + w[0], v);
        b61_weak_hash32_seeds_s(s + 32, z + w[1], y + b61_fetch64(s + 16), w);
        {
            uint64_t t = z;

            z = x;
            x = t;
        }
        s += 64;
        len -= 128u;
    } while (len >= 128u);

    x += b61_rotr64(v[0] + z, 49) * B61_K0;
    y = y * B61_K0 + b61_rotr64(w[1], 37);
    z = z * B61_K0 + b61_rotr64(w[0], 27);
    w[0] *= 9ull;
    v[0] *= B61_K0;

    /* If 0 < len < 128, hash up to 4 chunks of 32 bytes from the end of s. */
    for (tail_done = 0u; tail_done < len;) {
        tail_done += 32u;
        y = b61_rotr64(x + y, 42) * B61_K0 + v[1];
        w[0] += b61_fetch64(s + len - tail_done + 16u);
        x = x * B61_K0 + w[0];
        z += w[1] + b61_fetch64(s + len - tail_done);
        w[1] += v[0];
        b61_weak_hash32_seeds_s(s + len - tail_done, v[0] + z, v[1], v);
        v[0] *= B61_K0;
    }

    x = b61_hashlen16(x, v[0]);
    y = b61_hashlen16(y + z, w[0]);
    out[0] = b61_hashlen16(x + v[1], w[1]) + y;
    out[1] = b61_hashlen16(x + w[1], y + v[1]);
}

void
cityhash128(const char *s, size_t len, uint64_t out[2])
{
    const unsigned char *p = (const unsigned char *)s;

    if (out == NULL) {
        return;
    }
    if (p == NULL && len != 0u) {
        out[0] = 0ull;
        out[1] = 0ull;
        return;
    }
    if (len >= 16u) {
        b61_cityhash128_with_seed(p + 16, len - 16u, b61_fetch64(p),
                                  b61_fetch64(p + 8) + B61_K0, out);
    } else {
        b61_cityhash128_with_seed(p, len, B61_K0, B61_K1, out);
    }
}

uint64_t __cityhash64(const char *s, size_t len)
    __attribute__((alias("cityhash64")));
void __cityhash128(const char *s, size_t len, uint64_t out[2])
    __attribute__((alias("cityhash128")));
