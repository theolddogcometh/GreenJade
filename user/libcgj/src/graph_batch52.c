/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch52: DES and 3DES (EDE) single-block ECB.
 * Clean-room from public DES specification (FIPS 46-3 / NIST).
 * Integer/pointer only (no SSE). Flat schedule API (uint32_t sk[32]).
 *
 * Public symbols (+ __ aliases):
 *   DES_set_key, DES_ecb_encrypt, DES3_set_key, DES3_ecb_encrypt
 * Marker: __libcgj_batch52_marker = "libcgj-batch52"
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch52_marker[] = "libcgj-batch52";

/* enc: non-zero = encrypt, zero = decrypt (OpenSSL-style). */
#define B52_DES_ENCRYPT 1
#define B52_DES_DECRYPT 0

/* ========================================================================
 * FIPS 46-3 tables (1-based bit positions in the 64-bit block / key)
 * ======================================================================== */

/* Initial Permutation */
static const unsigned char s_b52_ip[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,  60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,  64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,  59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,  63, 55, 47, 39, 31, 23, 15, 7};

/* Final Permutation (IP inverse) */
static const unsigned char s_b52_fp[64] = {
    40, 8,  48, 16, 56, 24, 64, 32, 39, 7,  47, 15, 55, 23, 63, 31,
    38, 6,  46, 14, 54, 22, 62, 30, 37, 5,  45, 13, 53, 21, 61, 29,
    36, 4,  44, 12, 52, 20, 60, 28, 35, 3,  43, 11, 51, 19, 59, 27,
    34, 2,  42, 10, 50, 18, 58, 26, 33, 1,  41, 9,  49, 17, 57, 25};

/* Expansion E: 32 -> 48 */
static const unsigned char s_b52_e[48] = {
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11,
    12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

/* P permutation after S-boxes */
static const unsigned char s_b52_p[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25};

/* PC-1: 64-bit key -> 56 bits (C||D) */
static const unsigned char s_b52_pc1[56] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};

/* PC-2: 56-bit C||D -> 48-bit subkey */
static const unsigned char s_b52_pc2[48] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,
    26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

/* Left shifts per round for C and D halves */
static const unsigned char s_b52_shifts[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

/*
 * Eight S-boxes: each maps 6-bit input to 4-bit output.
 * Indexed as S[box][row*16 + col] where row = b0||b5, col = b1..b4.
 */
static const unsigned char s_b52_sbox[8][64] = {
    {/* S1 */
     14, 4,  13, 1,  2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0,  7,
     0,  15, 7,  4,  14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3,  8,
     4,  1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5,  0,
     15, 12, 8,  2,  4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6,  13},
    {/* S2 */
     15, 1,  8,  14, 6,  11, 3,  4,  9,  7,  2,  13, 12, 0,  5,  10,
     3,  13, 4,  7,  15, 2,  8,  14, 12, 0,  1,  10, 6,  9,  11, 5,
     0,  14, 7,  11, 10, 4,  13, 1,  5,  8,  12, 6,  9,  3,  2,  15,
     13, 8,  10, 1,  3,  15, 4,  2,  11, 6,  7,  12, 0,  5,  14, 9},
    {/* S3 */
     10, 0,  9,  14, 6,  3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,
     13, 7,  0,  9,  3,  4,  6,  10, 2,  8,  5,  14, 12, 11, 15, 1,
     13, 6,  4,  9,  8,  15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,
     1,  10, 13, 0,  6,  9,  8,  7,  4,  15, 14, 3,  11, 5,  2,  12},
    {/* S4 */
     7,  13, 14, 3,  0,  6,  9,  10, 1,  2,  8,  5,  11, 12, 4,  15,
     13, 8,  11, 5,  6,  15, 0,  3,  4,  7,  2,  12, 1,  10, 14, 9,
     10, 6,  9,  0,  12, 11, 7,  13, 15, 1,  3,  14, 5,  2,  8,  4,
     3,  15, 0,  6,  10, 1,  13, 8,  9,  4,  5,  11, 12, 7,  2,  14},
    {/* S5 */
     2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0,  14, 9,
     14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9,  8,  6,
     4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3,  0,  14,
     11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4,  5,  3},
    {/* S6 */
     12, 1,  10, 15, 9,  2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11,
     10, 15, 4,  2,  7,  12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8,
     9,  14, 15, 5,  2,  8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,
     4,  3,  2,  12, 9,  5,  15, 10, 11, 14, 1,  7,  6,  0,  8,  13},
    {/* S7 */
     4,  11, 2,  14, 15, 0,  8,  13, 3,  12, 9,  7,  5,  10, 6,  1,
     13, 0,  11, 7,  4,  9,  1,  10, 14, 3,  5,  12, 2,  15, 8,  6,
     1,  4,  11, 13, 12, 3,  7,  14, 10, 15, 6,  8,  0,  5,  9,  2,
     6,  11, 13, 8,  1,  4,  10, 7,  9,  5,  0,  15, 14, 2,  3,  12},
    {/* S8 */
     13, 2,  8,  4,  6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,
     1,  15, 13, 8,  10, 3,  7,  4,  12, 5,  6,  11, 0,  14, 9,  2,
     7,  11, 4,  1,  9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,
     2,  1,  14, 7,  4,  10, 8,  13, 15, 12, 9,  0,  3,  5,  6,  11}};

/* ========================================================================
 * Bit helpers — FIPS numbers bits 1..64 with bit 1 = MSB of first byte
 * ======================================================================== */

static unsigned
b52_get_bit64(uint64_t u64Val, unsigned uBit1)
{
    /* uBit1 in 1..64; bit 1 is MSB (bit 63 of the uint64). */
    return (unsigned)((u64Val >> (64u - uBit1)) & 1u);
}

static uint64_t
b52_set_bit64(uint64_t u64Val, unsigned uBit1, unsigned uBit)
{
    unsigned uShift = 64u - uBit1;

    if (uBit) {
        u64Val |= ((uint64_t)1u << uShift);
    } else {
        u64Val &= ~((uint64_t)1u << uShift);
    }
    return u64Val;
}

static uint64_t
b52_load_be64(const unsigned char *p)
{
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b52_store_be64(unsigned char *p, uint64_t u64Val)
{
    p[0] = (unsigned char)(u64Val >> 56);
    p[1] = (unsigned char)(u64Val >> 48);
    p[2] = (unsigned char)(u64Val >> 40);
    p[3] = (unsigned char)(u64Val >> 32);
    p[4] = (unsigned char)(u64Val >> 24);
    p[5] = (unsigned char)(u64Val >> 16);
    p[6] = (unsigned char)(u64Val >> 8);
    p[7] = (unsigned char)u64Val;
}

static uint64_t
b52_permute(uint64_t u64In, const unsigned char *pTab, unsigned cBits)
{
    uint64_t u64Out = 0;
    unsigned iBit;

    for (iBit = 0; iBit < cBits; iBit++) {
        unsigned uSrc = pTab[iBit];
        unsigned uBit = b52_get_bit64(u64In, uSrc);
        /* Output bits packed into the high end of the 64-bit word. */
        u64Out = b52_set_bit64(u64Out, iBit + 1u, uBit);
    }
    return u64Out;
}

/* Rotate left a 28-bit value held in the top 28 bits of a 64-bit word. */
static uint32_t
b52_rol28(uint32_t u28, unsigned n)
{
    u28 &= 0x0fffffffu;
    return ((u28 << n) | (u28 >> (28u - n))) & 0x0fffffffu;
}

/* ========================================================================
 * Key schedule — 16 subkeys as pairs of 32-bit words (48 bits total each)
 * sk[2*r]   = upper 24 bits of subkey in low 24 bits of word
 * sk[2*r+1] = lower 24 bits of subkey in low 24 bits of word
 * ======================================================================== */

static void
b52_set_key(const unsigned char aKey[8], uint32_t aSk[32])
{
    uint64_t u64Key;
    uint64_t u64Cd;
    uint32_t uC;
    uint32_t uD;
    unsigned iRound;
    unsigned iBit;

    u64Key = b52_load_be64(aKey);
    /* PC-1 produces 56 bits in the high end of u64Cd. */
    u64Cd = b52_permute(u64Key, s_b52_pc1, 56);

    /* Split into C (bits 1..28) and D (bits 29..56) of the 56-bit value. */
    uC = 0;
    uD = 0;
    for (iBit = 0; iBit < 28u; iBit++) {
        if (b52_get_bit64(u64Cd, iBit + 1u)) {
            uC |= (1u << (27u - iBit));
        }
        if (b52_get_bit64(u64Cd, iBit + 29u)) {
            uD |= (1u << (27u - iBit));
        }
    }

    for (iRound = 0; iRound < 16u; iRound++) {
        uint64_t u64CdRound;
        uint64_t u64Sub;
        uint32_t uHi;
        uint32_t uLo;

        uC = b52_rol28(uC, s_b52_shifts[iRound]);
        uD = b52_rol28(uD, s_b52_shifts[iRound]);

        /* Rebuild 56-bit CD in high end for PC-2. */
        u64CdRound = 0;
        for (iBit = 0; iBit < 28u; iBit++) {
            if (uC & (1u << (27u - iBit))) {
                u64CdRound = b52_set_bit64(u64CdRound, iBit + 1u, 1u);
            }
            if (uD & (1u << (27u - iBit))) {
                u64CdRound = b52_set_bit64(u64CdRound, iBit + 29u, 1u);
            }
        }

        u64Sub = b52_permute(u64CdRound, s_b52_pc2, 48);
        /* High 24 of the 48-bit subkey, then low 24. */
        uHi = 0;
        uLo = 0;
        for (iBit = 0; iBit < 24u; iBit++) {
            if (b52_get_bit64(u64Sub, iBit + 1u)) {
                uHi |= (1u << (23u - iBit));
            }
            if (b52_get_bit64(u64Sub, iBit + 25u)) {
                uLo |= (1u << (23u - iBit));
            }
        }
        aSk[2u * iRound] = uHi;
        aSk[2u * iRound + 1u] = uLo;
    }
}

/* Feistel F: 32-bit R + 48-bit subkey -> 32-bit */
static uint32_t
b52_f(uint32_t uR, uint32_t uSkHi, uint32_t uSkLo)
{
    uint64_t u64R;
    uint64_t u64E;
    uint64_t u64X;
    uint32_t uS;
    uint32_t uP;
    unsigned iBox;
    unsigned iBit;

    /* Place R in high 32 bits for E (which references bits 1..32 of R). */
    u64R = ((uint64_t)uR) << 32;
    u64E = b52_permute(u64R, s_b52_e, 48);

    /* XOR with 48-bit subkey. */
    u64X = 0;
    for (iBit = 0; iBit < 24u; iBit++) {
        unsigned uEb = b52_get_bit64(u64E, iBit + 1u);
        unsigned uKb = (uSkHi >> (23u - iBit)) & 1u;
        u64X = b52_set_bit64(u64X, iBit + 1u, uEb ^ uKb);
    }
    for (iBit = 0; iBit < 24u; iBit++) {
        unsigned uEb = b52_get_bit64(u64E, iBit + 25u);
        unsigned uKb = (uSkLo >> (23u - iBit)) & 1u;
        u64X = b52_set_bit64(u64X, iBit + 25u, uEb ^ uKb);
    }

    /* Eight S-boxes: 6 bits -> 4 bits each. */
    uS = 0;
    for (iBox = 0; iBox < 8u; iBox++) {
        unsigned uSix = 0;
        unsigned uRow;
        unsigned uCol;
        unsigned uOut;
        unsigned j;

        for (j = 0; j < 6u; j++) {
            if (b52_get_bit64(u64X, iBox * 6u + j + 1u)) {
                uSix |= (1u << (5u - j));
            }
        }
        uRow = ((uSix & 0x20u) >> 4) | (uSix & 0x01u);
        uCol = (uSix >> 1) & 0x0fu;
        uOut = s_b52_sbox[iBox][uRow * 16u + uCol];
        uS = (uS << 4) | (uOut & 0x0fu);
    }

    /* P permutation on 32-bit S output. */
    {
        uint64_t u64S = ((uint64_t)uS) << 32;
        uint64_t u64P = b52_permute(u64S, s_b52_p, 32);
        uP = 0;
        for (iBit = 0; iBit < 32u; iBit++) {
            if (b52_get_bit64(u64P, iBit + 1u)) {
                uP |= (1u << (31u - iBit));
            }
        }
    }
    return uP;
}

static void
b52_crypt(const unsigned char aIn[8], unsigned char aOut[8],
          const uint32_t aSk[32], int fEnc)
{
    uint64_t u64Block;
    uint64_t u64Ip;
    uint32_t uL;
    uint32_t uR;
    unsigned iRound;
    unsigned iBit;
    uint64_t u64Pre;
    uint64_t u64Out;

    u64Block = b52_load_be64(aIn);
    u64Ip = b52_permute(u64Block, s_b52_ip, 64);

    uL = 0;
    uR = 0;
    for (iBit = 0; iBit < 32u; iBit++) {
        if (b52_get_bit64(u64Ip, iBit + 1u)) {
            uL |= (1u << (31u - iBit));
        }
        if (b52_get_bit64(u64Ip, iBit + 33u)) {
            uR |= (1u << (31u - iBit));
        }
    }

    for (iRound = 0; iRound < 16u; iRound++) {
        unsigned iK;
        uint32_t uTmp;
        uint32_t uF;

        if (fEnc) {
            iK = iRound;
        } else {
            iK = 15u - iRound;
        }
        uF = b52_f(uR, aSk[2u * iK], aSk[2u * iK + 1u]);
        uTmp = uR;
        uR = uL ^ uF;
        uL = uTmp;
    }

    /*
     * After 16 rounds the halves are swapped relative to Feistel output
     * convention: final block before FP is R16 || L16.
     */
    u64Pre = 0;
    for (iBit = 0; iBit < 32u; iBit++) {
        if (uR & (1u << (31u - iBit))) {
            u64Pre = b52_set_bit64(u64Pre, iBit + 1u, 1u);
        }
        if (uL & (1u << (31u - iBit))) {
            u64Pre = b52_set_bit64(u64Pre, iBit + 33u, 1u);
        }
    }

    u64Out = b52_permute(u64Pre, s_b52_fp, 64);
    b52_store_be64(aOut, u64Out);
}

/* ========================================================================
 * Public API
 * ======================================================================== */

int
DES_set_key(const unsigned char aKey[8], uint32_t aSk[32])
{
    if (aKey == NULL || aSk == NULL) {
        return -1;
    }
    b52_set_key(aKey, aSk);
    return 0;
}

void
DES_ecb_encrypt(const unsigned char aIn[8], unsigned char aOut[8],
                const uint32_t aSk[32], int nEnc)
{
    if (aIn == NULL || aOut == NULL || aSk == NULL) {
        return;
    }
    b52_crypt(aIn, aOut, aSk, nEnc != 0 ? B52_DES_ENCRYPT : B52_DES_DECRYPT);
}

int
DES3_set_key(const unsigned char aKey[24], uint32_t aSk1[32], uint32_t aSk2[32],
             uint32_t aSk3[32])
{
    if (aKey == NULL || aSk1 == NULL || aSk2 == NULL || aSk3 == NULL) {
        return -1;
    }
    b52_set_key(aKey, aSk1);
    b52_set_key(aKey + 8, aSk2);
    b52_set_key(aKey + 16, aSk3);
    return 0;
}

void
DES3_ecb_encrypt(const unsigned char aIn[8], unsigned char aOut[8],
                 const uint32_t aSk1[32], const uint32_t aSk2[32],
                 const uint32_t aSk3[32], int nEnc)
{
    unsigned char aTmp[8];

    if (aIn == NULL || aOut == NULL || aSk1 == NULL || aSk2 == NULL ||
        aSk3 == NULL) {
        return;
    }

    if (nEnc != 0) {
        /* EDE encrypt: E(k1) -> D(k2) -> E(k3) */
        b52_crypt(aIn, aTmp, aSk1, B52_DES_ENCRYPT);
        b52_crypt(aTmp, aTmp, aSk2, B52_DES_DECRYPT);
        b52_crypt(aTmp, aOut, aSk3, B52_DES_ENCRYPT);
    } else {
        /* EDE decrypt: D(k3) -> E(k2) -> D(k1) */
        b52_crypt(aIn, aTmp, aSk3, B52_DES_DECRYPT);
        b52_crypt(aTmp, aTmp, aSk2, B52_DES_ENCRYPT);
        b52_crypt(aTmp, aOut, aSk1, B52_DES_DECRYPT);
    }
}

/* __ aliases for dynlink / glibc-shaped graphs */
int __DES_set_key(const unsigned char aKey[8], uint32_t aSk[32])
    __attribute__((alias("DES_set_key")));
void __DES_ecb_encrypt(const unsigned char aIn[8], unsigned char aOut[8],
                       const uint32_t aSk[32], int nEnc)
    __attribute__((alias("DES_ecb_encrypt")));
int __DES3_set_key(const unsigned char aKey[24], uint32_t aSk1[32],
                   uint32_t aSk2[32], uint32_t aSk3[32])
    __attribute__((alias("DES3_set_key")));
void __DES3_ecb_encrypt(const unsigned char aIn[8], unsigned char aOut[8],
                        const uint32_t aSk1[32], const uint32_t aSk2[32],
                        const uint32_t aSk3[32], int nEnc)
    __attribute__((alias("DES3_ecb_encrypt")));
