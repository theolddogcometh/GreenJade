/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43: ChaCha20 encrypt wrapper, HChaCha20, and
 * XChaCha20 (RFC 8439 + XChaCha). Poly1305 and chacha20_xor live in
 * batch42 — skipped here to avoid multi-def. Clean-room freestanding pure C:
 * integer/pointer only (no SSE, no doubles, no GPL).
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const char __libcgj_batch43_marker[] = "libcgj-batch43";

/* chacha20_xor from batch42 (RFC 8439 §2.4) */
int chacha20_xor(const unsigned char aKey[32], const unsigned char aNonce[12],
                 uint32_t uCounter, const unsigned char *pIn,
                 unsigned char *pOut, size_t cb);

/* ---- little-endian helpers ---------------------------------------------- */

static uint32_t
b43_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b43_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

static uint32_t
b43_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

/* ========================================================================
 * ChaCha quarter-round + HChaCha20 core (shared with XChaCha20)
 * ======================================================================== */

static void
b43_chacha_qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a += *b;
    *d ^= *a;
    *d = b43_rol32(*d, 16);
    *c += *d;
    *b ^= *c;
    *b = b43_rol32(*b, 12);
    *a += *b;
    *d ^= *a;
    *d = b43_rol32(*d, 8);
    *c += *d;
    *b ^= *c;
    *b = b43_rol32(*b, 7);
}

/*
 * chacha20_encrypt: stream-cipher encrypt (== XOR keystream). Distinct
 * public name for graphs that probe encrypt rather than xor.
 */
int
chacha20_encrypt(const unsigned char aKey[32], const unsigned char aNonce[12],
                 uint32_t uCounter, const unsigned char *pIn,
                 unsigned char *pOut, size_t cb)
{
    return chacha20_xor(aKey, aNonce, uCounter, pIn, pOut, cb);
}

/*
 * hchacha20: IETF XChaCha §2.2 — 32-byte subkey from key + 16-byte nonce.
 * Output is state words 0..3 and 12..15 after 20 rounds (no final add).
 */
int
hchacha20(const unsigned char aKey[32], const unsigned char aNonce[16],
          unsigned char aOut[32])
{
    uint32_t x[16];
    unsigned i;

    if (aKey == NULL || aNonce == NULL || aOut == NULL) {
        return -1;
    }
    /* "expand 32-byte k" */
    x[0] = 0x61707865u;
    x[1] = 0x3320646eu;
    x[2] = 0x79622d32u;
    x[3] = 0x6b206574u;
    x[4] = b43_load_le32(aKey + 0);
    x[5] = b43_load_le32(aKey + 4);
    x[6] = b43_load_le32(aKey + 8);
    x[7] = b43_load_le32(aKey + 12);
    x[8] = b43_load_le32(aKey + 16);
    x[9] = b43_load_le32(aKey + 20);
    x[10] = b43_load_le32(aKey + 24);
    x[11] = b43_load_le32(aKey + 28);
    x[12] = b43_load_le32(aNonce + 0);
    x[13] = b43_load_le32(aNonce + 4);
    x[14] = b43_load_le32(aNonce + 8);
    x[15] = b43_load_le32(aNonce + 12);

    for (i = 0; i < 10u; i++) {
        b43_chacha_qr(&x[0], &x[4], &x[8], &x[12]);
        b43_chacha_qr(&x[1], &x[5], &x[9], &x[13]);
        b43_chacha_qr(&x[2], &x[6], &x[10], &x[14]);
        b43_chacha_qr(&x[3], &x[7], &x[11], &x[15]);
        b43_chacha_qr(&x[0], &x[5], &x[10], &x[15]);
        b43_chacha_qr(&x[1], &x[6], &x[11], &x[12]);
        b43_chacha_qr(&x[2], &x[7], &x[8], &x[13]);
        b43_chacha_qr(&x[3], &x[4], &x[9], &x[14]);
    }

    b43_store_le32(aOut + 0, x[0]);
    b43_store_le32(aOut + 4, x[1]);
    b43_store_le32(aOut + 8, x[2]);
    b43_store_le32(aOut + 12, x[3]);
    b43_store_le32(aOut + 16, x[12]);
    b43_store_le32(aOut + 20, x[13]);
    b43_store_le32(aOut + 24, x[14]);
    b43_store_le32(aOut + 28, x[15]);
    return 0;
}

/*
 * xchacha20_xor: XChaCha20 with 24-byte nonce.
 * subkey = HChaCha20(key, nonce[0..15]);
 * chacha20_xor(subkey, 0||nonce[16..23], counter, ...)
 */
int
xchacha20_xor(const unsigned char aKey[32], const unsigned char aNonce[24],
              uint32_t uCounter, const unsigned char *pIn, unsigned char *pOut,
              size_t cb)
{
    unsigned char aSub[32];
    unsigned char aN12[12];

    if (aKey == NULL || aNonce == NULL) {
        return -1;
    }
    if (hchacha20(aKey, aNonce, aSub) != 0) {
        return -1;
    }
    /* IETF: ChaCha nonce is 4 zero bytes || last 8 of 24-byte nonce */
    aN12[0] = 0;
    aN12[1] = 0;
    aN12[2] = 0;
    aN12[3] = 0;
    aN12[4] = aNonce[16];
    aN12[5] = aNonce[17];
    aN12[6] = aNonce[18];
    aN12[7] = aNonce[19];
    aN12[8] = aNonce[20];
    aN12[9] = aNonce[21];
    aN12[10] = aNonce[22];
    aN12[11] = aNonce[23];
    return chacha20_xor(aSub, aN12, uCounter, pIn, pOut, cb);
}

int __chacha20_encrypt(const unsigned char aKey[32],
                       const unsigned char aNonce[12], uint32_t uCounter,
                       const unsigned char *pIn, unsigned char *pOut, size_t cb)
    __attribute__((alias("chacha20_encrypt")));
int __hchacha20(const unsigned char aKey[32], const unsigned char aNonce[16],
                unsigned char aOut[32]) __attribute__((alias("hchacha20")));
int __xchacha20_xor(const unsigned char aKey[32],
                    const unsigned char aNonce[24], uint32_t uCounter,
                    const unsigned char *pIn, unsigned char *pOut, size_t cb)
    __attribute__((alias("xchacha20_xor")));
