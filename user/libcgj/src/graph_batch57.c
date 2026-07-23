/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch57: AES-CMAC-128 (NIST SP 800-38B / RFC 4493).
 * Uses AES_set_encrypt_key / AES_encrypt from batch45 (declared extern;
 * not reimplemented here). Integer/pointer only (no SSE). Clean-room
 * freestanding pure C public ABI.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch57_marker[] = "libcgj-batch57";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45/50 AES_KEY. */
#define AES_MAXNR 14

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

/* AES primitives live in batch45 (link provided by parent; not needed for -c). */
int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);

/* Rb for AES-128 CMAC subkey (NIST SP 800-38B §5.3 / RFC 4493 §2.3). */
#define B57_RB 0x87u

/* ========================================================================
 * Local helpers (no libc string; freestanding / -fno-builtin safe)
 * ======================================================================== */

static void
b57_zero16(unsigned char a[16])
{
    size_t i;

    for (i = 0u; i < 16u; i++) {
        a[i] = 0u;
    }
}

static void
b57_xor16(unsigned char aDst[16], const unsigned char aA[16],
          const unsigned char aB[16])
{
    size_t i;

    for (i = 0u; i < 16u; i++) {
        aDst[i] = (unsigned char)(aA[i] ^ aB[i]);
    }
}

/*
 * Left-shift one bit over a 128-bit big-endian block, then XOR Rb if
 * the original MSB was 1. Implements dbl() for CMAC subkey derivation.
 */
static void
b57_dbl(unsigned char aOut[16], const unsigned char aIn[16])
{
    unsigned char uMsb;
    size_t i;

    uMsb = (unsigned char)(aIn[0] & 0x80u);
    for (i = 0u; i < 15u; i++) {
        aOut[i] = (unsigned char)((aIn[i] << 1) | (aIn[i + 1u] >> 7));
    }
    aOut[15] = (unsigned char)(aIn[15] << 1);
    if (uMsb != 0u) {
        aOut[15] ^= (unsigned char)B57_RB;
    }
}

/* ========================================================================
 * AES-CMAC-128 — NIST SP 800-38B / RFC 4493
 * ======================================================================== */

void
aes_cmac(const unsigned char *key, size_t key_len, const unsigned char *msg,
         size_t msg_len, unsigned char mac[16])
{
    AES_KEY stKey;
    unsigned char aL[16];
    unsigned char aK1[16];
    unsigned char aK2[16];
    unsigned char aX[16];
    unsigned char aY[16];
    unsigned char aLast[16];
    size_t cBlocks;
    size_t cbRem;
    size_t iBlock;
    size_t iByte;
    int fComplete;
    int nRc;

    if (mac == NULL) {
        return;
    }
    if (key == NULL || key_len != 16u) {
        return;
    }
    if (msg_len > 0u && msg == NULL) {
        return;
    }

    nRc = AES_set_encrypt_key(key, 128, &stKey);
    if (nRc != 0) {
        return;
    }

    /* L = AES_K(0^128); K1 = dbl(L); K2 = dbl(K1) */
    b57_zero16(aL);
    AES_encrypt(aL, aL, &stKey);
    b57_dbl(aK1, aL);
    b57_dbl(aK2, aK1);

    /*
     * n = ceil(len/b); empty message is one incomplete block (RFC 4493).
     * Complete last block when len > 0 and len is a multiple of 16.
     */
    if (msg_len == 0u) {
        cBlocks = 1u;
        fComplete = 0;
        cbRem = 0u;
    } else {
        cBlocks = (msg_len + 15u) / 16u;
        fComplete = ((msg_len & 15u) == 0u) ? 1 : 0;
        cbRem = msg_len & 15u;
    }

    /* Process full blocks 1 .. n-1: X_i = AES_K(X_{i-1} XOR M_i) */
    b57_zero16(aX);
    for (iBlock = 0u; iBlock + 1u < cBlocks; iBlock++) {
        b57_xor16(aY, aX, msg + (iBlock * 16u));
        AES_encrypt(aY, aX, &stKey);
    }

    /* Form M_last with K1 (complete) or padded || K2 (incomplete). */
    if (fComplete != 0) {
        b57_xor16(aLast, msg + ((cBlocks - 1u) * 16u), aK1);
    } else {
        b57_zero16(aLast);
        for (iByte = 0u; iByte < cbRem; iByte++) {
            aLast[iByte] = msg[((cBlocks - 1u) * 16u) + iByte];
        }
        aLast[cbRem] = 0x80u;
        b57_xor16(aLast, aLast, aK2);
    }

    b57_xor16(aY, aX, aLast);
    AES_encrypt(aY, mac, &stKey);
}

/* Fixed 16-byte key convenience wrapper. */
void
AES_CMAC(const unsigned char *key, const unsigned char *msg, size_t msg_len,
         unsigned char mac[16])
{
    aes_cmac(key, 16u, msg, msg_len, mac);
}

void __aes_cmac(const unsigned char *key, size_t key_len,
                const unsigned char *msg, size_t msg_len, unsigned char mac[16])
    __attribute__((alias("aes_cmac")));

void __AES_CMAC(const unsigned char *key, const unsigned char *msg,
                size_t msg_len, unsigned char mac[16])
    __attribute__((alias("AES_CMAC")));
