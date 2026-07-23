/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch59: Serpent block cipher (AES finalist).
 * Clean-room from public Anderson/Biham/Knudsen specification:
 *   128-bit block; 128/192/256-bit keys; 32 rounds; bitsliced mode
 *   (no IP/FP — matches NESSIE / common software layout).
 * Integer/pointer only (no SSE, no FP). Freestanding-safe TU.
 *
 * Public ABI:
 *   serpent_set_key(const unsigned char *key, unsigned bits,
 *                   uint32_t *subkeys);  // 132 words
 *   serpent_encrypt(const uint32_t *subkeys,
 *                   const unsigned char in[16], unsigned char out[16]);
 *   serpent_decrypt(const uint32_t *subkeys,
 *                   const unsigned char in[16], unsigned char out[16]);
 * plus __serpent_* aliases. Marker: __libcgj_batch59_marker.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch59_marker[] = "libcgj-batch59";

/* Golden-ratio constant from the Serpent paper (phi = 0x9e3779b9). */
#define B59_PHI 0x9e3779b9u

/* ---- helpers ----------------------------------------------------------- */

static uint32_t
b59_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static uint32_t
b59_ror32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static uint32_t
b59_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b59_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

/* ---- S-boxes (paper A.5) and inverses ---------------------------------- */

static const unsigned char s_b59_sbox[8][16] = {
    /* S0 */ {3, 8, 15, 1, 10, 6, 5, 11, 14, 13, 4, 2, 7, 0, 9, 12},
    /* S1 */ {15, 12, 2, 7, 9, 0, 5, 10, 1, 11, 14, 8, 6, 13, 3, 4},
    /* S2 */ {8, 6, 7, 9, 3, 12, 10, 15, 13, 1, 14, 4, 0, 11, 5, 2},
    /* S3 */ {0, 15, 11, 8, 12, 9, 6, 3, 13, 1, 2, 4, 10, 7, 5, 14},
    /* S4 */ {1, 15, 8, 3, 12, 0, 11, 6, 2, 5, 4, 10, 9, 14, 7, 13},
    /* S5 */ {15, 5, 2, 11, 4, 10, 9, 12, 0, 3, 14, 8, 13, 6, 7, 1},
    /* S6 */ {7, 2, 12, 5, 8, 4, 6, 11, 14, 9, 1, 15, 13, 3, 10, 0},
    /* S7 */ {1, 13, 15, 0, 14, 8, 2, 11, 7, 4, 12, 10, 9, 3, 5, 6}};

static const unsigned char s_b59_isbox[8][16] = {
    /* InvS0 */ {13, 3, 11, 0, 10, 6, 5, 12, 1, 14, 4, 7, 15, 9, 8, 2},
    /* InvS1 */ {5, 8, 2, 14, 15, 6, 12, 3, 11, 4, 7, 9, 1, 13, 10, 0},
    /* InvS2 */ {12, 9, 15, 4, 11, 14, 1, 2, 0, 3, 6, 13, 5, 8, 10, 7},
    /* InvS3 */ {0, 9, 10, 7, 11, 14, 6, 13, 3, 5, 12, 2, 4, 8, 15, 1},
    /* InvS4 */ {5, 0, 8, 3, 10, 9, 7, 14, 2, 12, 11, 6, 4, 15, 13, 1},
    /* InvS5 */ {8, 15, 2, 9, 4, 1, 13, 14, 11, 6, 5, 3, 7, 12, 10, 0},
    /* InvS6 */ {15, 10, 1, 13, 5, 3, 6, 0, 4, 9, 14, 7, 2, 12, 8, 11},
    /* InvS7 */ {3, 0, 6, 13, 9, 14, 15, 8, 5, 12, 11, 7, 10, 1, 4, 2}};

/*
 * Bitsliced 4×32 S-box: for each bit position, the four bits from
 * (x0,x1,x2,x3) form a nibble fed to S[which]; results written back.
 * Word0 supplies LSB of the nibble (Serpent paper convention).
 */
static void
b59_sbox(unsigned which, uint32_t *x0, uint32_t *x1, uint32_t *x2,
         uint32_t *x3)
{
    uint32_t a = *x0, b = *x1, c = *x2, d = *x3;
    uint32_t y0 = 0, y1 = 0, y2 = 0, y3 = 0;
    unsigned k;

    for (k = 0; k < 32u; k++) {
        unsigned in = ((a >> k) & 1u) | (((b >> k) & 1u) << 1) |
                      (((c >> k) & 1u) << 2) | (((d >> k) & 1u) << 3);
        unsigned out = s_b59_sbox[which & 7u][in];
        y0 |= (uint32_t)((out >> 0) & 1u) << k;
        y1 |= (uint32_t)((out >> 1) & 1u) << k;
        y2 |= (uint32_t)((out >> 2) & 1u) << k;
        y3 |= (uint32_t)((out >> 3) & 1u) << k;
    }
    *x0 = y0;
    *x1 = y1;
    *x2 = y2;
    *x3 = y3;
}

static void
b59_isbox(unsigned which, uint32_t *x0, uint32_t *x1, uint32_t *x2,
          uint32_t *x3)
{
    uint32_t a = *x0, b = *x1, c = *x2, d = *x3;
    uint32_t y0 = 0, y1 = 0, y2 = 0, y3 = 0;
    unsigned k;

    for (k = 0; k < 32u; k++) {
        unsigned in = ((a >> k) & 1u) | (((b >> k) & 1u) << 1) |
                      (((c >> k) & 1u) << 2) | (((d >> k) & 1u) << 3);
        unsigned out = s_b59_isbox[which & 7u][in];
        y0 |= (uint32_t)((out >> 0) & 1u) << k;
        y1 |= (uint32_t)((out >> 1) & 1u) << k;
        y2 |= (uint32_t)((out >> 2) & 1u) << k;
        y3 |= (uint32_t)((out >> 3) & 1u) << k;
    }
    *x0 = y0;
    *x1 = y1;
    *x2 = y2;
    *x3 = y3;
}

/* ---- Linear transform and inverse (paper) ------------------------------ */

static void
b59_lt(uint32_t *x0, uint32_t *x1, uint32_t *x2, uint32_t *x3)
{
    uint32_t a = *x0, b = *x1, c = *x2, d = *x3;

    a = b59_rol32(a, 13);
    c = b59_rol32(c, 3);
    b = b ^ a ^ c;
    d = d ^ c ^ (a << 3);
    b = b59_rol32(b, 1);
    d = b59_rol32(d, 7);
    a = a ^ b ^ d;
    c = c ^ d ^ (b << 7);
    a = b59_rol32(a, 5);
    c = b59_rol32(c, 22);

    *x0 = a;
    *x1 = b;
    *x2 = c;
    *x3 = d;
}

static void
b59_ilt(uint32_t *x0, uint32_t *x1, uint32_t *x2, uint32_t *x3)
{
    uint32_t a = *x0, b = *x1, c = *x2, d = *x3;

    c = b59_ror32(c, 22);
    a = b59_ror32(a, 5);
    c = c ^ d ^ (b << 7);
    a = a ^ b ^ d;
    d = b59_ror32(d, 7);
    b = b59_ror32(b, 1);
    d = d ^ c ^ (a << 3);
    b = b ^ a ^ c;
    c = b59_ror32(c, 3);
    a = b59_ror32(a, 13);

    *x0 = a;
    *x1 = b;
    *x2 = c;
    *x3 = d;
}

/* ---- Key schedule: 132 subkey words ------------------------------------ */

/*
 * Pad user key to 256 bits (append a single 1-bit then zeros), expand to
 * 132 prekeys, then S-box them with S_{(3-i) mod 8} per 4-word group.
 * subkeys must hold at least 132 uint32_t words.
 */
void
serpent_set_key(const unsigned char *key, unsigned bits, uint32_t *subkeys)
{
    uint32_t w[140];
    unsigned nbytes;
    unsigned i;
    unsigned r;

    if (subkeys == NULL) {
        return;
    }
    if (key == NULL || (bits != 128u && bits != 192u && bits != 256u)) {
        /* Leave schedule zeroed on bad input so encrypt is deterministic. */
        for (i = 0; i < 132u; i++) {
            subkeys[i] = 0;
        }
        return;
    }

    nbytes = bits / 8u;
    for (i = 0; i < 8u; i++) {
        w[i] = 0;
    }
    for (i = 0; i < nbytes; i++) {
        w[i / 4u] |= (uint32_t)key[i] << ((i % 4u) * 8u);
    }
    /* Pad: one '1' bit immediately after the key material (LE bit order). */
    if (bits < 256u) {
        w[nbytes / 4u] |= (uint32_t)1u << ((nbytes % 4u) * 8u);
    }

    /* Prekey expansion: w[8..139] → 132 words. */
    for (i = 8u; i < 140u; i++) {
        w[i] = b59_rol32(w[i - 8u] ^ w[i - 5u] ^ w[i - 3u] ^ w[i - 1u] ^
                             B59_PHI ^ (i - 8u),
                         11);
    }

    /*
     * Apply S-boxes: for subkey group r (0..32), use S_{(3-r) mod 8}
     * on prekey words w[8+4r .. 8+4r+3].
     */
    for (r = 0; r < 33u; r++) {
        uint32_t a = w[8u + 4u * r];
        uint32_t b = w[8u + 4u * r + 1u];
        uint32_t c = w[8u + 4u * r + 2u];
        uint32_t d = w[8u + 4u * r + 3u];
        unsigned which = (unsigned)((3 - (int)r) & 7);

        b59_sbox(which, &a, &b, &c, &d);
        subkeys[4u * r] = a;
        subkeys[4u * r + 1u] = b;
        subkeys[4u * r + 2u] = c;
        subkeys[4u * r + 3u] = d;
    }
}

void
serpent_encrypt(const uint32_t *subkeys, const unsigned char in[16],
                unsigned char out[16])
{
    uint32_t x0, x1, x2, x3;
    unsigned r;

    if (subkeys == NULL || in == NULL || out == NULL) {
        return;
    }

    x0 = b59_load_le32(in);
    x1 = b59_load_le32(in + 4);
    x2 = b59_load_le32(in + 8);
    x3 = b59_load_le32(in + 12);

    for (r = 0; r < 31u; r++) {
        x0 ^= subkeys[4u * r];
        x1 ^= subkeys[4u * r + 1u];
        x2 ^= subkeys[4u * r + 2u];
        x3 ^= subkeys[4u * r + 3u];
        b59_sbox(r & 7u, &x0, &x1, &x2, &x3);
        b59_lt(&x0, &x1, &x2, &x3);
    }
    /* Round 31: S7 then final key mix (no LT). */
    x0 ^= subkeys[4u * 31u];
    x1 ^= subkeys[4u * 31u + 1u];
    x2 ^= subkeys[4u * 31u + 2u];
    x3 ^= subkeys[4u * 31u + 3u];
    b59_sbox(7, &x0, &x1, &x2, &x3);
    x0 ^= subkeys[4u * 32u];
    x1 ^= subkeys[4u * 32u + 1u];
    x2 ^= subkeys[4u * 32u + 2u];
    x3 ^= subkeys[4u * 32u + 3u];

    b59_store_le32(out, x0);
    b59_store_le32(out + 4, x1);
    b59_store_le32(out + 8, x2);
    b59_store_le32(out + 12, x3);
}

void
serpent_decrypt(const uint32_t *subkeys, const unsigned char in[16],
                unsigned char out[16])
{
    uint32_t x0, x1, x2, x3;
    int r;

    if (subkeys == NULL || in == NULL || out == NULL) {
        return;
    }

    x0 = b59_load_le32(in);
    x1 = b59_load_le32(in + 4);
    x2 = b59_load_le32(in + 8);
    x3 = b59_load_le32(in + 12);

    x0 ^= subkeys[4u * 32u];
    x1 ^= subkeys[4u * 32u + 1u];
    x2 ^= subkeys[4u * 32u + 2u];
    x3 ^= subkeys[4u * 32u + 3u];
    b59_isbox(7, &x0, &x1, &x2, &x3);
    x0 ^= subkeys[4u * 31u];
    x1 ^= subkeys[4u * 31u + 1u];
    x2 ^= subkeys[4u * 31u + 2u];
    x3 ^= subkeys[4u * 31u + 3u];

    for (r = 30; r >= 0; r--) {
        b59_ilt(&x0, &x1, &x2, &x3);
        b59_isbox((unsigned)r & 7u, &x0, &x1, &x2, &x3);
        x0 ^= subkeys[4u * (unsigned)r];
        x1 ^= subkeys[4u * (unsigned)r + 1u];
        x2 ^= subkeys[4u * (unsigned)r + 2u];
        x3 ^= subkeys[4u * (unsigned)r + 3u];
    }

    b59_store_le32(out, x0);
    b59_store_le32(out + 4, x1);
    b59_store_le32(out + 8, x2);
    b59_store_le32(out + 12, x3);
}

void __serpent_set_key(const unsigned char *key, unsigned bits,
                       uint32_t *subkeys)
    __attribute__((alias("serpent_set_key")));
void __serpent_encrypt(const uint32_t *subkeys, const unsigned char in[16],
                       unsigned char out[16])
    __attribute__((alias("serpent_encrypt")));
void __serpent_decrypt(const uint32_t *subkeys, const unsigned char in[16],
                       unsigned char out[16])
    __attribute__((alias("serpent_decrypt")));
