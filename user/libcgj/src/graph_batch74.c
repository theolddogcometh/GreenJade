/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch74: AES-CBC + AES-CTR (NIST SP 800-38A).
 * Block cipher primitives from batch45 (extern only — do not reimplement):
 *   AES_set_encrypt_key / AES_set_decrypt_key / AES_encrypt / AES_decrypt
 * Local AES_KEY layout matches OpenSSL aes.h / batch45.
 * Integer/pointer only (no SSE). Clean-room freestanding pure C public ABI.
 *
 * Surface:
 *   AES_cbc_encrypt / __AES_cbc_encrypt
 *   AES_ctr128_encrypt / __AES_ctr128_encrypt
 *   aes_cbc_encrypt / __aes_cbc_encrypt
 *   aes_cbc_decrypt / __aes_cbc_decrypt
 *   aes_ctr_xor / __aes_ctr_xor
 *   __libcgj_batch74_marker = "libcgj-batch74"
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: in/out/key/ivec/ecount NULL → cbc/ctr no-op.
 *   AES primitives from batch45 (extern). NIST SP 800-38A CBC + CTR.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch74_marker[] = "libcgj-batch74";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45/50/57. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16
#define AES_ENCRYPT 1
#define AES_DECRYPT 0

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

/* batch45 AES primitives (no local AES block implementation). */
int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);

/* ========================================================================
 * Local helpers (no libc string; freestanding / -fno-builtin safe)
 * ======================================================================== */

static void
b74_copy16(unsigned char aDst[16], const unsigned char aSrc[16])
{
    size_t i;

    for (i = 0u; i < 16u; i++) {
        aDst[i] = aSrc[i];
    }
}

static void
b74_xor16(unsigned char aDst[16], const unsigned char aA[16],
          const unsigned char aB[16])
{
    size_t i;

    for (i = 0u; i < 16u; i++) {
        aDst[i] = (unsigned char)(aA[i] ^ aB[i]);
    }
}

/*
 * Big-endian 128-bit counter increment (OpenSSL CRYPTO_ctr128_inc shape).
 * Used for NIST SP 800-38A CTR with a full-block counter.
 */
static void
b74_ctr_inc(unsigned char aCtr[16])
{
    unsigned int uCarry = 1u;
    int i;

    for (i = 15; i >= 0 && uCarry != 0u; i--) {
        unsigned int u = (unsigned int)aCtr[i] + uCarry;

        aCtr[i] = (unsigned char)u;
        uCarry = u >> 8;
    }
}

/* ========================================================================
 * AES-CBC — NIST SP 800-38A §6.2
 *
 * Encrypt: C_j = CIPH_K(P_j XOR C_{j-1}), C_0 = IV
 * Decrypt: P_j = CIPH^{-1}_K(C_j) XOR C_{j-1}, C_0 = IV
 * IV is updated in place to the last ciphertext block (OpenSSL shape).
 * Length must be a multiple of 16; non-multiples are truncated to full blocks.
 * ======================================================================== */

void
AES_cbc_encrypt(const unsigned char *in, unsigned char *out, size_t len,
                const AES_KEY *key, unsigned char *ivec, int enc)
{
    unsigned char aIv[16];
    unsigned char aTmp[16];
    unsigned char aPlain[16];
    size_t nBlocks;
    size_t iBlock;
    size_t i;

    if (in == NULL || out == NULL || key == NULL || ivec == NULL) {
        return;
    }
    if (len < 16u) {
        return;
    }

    nBlocks = len / 16u;
    b74_copy16(aIv, ivec);

    if (enc == AES_ENCRYPT) {
        for (iBlock = 0u; iBlock < nBlocks; iBlock++) {
            const unsigned char *pIn = in + (iBlock * 16u);
            unsigned char *pOut = out + (iBlock * 16u);

            b74_xor16(aTmp, pIn, aIv);
            AES_encrypt(aTmp, pOut, key);
            b74_copy16(aIv, pOut);
        }
    } else {
        /* Decrypt: keep ciphertext for chaining before writing plaintext. */
        for (iBlock = 0u; iBlock < nBlocks; iBlock++) {
            const unsigned char *pIn = in + (iBlock * 16u);
            unsigned char *pOut = out + (iBlock * 16u);

            b74_copy16(aTmp, pIn);
            AES_decrypt(pIn, aPlain, key);
            b74_xor16(pOut, aPlain, aIv);
            b74_copy16(aIv, aTmp);
        }
    }

    for (i = 0u; i < 16u; i++) {
        ivec[i] = aIv[i];
    }
}

/*
 * Simpler CBC encrypt: always AES_ENCRYPT direction.
 * IV updated in place. len multiple of 16 (full blocks only).
 */
void
aes_cbc_encrypt(const unsigned char *in, unsigned char *out, size_t len,
                const AES_KEY *key, unsigned char *ivec)
{
    AES_cbc_encrypt(in, out, len, key, ivec, AES_ENCRYPT);
}

/*
 * Simpler CBC decrypt: always AES_DECRYPT direction.
 * IV updated in place. len multiple of 16 (full blocks only).
 */
void
aes_cbc_decrypt(const unsigned char *in, unsigned char *out, size_t len,
                const AES_KEY *key, unsigned char *ivec)
{
    AES_cbc_encrypt(in, out, len, key, ivec, AES_DECRYPT);
}

/* ========================================================================
 * AES-CTR — NIST SP 800-38A §6.5
 *
 * OpenSSL AES_ctr128_encrypt shape:
 *   - ivec holds the current counter block (updated as blocks are consumed)
 *   - ecount_buf holds the encrypted counter (keystream) for partial reuse
 *   - *num is offset into ecount_buf (0..15)
 * Encrypt and decrypt are identical (XOR with keystream).
 * ======================================================================== */

void
AES_ctr128_encrypt(const unsigned char *in, unsigned char *out, size_t len,
                   const AES_KEY *key, unsigned char ivec[16],
                   unsigned char ecount_buf[16], unsigned int *num)
{
    unsigned int uOff;
    size_t i;

    if (in == NULL || out == NULL || key == NULL || ivec == NULL ||
        ecount_buf == NULL || num == NULL) {
        return;
    }

    uOff = *num;
    if (uOff >= 16u) {
        uOff = 0u;
    }

    for (i = 0u; i < len; i++) {
        if (uOff == 0u) {
            AES_encrypt(ivec, ecount_buf, key);
            b74_ctr_inc(ivec);
        }
        out[i] = (unsigned char)(in[i] ^ ecount_buf[uOff]);
        uOff++;
        if (uOff == 16u) {
            uOff = 0u;
        }
    }
    *num = uOff;
}

/*
 * Simpler one-shot CTR: starts with num=0, no external ecount_buf.
 * ivec is the initial counter and is updated as full keystream blocks
 * are produced (same counter advance as AES_ctr128_encrypt).
 */
void
aes_ctr_xor(const unsigned char *in, unsigned char *out, size_t len,
            const AES_KEY *key, unsigned char ivec[16])
{
    unsigned char aEcount[16];
    unsigned int uNum = 0u;
    size_t i;

    if (in == NULL || out == NULL || key == NULL || ivec == NULL) {
        return;
    }

    for (i = 0u; i < 16u; i++) {
        aEcount[i] = 0u;
    }
    AES_ctr128_encrypt(in, out, len, key, ivec, aEcount, &uNum);
}

/* Public __ aliases (glibc dual-symbol shape). */
void __AES_cbc_encrypt(const unsigned char *in, unsigned char *out, size_t len,
                       const AES_KEY *key, unsigned char *ivec, int enc)
    __attribute__((alias("AES_cbc_encrypt")));
void __AES_ctr128_encrypt(const unsigned char *in, unsigned char *out,
                          size_t len, const AES_KEY *key,
                          unsigned char ivec[16], unsigned char ecount_buf[16],
                          unsigned int *num)
    __attribute__((alias("AES_ctr128_encrypt")));
void __aes_cbc_encrypt(const unsigned char *in, unsigned char *out, size_t len,
                       const AES_KEY *key, unsigned char *ivec)
    __attribute__((alias("aes_cbc_encrypt")));
void __aes_cbc_decrypt(const unsigned char *in, unsigned char *out, size_t len,
                       const AES_KEY *key, unsigned char *ivec)
    __attribute__((alias("aes_cbc_decrypt")));
void __aes_ctr_xor(const unsigned char *in, unsigned char *out, size_t len,
                   const AES_KEY *key, unsigned char ivec[16])
    __attribute__((alias("aes_ctr_xor")));
