/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch62: AES-XCBC-MAC-96 (RFC 3566).
 * Clean-room pure C, freestanding, integer/pointer only (no SSE).
 * AES block cipher primitives come from batch45:
 *   AES_set_encrypt_key / AES_encrypt (extern — do not reimplement).
 * AES_CMAC is owned by batch57 — not defined here.
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: key/out NULL → no-op; msg NULL only OK when len==0.
 *   AES-XCBC-MAC-96 truncates to 12 octets; AES from batch45 (extern).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch62_marker[] = "libcgj-batch62";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — must match batch45/50. */
#define AES_MAXNR 14

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

/* batch45 AES primitives (no local AES implementation). */
int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);

/* ========================================================================
 * Local helpers (no libc string dependency — freestanding safe)
 * ======================================================================== */

static void
b62_zero16(unsigned char *p)
{
    unsigned i;

    for (i = 0; i < 16u; i++) {
        p[i] = 0;
    }
}

static void
b62_xor16(unsigned char *d, const unsigned char *a, const unsigned char *b)
{
    unsigned i;

    for (i = 0; i < 16u; i++) {
        d[i] = (unsigned char)(a[i] ^ b[i]);
    }
}

static void
b62_fill16(unsigned char *p, unsigned char v)
{
    unsigned i;

    for (i = 0; i < 16u; i++) {
        p[i] = v;
    }
}

static void
b62_wipe(unsigned char *p, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++) {
        p[i] = 0;
    }
}

/* ========================================================================
 * AES-XCBC-MAC-96 (RFC 3566 §4)
 *
 * K1 = AES_K(0x01…01), K2 = AES_K(0x02…02), K3 = AES_K(0x03…03)
 * E[0] = 0
 * For i = 1..n-1: E[i] = AES_K1(M[i] ⊕ E[i-1])
 * Last block M[n]:
 *   full 128-bit:  E[n] = AES_K1(M[n] ⊕ E[n-1] ⊕ K2)
 *   short / empty: pad 10* then E[n] = AES_K1(M[n] ⊕ E[n-1] ⊕ K3)
 * Authenticator = leftmost 96 bits of E[n].
 * ======================================================================== */

void
aes_xcbc_mac_96(const unsigned char key[16], const unsigned char *msg,
                size_t len, unsigned char out[12])
{
    AES_KEY k_sched;
    AES_KEY k1_sched;
    unsigned char k1[16];
    unsigned char k2[16];
    unsigned char k3[16];
    unsigned char e[16];
    unsigned char t[16];
    unsigned char block[16];
    const unsigned char *p;
    size_t full;
    size_t rem;
    size_t b;
    unsigned i;

    if (key == NULL || out == NULL) {
        return;
    }
    if (msg == NULL && len != 0) {
        return;
    }

    /* Schedule secret key K (AES-128 only per RFC 3566 §4.1). */
    if (AES_set_encrypt_key(key, 128, &k_sched) != 0) {
        return;
    }

    /* Derive K1, K2, K3 from K. */
    b62_fill16(t, 0x01);
    AES_encrypt(t, k1, &k_sched);
    b62_fill16(t, 0x02);
    AES_encrypt(t, k2, &k_sched);
    b62_fill16(t, 0x03);
    AES_encrypt(t, k3, &k_sched);

    if (AES_set_encrypt_key(k1, 128, &k1_sched) != 0) {
        b62_wipe(k1, 16);
        b62_wipe(k2, 16);
        b62_wipe(k3, 16);
        return;
    }

    /* E[0] = 0^128 */
    b62_zero16(e);

    full = len / 16u;
    rem = len % 16u;
    p = msg;

    if (len == 0) {
        /*
         * Empty message (RFC 3566 NOTE1): single padded block with K3.
         * M[1] = 0x80 || 0^120
         */
        b62_zero16(block);
        block[0] = 0x80;
        b62_xor16(t, e, block);
        b62_xor16(t, t, k3);
        AES_encrypt(t, e, &k1_sched);
    } else if (rem == 0) {
        /* n full blocks; last uses K2, earlier use plain CBC with K1. */
        for (b = 0; b + 1u < full; b++) {
            b62_xor16(t, e, p);
            AES_encrypt(t, e, &k1_sched);
            p += 16;
        }
        b62_xor16(t, e, p);
        b62_xor16(t, t, k2);
        AES_encrypt(t, e, &k1_sched);
    } else {
        /* full complete blocks, then short final block padded 10* + K3. */
        for (b = 0; b < full; b++) {
            b62_xor16(t, e, p);
            AES_encrypt(t, e, &k1_sched);
            p += 16;
        }
        b62_zero16(block);
        for (i = 0; i < (unsigned)rem; i++) {
            block[i] = p[i];
        }
        block[rem] = 0x80;
        b62_xor16(t, e, block);
        b62_xor16(t, t, k3);
        AES_encrypt(t, e, &k1_sched);
    }

    /* Truncate to 96 bits (leftmost 12 octets). */
    for (i = 0; i < 12u; i++) {
        out[i] = e[i];
    }

    b62_wipe(k1, 16);
    b62_wipe(k2, 16);
    b62_wipe(k3, 16);
    b62_wipe(e, 16);
    b62_wipe(t, 16);
    b62_wipe(block, 16);
}

void AES_XCBC_MAC_96(const unsigned char key[16], const unsigned char *msg,
                     size_t len, unsigned char out[12])
    __attribute__((alias("aes_xcbc_mac_96")));

void __aes_xcbc_mac_96(const unsigned char key[16], const unsigned char *msg,
                       size_t len, unsigned char out[12])
    __attribute__((alias("aes_xcbc_mac_96")));

void __AES_XCBC_MAC_96(const unsigned char key[16], const unsigned char *msg,
                       size_t len, unsigned char out[12])
    __attribute__((alias("aes_xcbc_mac_96")));
