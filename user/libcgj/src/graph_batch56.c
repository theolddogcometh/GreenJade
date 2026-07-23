/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch56: Salsa20 stream cipher (20 rounds default).
 * Clean-room freestanding pure C from the public Bernstein Salsa20
 * specification (cr.yp.to/snuffle/spec.pdf, 2005.04.27). Integer/pointer
 * only (no SSE, no doubles, no GPL). Does not define ChaCha/XChaCha symbols
 * (those live in batch42/43).
 *
 * Public ABI:
 *   salsa20_keysetup / salsa20_keysetup128 — fill 16-word state
 *   salsa20_block — one 64-byte keystream block at counter
 *   salsa20_xor   — XOR keystream over in → out from counter
 *
 * State layout (words, little-endian bytes in key/nonce):
 *   x[0]=c0, x[1..4]=k0, x[5]=c1, x[6..7]=nonce, x[8..9]=counter,
 *   x[10]=c2, x[11..14]=k1, x[15]=c3
 * 32-byte key: c = "expand 32-byte k" (σ)
 * 16-byte key: c = "expand 16-byte k" (τ); k repeated as k0 and k1
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch56_marker[] = "libcgj-batch56";

/* σ: "expand 32-byte k" as four little-endian words */
#define B56_SIGMA0 0x61707865u /* "expa" */
#define B56_SIGMA1 0x3320646eu /* "nd 3" */
#define B56_SIGMA2 0x79622d32u /* "2-by" */
#define B56_SIGMA3 0x6b206574u /* "te k" */

/* τ: "expand 16-byte k" */
#define B56_TAU0 0x61707865u /* "expa" */
#define B56_TAU1 0x3120646eu /* "nd 1" */
#define B56_TAU2 0x79622d36u /* "6-by" */
#define B56_TAU3 0x6b206574u /* "te k" */

/* Default Salsa20: 20 rounds = 10 double-rounds */
#define B56_ROUNDS 20u

/* ---- little-endian helpers ---------------------------------------------- */

static uint32_t
b56_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b56_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

static uint32_t
b56_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

/*
 * Salsa20 core: 20 rounds (10× columnround then rowround), then add original.
 * Matches Bernstein §5–§8 and the salsa20/8 core shape used by scrypt (RFC 7914).
 */
static void
b56_salsa20_core(const uint32_t aIn[16], unsigned char aOut[64])
{
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;
    unsigned i;

    x0 = aIn[0];
    x1 = aIn[1];
    x2 = aIn[2];
    x3 = aIn[3];
    x4 = aIn[4];
    x5 = aIn[5];
    x6 = aIn[6];
    x7 = aIn[7];
    x8 = aIn[8];
    x9 = aIn[9];
    x10 = aIn[10];
    x11 = aIn[11];
    x12 = aIn[12];
    x13 = aIn[13];
    x14 = aIn[14];
    x15 = aIn[15];

    for (i = 0u; i < B56_ROUNDS; i += 2u) {
        /* columnround */
        x4 ^= b56_rol32(x0 + x12, 7);
        x8 ^= b56_rol32(x4 + x0, 9);
        x12 ^= b56_rol32(x8 + x4, 13);
        x0 ^= b56_rol32(x12 + x8, 18);

        x9 ^= b56_rol32(x5 + x1, 7);
        x13 ^= b56_rol32(x9 + x5, 9);
        x1 ^= b56_rol32(x13 + x9, 13);
        x5 ^= b56_rol32(x1 + x13, 18);

        x14 ^= b56_rol32(x10 + x6, 7);
        x2 ^= b56_rol32(x14 + x10, 9);
        x6 ^= b56_rol32(x2 + x14, 13);
        x10 ^= b56_rol32(x6 + x2, 18);

        x3 ^= b56_rol32(x15 + x11, 7);
        x7 ^= b56_rol32(x3 + x15, 9);
        x11 ^= b56_rol32(x7 + x3, 13);
        x15 ^= b56_rol32(x11 + x7, 18);

        /* rowround */
        x1 ^= b56_rol32(x0 + x3, 7);
        x2 ^= b56_rol32(x1 + x0, 9);
        x3 ^= b56_rol32(x2 + x1, 13);
        x0 ^= b56_rol32(x3 + x2, 18);

        x6 ^= b56_rol32(x5 + x4, 7);
        x7 ^= b56_rol32(x6 + x5, 9);
        x4 ^= b56_rol32(x7 + x6, 13);
        x5 ^= b56_rol32(x4 + x7, 18);

        x11 ^= b56_rol32(x10 + x9, 7);
        x8 ^= b56_rol32(x11 + x10, 9);
        x9 ^= b56_rol32(x8 + x11, 13);
        x10 ^= b56_rol32(x9 + x8, 18);

        x12 ^= b56_rol32(x15 + x14, 7);
        x13 ^= b56_rol32(x12 + x15, 9);
        x14 ^= b56_rol32(x13 + x12, 13);
        x15 ^= b56_rol32(x14 + x13, 18);
    }

    b56_store_le32(aOut + 0, x0 + aIn[0]);
    b56_store_le32(aOut + 4, x1 + aIn[1]);
    b56_store_le32(aOut + 8, x2 + aIn[2]);
    b56_store_le32(aOut + 12, x3 + aIn[3]);
    b56_store_le32(aOut + 16, x4 + aIn[4]);
    b56_store_le32(aOut + 20, x5 + aIn[5]);
    b56_store_le32(aOut + 24, x6 + aIn[6]);
    b56_store_le32(aOut + 28, x7 + aIn[7]);
    b56_store_le32(aOut + 32, x8 + aIn[8]);
    b56_store_le32(aOut + 36, x9 + aIn[9]);
    b56_store_le32(aOut + 40, x10 + aIn[10]);
    b56_store_le32(aOut + 44, x11 + aIn[11]);
    b56_store_le32(aOut + 48, x12 + aIn[12]);
    b56_store_le32(aOut + 52, x13 + aIn[13]);
    b56_store_le32(aOut + 56, x14 + aIn[14]);
    b56_store_le32(aOut + 60, x15 + aIn[15]);
}

/* ========================================================================
 * Key setup — constants + key + 8-byte nonce; counter words left 0
 * ======================================================================== */

void
salsa20_keysetup(uint32_t x[16], const unsigned char key[32],
                 const unsigned char nonce[8])
{
    if (x == NULL || key == NULL || nonce == NULL) {
        return;
    }

    x[0] = B56_SIGMA0;
    x[1] = b56_load_le32(key + 0);
    x[2] = b56_load_le32(key + 4);
    x[3] = b56_load_le32(key + 8);
    x[4] = b56_load_le32(key + 12);
    x[5] = B56_SIGMA1;
    x[6] = b56_load_le32(nonce + 0);
    x[7] = b56_load_le32(nonce + 4);
    x[8] = 0u;
    x[9] = 0u;
    x[10] = B56_SIGMA2;
    x[11] = b56_load_le32(key + 16);
    x[12] = b56_load_le32(key + 20);
    x[13] = b56_load_le32(key + 24);
    x[14] = b56_load_le32(key + 28);
    x[15] = B56_SIGMA3;
}

void
salsa20_keysetup128(uint32_t x[16], const unsigned char key[16],
                    const unsigned char nonce[8])
{
    if (x == NULL || key == NULL || nonce == NULL) {
        return;
    }

    x[0] = B56_TAU0;
    x[1] = b56_load_le32(key + 0);
    x[2] = b56_load_le32(key + 4);
    x[3] = b56_load_le32(key + 8);
    x[4] = b56_load_le32(key + 12);
    x[5] = B56_TAU1;
    x[6] = b56_load_le32(nonce + 0);
    x[7] = b56_load_le32(nonce + 4);
    x[8] = 0u;
    x[9] = 0u;
    x[10] = B56_TAU2;
    /* 16-byte key repeated in both key slots (Bernstein §9) */
    x[11] = x[1];
    x[12] = x[2];
    x[13] = x[3];
    x[14] = x[4];
    x[15] = B56_TAU3;
}

/* ========================================================================
 * Block + stream XOR
 * ======================================================================== */

void
salsa20_block(unsigned char out[64], const uint32_t x[16], uint64_t counter)
{
    uint32_t aSt[16];
    unsigned i;

    if (out == NULL || x == NULL) {
        return;
    }

    for (i = 0u; i < 16u; i++) {
        aSt[i] = x[i];
    }
    /* Position i = little-endian 64-bit counter in words 8 and 9 (§10) */
    aSt[8] = (uint32_t)counter;
    aSt[9] = (uint32_t)(counter >> 32);
    b56_salsa20_core(aSt, out);
}

void
salsa20_xor(unsigned char *out, const unsigned char *in, size_t len,
            const uint32_t x[16], uint64_t counter)
{
    unsigned char aBlock[64];
    size_t nOff;

    if (x == NULL) {
        return;
    }
    if (len == 0u) {
        return;
    }
    if (out == NULL || in == NULL) {
        return;
    }

    nOff = 0u;
    while (nOff < len) {
        size_t nTake;
        size_t i;

        salsa20_block(aBlock, x, counter);
        nTake = len - nOff;
        if (nTake > 64u) {
            nTake = 64u;
        }
        for (i = 0u; i < nTake; i++) {
            out[nOff + i] = in[nOff + i] ^ aBlock[i];
        }
        nOff += nTake;
        counter++;
    }
}

void __salsa20_keysetup(uint32_t x[16], const unsigned char key[32],
                        const unsigned char nonce[8])
    __attribute__((alias("salsa20_keysetup")));
void __salsa20_keysetup128(uint32_t x[16], const unsigned char key[16],
                           const unsigned char nonce[8])
    __attribute__((alias("salsa20_keysetup128")));
void __salsa20_block(unsigned char out[64], const uint32_t x[16],
                     uint64_t counter)
    __attribute__((alias("salsa20_block")));
void __salsa20_xor(unsigned char *out, const unsigned char *in, size_t len,
                   const uint32_t x[16], uint64_t counter)
    __attribute__((alias("salsa20_xor")));
