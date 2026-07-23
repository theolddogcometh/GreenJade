/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch64: X25519 (Curve25519 Diffie-Hellman) per
 * RFC 7748. Freestanding integer-only Montgomery ladder; no SSE/FP.
 * Clean-room public ABI (algorithm from RFC 7748 §5).
 *
 * Surface:
 *   x25519(out, scalar, point)  — RFC 7748 X25519
 *   __x25519                     — alias
 *   __libcgj_batch64_marker
 *
 * Ed25519 (create_keypair / sign / verify) intentionally omitted here;
 * full twisted-Edwards ops are left for a later batch. X25519 alone is
 * the complete, correct ECDH primitive for this TU.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch64_marker[] = "libcgj-batch64";

/* --------------------------------------------------------------------------
 * Field arithmetic over GF(2^255 - 19)
 *
 * Representation: five unsigned 64-bit limbs, little-endian, each holding
 * at most 51 bits after a carry pass (except temporarily during mul/sqr).
 *   v = l0 + 2^51*l1 + 2^102*l2 + 2^153*l3 + 2^204*l4
 * -------------------------------------------------------------------------- */

typedef struct {
    uint64_t l[5];
} b64_fe;

/* Load little-endian 32 bytes into a field element (no reduction yet). */
static void
b64_fe_frombytes(b64_fe *h, const unsigned char s[32])
{
    uint64_t t0, t1, t2, t3;

    t0 = (uint64_t)s[0] | ((uint64_t)s[1] << 8) | ((uint64_t)s[2] << 16) |
         ((uint64_t)s[3] << 24) | ((uint64_t)s[4] << 32) |
         ((uint64_t)s[5] << 40) | ((uint64_t)s[6] << 48) |
         ((uint64_t)s[7] << 56);
    t1 = (uint64_t)s[8] | ((uint64_t)s[9] << 8) | ((uint64_t)s[10] << 16) |
         ((uint64_t)s[11] << 24) | ((uint64_t)s[12] << 32) |
         ((uint64_t)s[13] << 40) | ((uint64_t)s[14] << 48) |
         ((uint64_t)s[15] << 56);
    t2 = (uint64_t)s[16] | ((uint64_t)s[17] << 8) | ((uint64_t)s[18] << 16) |
         ((uint64_t)s[19] << 24) | ((uint64_t)s[20] << 32) |
         ((uint64_t)s[21] << 40) | ((uint64_t)s[22] << 48) |
         ((uint64_t)s[23] << 56);
    t3 = (uint64_t)s[24] | ((uint64_t)s[25] << 8) | ((uint64_t)s[26] << 16) |
         ((uint64_t)s[27] << 24) | ((uint64_t)s[28] << 32) |
         ((uint64_t)s[29] << 40) | ((uint64_t)s[30] << 48) |
         ((uint64_t)s[31] << 56);

    /* RFC 7748: ignore the most significant bit of the final byte. */
    t3 &= 0x7fffffffffffffffull;

    h->l[0] = t0 & 0x7ffffffffffffull;
    h->l[1] = ((t0 >> 51) | (t1 << 13)) & 0x7ffffffffffffull;
    h->l[2] = ((t1 >> 38) | (t2 << 26)) & 0x7ffffffffffffull;
    h->l[3] = ((t2 >> 25) | (t3 << 39)) & 0x7ffffffffffffull;
    h->l[4] = (t3 >> 12) & 0x7ffffffffffffull;
}

/* Reduce to canonical form and store little-endian 32 bytes. */
static void
b64_fe_tobytes(unsigned char s[32], const b64_fe *h)
{
    uint64_t t0, t1, t2, t3, t4;
    uint64_t q;
    int i;

    t0 = h->l[0];
    t1 = h->l[1];
    t2 = h->l[2];
    t3 = h->l[3];
    t4 = h->l[4];

    /* Two carry passes to bound limbs. */
    for (i = 0; i < 2; i++) {
        t1 += t0 >> 51;
        t0 &= 0x7ffffffffffffull;
        t2 += t1 >> 51;
        t1 &= 0x7ffffffffffffull;
        t3 += t2 >> 51;
        t2 &= 0x7ffffffffffffull;
        t4 += t3 >> 51;
        t3 &= 0x7ffffffffffffull;
        t0 += 19ull * (t4 >> 51);
        t4 &= 0x7ffffffffffffull;
    }

    /* Conditional subtract p if h >= p. */
    q = (t0 + 19ull) >> 51;
    q = (t1 + q) >> 51;
    q = (t2 + q) >> 51;
    q = (t3 + q) >> 51;
    q = (t4 + q) >> 51;

    t0 += 19ull * q;
    t1 += t0 >> 51;
    t0 &= 0x7ffffffffffffull;
    t2 += t1 >> 51;
    t1 &= 0x7ffffffffffffull;
    t3 += t2 >> 51;
    t2 &= 0x7ffffffffffffull;
    t4 += t3 >> 51;
    t3 &= 0x7ffffffffffffull;
    t4 &= 0x7ffffffffffffull;

    s[0] = (unsigned char)t0;
    s[1] = (unsigned char)(t0 >> 8);
    s[2] = (unsigned char)(t0 >> 16);
    s[3] = (unsigned char)(t0 >> 24);
    s[4] = (unsigned char)(t0 >> 32);
    s[5] = (unsigned char)(t0 >> 40);
    s[6] = (unsigned char)((t0 >> 48) | (t1 << 3));
    s[7] = (unsigned char)(t1 >> 5);
    s[8] = (unsigned char)(t1 >> 13);
    s[9] = (unsigned char)(t1 >> 21);
    s[10] = (unsigned char)(t1 >> 29);
    s[11] = (unsigned char)(t1 >> 37);
    s[12] = (unsigned char)((t1 >> 45) | (t2 << 6));
    s[13] = (unsigned char)(t2 >> 2);
    s[14] = (unsigned char)(t2 >> 10);
    s[15] = (unsigned char)(t2 >> 18);
    s[16] = (unsigned char)(t2 >> 26);
    s[17] = (unsigned char)(t2 >> 34);
    s[18] = (unsigned char)(t2 >> 42);
    s[19] = (unsigned char)((t2 >> 50) | (t3 << 1));
    s[20] = (unsigned char)(t3 >> 7);
    s[21] = (unsigned char)(t3 >> 15);
    s[22] = (unsigned char)(t3 >> 23);
    s[23] = (unsigned char)(t3 >> 31);
    s[24] = (unsigned char)(t3 >> 39);
    s[25] = (unsigned char)((t3 >> 47) | (t4 << 4));
    s[26] = (unsigned char)(t4 >> 4);
    s[27] = (unsigned char)(t4 >> 12);
    s[28] = (unsigned char)(t4 >> 20);
    s[29] = (unsigned char)(t4 >> 28);
    s[30] = (unsigned char)(t4 >> 36);
    s[31] = (unsigned char)(t4 >> 44);
}

static void
b64_fe_copy(b64_fe *h, const b64_fe *f)
{
    h->l[0] = f->l[0];
    h->l[1] = f->l[1];
    h->l[2] = f->l[2];
    h->l[3] = f->l[3];
    h->l[4] = f->l[4];
}

static void
b64_fe_0(b64_fe *h)
{
    h->l[0] = 0;
    h->l[1] = 0;
    h->l[2] = 0;
    h->l[3] = 0;
    h->l[4] = 0;
}

static void
b64_fe_1(b64_fe *h)
{
    h->l[0] = 1;
    h->l[1] = 0;
    h->l[2] = 0;
    h->l[3] = 0;
    h->l[4] = 0;
}

/* Constant-time conditional swap: if b==1 swap(f,g); if b==0 no-op. */
static void
b64_fe_cswap(b64_fe *f, b64_fe *g, unsigned int b)
{
    uint64_t mask = (uint64_t)0 - (uint64_t)b;
    uint64_t x;
    int i;

    for (i = 0; i < 5; i++) {
        x = mask & (f->l[i] ^ g->l[i]);
        f->l[i] ^= x;
        g->l[i] ^= x;
    }
}

static void
b64_fe_add(b64_fe *h, const b64_fe *f, const b64_fe *g)
{
    h->l[0] = f->l[0] + g->l[0];
    h->l[1] = f->l[1] + g->l[1];
    h->l[2] = f->l[2] + g->l[2];
    h->l[3] = f->l[3] + g->l[3];
    h->l[4] = f->l[4] + g->l[4];
}

static void
b64_fe_sub(b64_fe *h, const b64_fe *f, const b64_fe *g)
{
    /*
     * Add 2p so limbs stay non-negative before reduction.
     * 2p limb layout (51-bit): l0 = 2*(2^51-19), li = 2*(2^51-1).
     */
    uint64_t t0, t1, t2, t3, t4;

    t0 = (f->l[0] + 0xfffffffffffdaull) - g->l[0];
    t1 = (f->l[1] + 0xffffffffffffeull) - g->l[1];
    t2 = (f->l[2] + 0xffffffffffffeull) - g->l[2];
    t3 = (f->l[3] + 0xffffffffffffeull) - g->l[3];
    t4 = (f->l[4] + 0xffffffffffffeull) - g->l[4];

    t1 += t0 >> 51;
    t0 &= 0x7ffffffffffffull;
    t2 += t1 >> 51;
    t1 &= 0x7ffffffffffffull;
    t3 += t2 >> 51;
    t2 &= 0x7ffffffffffffull;
    t4 += t3 >> 51;
    t3 &= 0x7ffffffffffffull;
    t0 += 19ull * (t4 >> 51);
    t4 &= 0x7ffffffffffffull;
    t1 += t0 >> 51;
    t0 &= 0x7ffffffffffffull;

    h->l[0] = t0;
    h->l[1] = t1;
    h->l[2] = t2;
    h->l[3] = t3;
    h->l[4] = t4;
}

static void
b64_fe_mul(b64_fe *h, const b64_fe *f, const b64_fe *g)
{
    uint64_t f0 = f->l[0], f1 = f->l[1], f2 = f->l[2], f3 = f->l[3],
             f4 = f->l[4];
    uint64_t g0 = g->l[0], g1 = g->l[1], g2 = g->l[2], g3 = g->l[3],
             g4 = g->l[4];
    uint64_t f1_19 = f1 * 19ull;
    uint64_t f2_19 = f2 * 19ull;
    uint64_t f3_19 = f3 * 19ull;
    uint64_t f4_19 = f4 * 19ull;
    __uint128_t r0, r1, r2, r3, r4;
    uint64_t t0, t1, t2, t3, t4;
    uint64_t c;

    r0 = (__uint128_t)f0 * g0 + (__uint128_t)f1_19 * g4 +
         (__uint128_t)f2_19 * g3 + (__uint128_t)f3_19 * g2 +
         (__uint128_t)f4_19 * g1;
    r1 = (__uint128_t)f0 * g1 + (__uint128_t)f1 * g0 +
         (__uint128_t)f2_19 * g4 + (__uint128_t)f3_19 * g3 +
         (__uint128_t)f4_19 * g2;
    r2 = (__uint128_t)f0 * g2 + (__uint128_t)f1 * g1 + (__uint128_t)f2 * g0 +
         (__uint128_t)f3_19 * g4 + (__uint128_t)f4_19 * g3;
    r3 = (__uint128_t)f0 * g3 + (__uint128_t)f1 * g2 + (__uint128_t)f2 * g1 +
         (__uint128_t)f3 * g0 + (__uint128_t)f4_19 * g4;
    r4 = (__uint128_t)f0 * g4 + (__uint128_t)f1 * g3 + (__uint128_t)f2 * g2 +
         (__uint128_t)f3 * g1 + (__uint128_t)f4 * g0;

    t0 = (uint64_t)r0 & 0x7ffffffffffffull;
    c = (uint64_t)(r0 >> 51);
    r1 += c;
    t1 = (uint64_t)r1 & 0x7ffffffffffffull;
    c = (uint64_t)(r1 >> 51);
    r2 += c;
    t2 = (uint64_t)r2 & 0x7ffffffffffffull;
    c = (uint64_t)(r2 >> 51);
    r3 += c;
    t3 = (uint64_t)r3 & 0x7ffffffffffffull;
    c = (uint64_t)(r3 >> 51);
    r4 += c;
    t4 = (uint64_t)r4 & 0x7ffffffffffffull;
    c = (uint64_t)(r4 >> 51);
    t0 += c * 19ull;
    t1 += t0 >> 51;
    t0 &= 0x7ffffffffffffull;
    t2 += t1 >> 51; /* may leave t1 slightly oversized; ladder tolerates */
    t1 &= 0x7ffffffffffffull;

    h->l[0] = t0;
    h->l[1] = t1;
    h->l[2] = t2;
    h->l[3] = t3;
    h->l[4] = t4;
}

static void
b64_fe_sq(b64_fe *h, const b64_fe *f)
{
    b64_fe_mul(h, f, f);
}

static void
b64_fe_mul121666(b64_fe *h, const b64_fe *f)
{
    __uint128_t r0, r1, r2, r3, r4;
    uint64_t t0, t1, t2, t3, t4;
    uint64_t c;

    r0 = (__uint128_t)f->l[0] * 121666ull;
    r1 = (__uint128_t)f->l[1] * 121666ull;
    r2 = (__uint128_t)f->l[2] * 121666ull;
    r3 = (__uint128_t)f->l[3] * 121666ull;
    r4 = (__uint128_t)f->l[4] * 121666ull;

    t0 = (uint64_t)r0 & 0x7ffffffffffffull;
    c = (uint64_t)(r0 >> 51);
    r1 += c;
    t1 = (uint64_t)r1 & 0x7ffffffffffffull;
    c = (uint64_t)(r1 >> 51);
    r2 += c;
    t2 = (uint64_t)r2 & 0x7ffffffffffffull;
    c = (uint64_t)(r2 >> 51);
    r3 += c;
    t3 = (uint64_t)r3 & 0x7ffffffffffffull;
    c = (uint64_t)(r3 >> 51);
    r4 += c;
    t4 = (uint64_t)r4 & 0x7ffffffffffffull;
    c = (uint64_t)(r4 >> 51);
    t0 += c * 19ull;
    t1 += t0 >> 51;
    t0 &= 0x7ffffffffffffull;

    h->l[0] = t0;
    h->l[1] = t1;
    h->l[2] = t2;
    h->l[3] = t3;
    h->l[4] = t4;
}

/* Invert via Fermat: a^(p-2) = a^(2^255-21). Chain matches ref10/RFC. */
static void
b64_fe_invert(b64_fe *out, const b64_fe *z)
{
    b64_fe t0, t1, t2, t3;
    int i;

    b64_fe_sq(&t0, z); /* 2^1 */
    b64_fe_sq(&t1, &t0);
    b64_fe_sq(&t1, &t1); /* 2^3 */
    b64_fe_mul(&t1, z, &t1);
    b64_fe_mul(&t0, &t0, &t1);
    b64_fe_sq(&t2, &t0);
    b64_fe_mul(&t1, &t1, &t2);
    b64_fe_sq(&t2, &t1);
    for (i = 1; i < 5; i++) {
        b64_fe_sq(&t2, &t2);
    }
    b64_fe_mul(&t1, &t2, &t1);
    b64_fe_sq(&t2, &t1);
    for (i = 1; i < 10; i++) {
        b64_fe_sq(&t2, &t2);
    }
    b64_fe_mul(&t2, &t2, &t1);
    b64_fe_sq(&t3, &t2);
    for (i = 1; i < 20; i++) {
        b64_fe_sq(&t3, &t3);
    }
    b64_fe_mul(&t2, &t3, &t2);
    b64_fe_sq(&t2, &t2);
    for (i = 1; i < 10; i++) {
        b64_fe_sq(&t2, &t2);
    }
    b64_fe_mul(&t1, &t2, &t1);
    b64_fe_sq(&t2, &t1);
    for (i = 1; i < 50; i++) {
        b64_fe_sq(&t2, &t2);
    }
    b64_fe_mul(&t2, &t2, &t1);
    b64_fe_sq(&t3, &t2);
    for (i = 1; i < 100; i++) {
        b64_fe_sq(&t3, &t3);
    }
    b64_fe_mul(&t2, &t3, &t2);
    b64_fe_sq(&t2, &t2);
    for (i = 1; i < 50; i++) {
        b64_fe_sq(&t2, &t2);
    }
    b64_fe_mul(&t1, &t2, &t1);
    b64_fe_sq(&t1, &t1);
    for (i = 1; i < 5; i++) {
        b64_fe_sq(&t1, &t1);
    }
    b64_fe_mul(out, &t1, &t0);
}

/* --------------------------------------------------------------------------
 * X25519 — RFC 7748 Montgomery ladder
 * -------------------------------------------------------------------------- */

void
x25519(unsigned char out[32], const unsigned char scalar[32],
       const unsigned char point[32])
{
    unsigned char e[32];
    b64_fe x1, x2, z2, x3, z3, tmp0, tmp1;
    unsigned int swap;
    int pos;
    unsigned int b;
    int i;

    for (i = 0; i < 32; i++) {
        e[i] = scalar[i];
    }
    /* decodeScalar25519 */
    e[0] &= 248u;
    e[31] &= 127u;
    e[31] |= 64u;

    b64_fe_frombytes(&x1, point);
    b64_fe_1(&x2);
    b64_fe_0(&z2);
    b64_fe_copy(&x3, &x1);
    b64_fe_1(&z3);

    swap = 0;
    for (pos = 254; pos >= 0; pos--) {
        b = (unsigned int)(e[pos / 8] >> (pos & 7)) & 1u;
        swap ^= b;
        b64_fe_cswap(&x2, &x3, swap);
        b64_fe_cswap(&z2, &z3, swap);
        swap = b;

        b64_fe_sub(&tmp0, &x3, &z3);
        b64_fe_sub(&tmp1, &x2, &z2);
        b64_fe_add(&x2, &x2, &z2);
        b64_fe_add(&z2, &x3, &z3);
        b64_fe_mul(&z3, &tmp0, &x2);
        b64_fe_mul(&z2, &z2, &tmp1);
        b64_fe_sq(&tmp0, &tmp1);
        b64_fe_sq(&tmp1, &x2);
        b64_fe_add(&x3, &z3, &z2);
        b64_fe_sub(&z2, &z3, &z2);
        b64_fe_mul(&x2, &tmp1, &tmp0);
        b64_fe_sub(&tmp1, &tmp1, &tmp0);
        b64_fe_sq(&z2, &z2);
        b64_fe_mul121666(&z3, &tmp1);
        b64_fe_sq(&x3, &x3);
        b64_fe_add(&tmp0, &tmp0, &z3);
        b64_fe_mul(&z3, &x1, &z2);
        b64_fe_mul(&z2, &tmp1, &tmp0);
    }
    b64_fe_cswap(&x2, &x3, swap);
    b64_fe_cswap(&z2, &z3, swap);

    b64_fe_invert(&z2, &z2);
    b64_fe_mul(&x2, &x2, &z2);
    b64_fe_tobytes(out, &x2);

    /* Best-effort wipe of clamped scalar copy. */
    for (i = 0; i < 32; i++) {
        e[i] = 0;
    }
}

void __x25519(unsigned char out[32], const unsigned char scalar[32],
              const unsigned char point[32])
    __attribute__((alias("x25519")));
