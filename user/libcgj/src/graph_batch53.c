/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch53: SM4 block cipher (128-bit block, 128-bit key).
 * Clean-room implementation from public GM/T 0002-2012 / GB/T 32907 parameters
 * (S-box, FK, CK, T/T' linear transforms). Integer/pointer only (no SSE).
 *
 * GB/T 32907 sample (public; for independent verification, not executed here):
 *   key        = 01 23 45 67 89 AB CD EF FE DC BA 98 76 54 32 10
 *   plaintext  = 01 23 45 67 89 AB CD EF FE DC BA 98 76 54 32 10
 *   ciphertext = 68 1E DF 34 D2 06 96 5E 86 B3 E9 4F 53 6E 42 46
 *   (1e6 enc)  = 59 52 98 C7 C6 FD 27 1F 04 02 F8 04 C3 3D 3F 66
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch53_marker[] = "libcgj-batch53";

/* ---- helpers ------------------------------------------------------------ */

static uint32_t
b53_load_be32(const unsigned char *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void
b53_store_be32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)(x >> 24);
    p[1] = (unsigned char)(x >> 16);
    p[2] = (unsigned char)(x >> 8);
    p[3] = (unsigned char)x;
}

static uint32_t
b53_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

/* ---- SM4 tables (public standard constants) ---------------------------- */

/* S-box Sbox(a) — GM/T 0002-2012 / GB/T 32907 Table 1 */
static const unsigned char s_b53_sbox[256] = {
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2,
    0x28, 0xfb, 0x2c, 0x05, 0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
    0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99, 0x9c, 0x42, 0x50, 0xf4,
    0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa,
    0x75, 0x8f, 0x3f, 0xa6, 0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
    0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8, 0x68, 0x6b, 0x81, 0xb2,
    0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b,
    0x01, 0x21, 0x78, 0x87, 0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
    0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e, 0xea, 0xbf, 0x8a, 0xd2,
    0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30,
    0xf5, 0x8c, 0xb1, 0xe3, 0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
    0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f, 0xd5, 0xdb, 0x37, 0x45,
    0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41,
    0x1f, 0x10, 0x5a, 0xd8, 0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
    0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0, 0x89, 0x69, 0x97, 0x4a,
    0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e,
    0xd7, 0xcb, 0x39, 0x48};

/* System parameter FK — GB/T 32907 §6.4.1 */
static const uint32_t s_b53_fk[4] = {
    0xa3b1bac6u, 0x56aa3350u, 0x677d9197u, 0xb27022dcu};

/* Fixed parameter CK_i — GB/T 32907 §6.4.2 (i = 0..31) */
static const uint32_t s_b53_ck[32] = {
    0x00070e15u, 0x1c232a31u, 0x383f464du, 0x545b6269u, 0x70777e85u,
    0x8c939aa1u, 0xa8afb6bdu, 0xc4cbd2d9u, 0xe0e7eef5u, 0xfc030a11u,
    0x181f262du, 0x343b4249u, 0x50575e65u, 0x6c737a81u, 0x888f969du,
    0xa4abb2b9u, 0xc0c7ced5u, 0xdce3eaf1u, 0xf8ff060du, 0x141b2229u,
    0x30373e45u, 0x4c535a61u, 0x686f767du, 0x848b9299u, 0xa0a7aeb5u,
    0xbcc3cad1u, 0xd8dfe6edu, 0xf4fb0209u, 0x10171e25u, 0x2c333a41u,
    0x484f565du, 0x646b7279u};

/* Nonlinear τ: S-box on each byte of a 32-bit word. */
static uint32_t
b53_tau(uint32_t a)
{
    return ((uint32_t)s_b53_sbox[(a >> 24) & 0xffu] << 24) |
           ((uint32_t)s_b53_sbox[(a >> 16) & 0xffu] << 16) |
           ((uint32_t)s_b53_sbox[(a >> 8) & 0xffu] << 8) |
           (uint32_t)s_b53_sbox[a & 0xffu];
}

/* Linear L for round function: B ⊕ (B<<<2) ⊕ (B<<<10) ⊕ (B<<<18) ⊕ (B<<<24) */
static uint32_t
b53_L(uint32_t b)
{
    return b ^ b53_rol32(b, 2) ^ b53_rol32(b, 10) ^ b53_rol32(b, 18) ^
           b53_rol32(b, 24);
}

/* Linear L' for key expansion: B ⊕ (B<<<13) ⊕ (B<<<23) */
static uint32_t
b53_Lp(uint32_t b)
{
    return b ^ b53_rol32(b, 13) ^ b53_rol32(b, 23);
}

/* T = L ∘ τ  (encryption round transform) */
static uint32_t
b53_T(uint32_t x)
{
    return b53_L(b53_tau(x));
}

/* T' = L' ∘ τ  (key-expansion transform) */
static uint32_t
b53_Tp(uint32_t x)
{
    return b53_Lp(b53_tau(x));
}

/*
 * One Feistel round over four 32-bit words using round key rk_i.
 * X_{i+4} = X_i ⊕ T(X_{i+1} ⊕ X_{i+2} ⊕ X_{i+3} ⊕ rk_i)
 */
static void
b53_round(uint32_t *x0, uint32_t *x1, uint32_t *x2, uint32_t *x3, uint32_t rk)
{
    uint32_t t = b53_T(*x1 ^ *x2 ^ *x3 ^ rk);
    uint32_t x4 = *x0 ^ t;
    *x0 = *x1;
    *x1 = *x2;
    *x2 = *x3;
    *x3 = x4;
}

/* ---- public API -------------------------------------------------------- */

/*
 * Expand 128-bit key into 32 round keys (encryption order).
 * Decrypt uses the same schedule in reverse (see SM4_decrypt).
 */
void
SM4_set_key(const unsigned char key[16], uint32_t rk[32])
{
    uint32_t k[4];
    unsigned i;

    if (key == NULL || rk == NULL) {
        return;
    }

    k[0] = b53_load_be32(key) ^ s_b53_fk[0];
    k[1] = b53_load_be32(key + 4) ^ s_b53_fk[1];
    k[2] = b53_load_be32(key + 8) ^ s_b53_fk[2];
    k[3] = b53_load_be32(key + 12) ^ s_b53_fk[3];

    for (i = 0; i < 32u; i++) {
        uint32_t t = k[1] ^ k[2] ^ k[3] ^ s_b53_ck[i];
        uint32_t ki = k[0] ^ b53_Tp(t);
        rk[i] = ki;
        k[0] = k[1];
        k[1] = k[2];
        k[2] = k[3];
        k[3] = ki;
    }
}

void
SM4_encrypt(const unsigned char in[16], unsigned char out[16],
            const uint32_t rk[32])
{
    uint32_t x0, x1, x2, x3;
    unsigned i;

    if (in == NULL || out == NULL || rk == NULL) {
        return;
    }

    x0 = b53_load_be32(in);
    x1 = b53_load_be32(in + 4);
    x2 = b53_load_be32(in + 8);
    x3 = b53_load_be32(in + 12);

    for (i = 0; i < 32u; i++) {
        b53_round(&x0, &x1, &x2, &x3, rk[i]);
    }

    /* Reverse transformation R: (Y0,Y1,Y2,Y3) = (X35,X34,X33,X32) */
    b53_store_be32(out, x3);
    b53_store_be32(out + 4, x2);
    b53_store_be32(out + 8, x1);
    b53_store_be32(out + 12, x0);
}

void
SM4_decrypt(const unsigned char in[16], unsigned char out[16],
            const uint32_t rk[32])
{
    uint32_t x0, x1, x2, x3;
    unsigned i;

    if (in == NULL || out == NULL || rk == NULL) {
        return;
    }

    x0 = b53_load_be32(in);
    x1 = b53_load_be32(in + 4);
    x2 = b53_load_be32(in + 8);
    x3 = b53_load_be32(in + 12);

    /* Same structure as encrypt; round keys in reverse order. */
    for (i = 0; i < 32u; i++) {
        b53_round(&x0, &x1, &x2, &x3, rk[31u - i]);
    }

    b53_store_be32(out, x3);
    b53_store_be32(out + 4, x2);
    b53_store_be32(out + 8, x1);
    b53_store_be32(out + 12, x0);
}

/*
 * Optional multi-block ECB helper.
 * enc != 0: encrypt; enc == 0: decrypt.
 * nblocks is the number of 16-byte blocks; in/out may alias (in-place).
 */
void
sm4_crypt_ecb(const uint32_t rk[32], int enc, size_t nblocks,
              const unsigned char *in, unsigned char *out)
{
    size_t i;
    unsigned char tmp[16];
    const unsigned char *pIn;
    unsigned char *pOut;

    if (rk == NULL || in == NULL || out == NULL) {
        return;
    }

    for (i = 0; i < nblocks; i++) {
        pIn = in + (i * 16u);
        pOut = out + (i * 16u);
        if (enc) {
            SM4_encrypt(pIn, tmp, rk);
        } else {
            SM4_decrypt(pIn, tmp, rk);
        }
        pOut[0] = tmp[0];
        pOut[1] = tmp[1];
        pOut[2] = tmp[2];
        pOut[3] = tmp[3];
        pOut[4] = tmp[4];
        pOut[5] = tmp[5];
        pOut[6] = tmp[6];
        pOut[7] = tmp[7];
        pOut[8] = tmp[8];
        pOut[9] = tmp[9];
        pOut[10] = tmp[10];
        pOut[11] = tmp[11];
        pOut[12] = tmp[12];
        pOut[13] = tmp[13];
        pOut[14] = tmp[14];
        pOut[15] = tmp[15];
    }
}

/* ---- underscored aliases ----------------------------------------------- */

void __SM4_set_key(const unsigned char key[16], uint32_t rk[32])
    __attribute__((alias("SM4_set_key")));
void __SM4_encrypt(const unsigned char in[16], unsigned char out[16],
                   const uint32_t rk[32])
    __attribute__((alias("SM4_encrypt")));
void __SM4_decrypt(const unsigned char in[16], unsigned char out[16],
                   const uint32_t rk[32])
    __attribute__((alias("SM4_decrypt")));
void __sm4_crypt_ecb(const uint32_t rk[32], int enc, size_t nblocks,
                     const unsigned char *in, unsigned char *out)
    __attribute__((alias("sm4_crypt_ecb")));
