/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90: ChaCha20 SSE2 stream XOR (RFC 8439).
 * Clean-room freestanding C with SSE2 quarter-round parallelization
 * over 4×64-byte blocks (256-byte chunks). Userspace only (-msse2).
 *
 * Does NOT redefine chacha20_xor / chacha20_block (batch42) or
 * salsa20_* (batch56). Remainder / small lengths fall back to extern
 * chacha20_xor for a shared correctness path.
 *
 * Public ABI:
 *   void chacha20_xor_sse2(unsigned char *out, const unsigned char *in,
 *                          size_t len, const unsigned char key[32],
 *                          const unsigned char nonce[12], uint32_t counter);
 *   __chacha20_xor_sse2 (alias)
 *   __libcgj_batch90_marker = "libcgj-batch90"
 *
 * Soft deepen: SSE2 XOR of 256-byte keystream chunks (16B lanes), null
 * reject, remainder via batch42 chacha20_xor for shared correctness.
 */

#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch90_marker[] = "libcgj-batch90";

/* batch42 — RFC 8439 §2.4; do not redefine in this TU */
int chacha20_xor(const unsigned char aKey[32], const unsigned char aNonce[12],
                 uint32_t uCounter, const unsigned char *pIn,
                 unsigned char *pOut, size_t cb);

/* ---- little-endian helpers --------------------------------------------- */

static uint32_t
b90_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b90_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

/* ---- SSE2 helpers ------------------------------------------------------ */

static __m128i
b90_rotl32_sse(__m128i v, unsigned n)
{
    return _mm_or_si128(_mm_slli_epi32(v, (int)n),
                        _mm_srli_epi32(v, (int)(32u - n)));
}

/* One ChaCha quarter-round on four parallel 32-bit lanes (4 blocks). */
static void
b90_qr(__m128i *a, __m128i *b, __m128i *c, __m128i *d)
{
    *a = _mm_add_epi32(*a, *b);
    *d = _mm_xor_si128(*d, *a);
    *d = b90_rotl32_sse(*d, 16);

    *c = _mm_add_epi32(*c, *d);
    *b = _mm_xor_si128(*b, *c);
    *b = b90_rotl32_sse(*b, 12);

    *a = _mm_add_epi32(*a, *b);
    *d = _mm_xor_si128(*d, *a);
    *d = b90_rotl32_sse(*d, 8);

    *c = _mm_add_epi32(*c, *d);
    *b = _mm_xor_si128(*b, *c);
    *b = b90_rotl32_sse(*b, 7);
}

/*
 * Store one state word (4 parallel lanes → 4 blocks) as LE u32 at word
 * index wi in each 64-byte keystream block.
 */
static void
b90_scatter_word(unsigned char aBlk[4][64], unsigned wi, __m128i v)
{
    uint32_t aT[4];
    unsigned b;

    _mm_storeu_si128((__m128i *)(void *)aT, v);
    for (b = 0u; b < 4u; b++) {
        b90_store_le32(aBlk[b] + wi * 4u, aT[b]);
    }
}

/*
 * Generate 256 bytes of keystream: four consecutive ChaCha20 blocks at
 * counters counter..counter+3, vertical SSE2 (same word across 4 blocks
 * in each XMM lane).
 */
static void
b90_chacha20_blocks4(unsigned char aKs[256], const unsigned char aKey[32],
                     const unsigned char aNonce[12], uint32_t uCounter)
{
    __m128i x0, x1, x2, x3, x4, x5, x6, x7;
    __m128i x8, x9, x10, x11, x12, x13, x14, x15;
    __m128i s0, s1, s2, s3, s4, s5, s6, s7;
    __m128i s8, s9, s10, s11, s12, s13, s14, s15;
    unsigned char aBlk[4][64];
    unsigned i;
    unsigned r;

    /* "expand 32-byte k" + key + counter lanes + nonce */
    s0 = x0 = _mm_set1_epi32((int)0x61707865u);
    s1 = x1 = _mm_set1_epi32((int)0x3320646eu);
    s2 = x2 = _mm_set1_epi32((int)0x79622d32u);
    s3 = x3 = _mm_set1_epi32((int)0x6b206574u);
    s4 = x4 = _mm_set1_epi32((int)b90_load_le32(aKey + 0));
    s5 = x5 = _mm_set1_epi32((int)b90_load_le32(aKey + 4));
    s6 = x6 = _mm_set1_epi32((int)b90_load_le32(aKey + 8));
    s7 = x7 = _mm_set1_epi32((int)b90_load_le32(aKey + 12));
    s8 = x8 = _mm_set1_epi32((int)b90_load_le32(aKey + 16));
    s9 = x9 = _mm_set1_epi32((int)b90_load_le32(aKey + 20));
    s10 = x10 = _mm_set1_epi32((int)b90_load_le32(aKey + 24));
    s11 = x11 = _mm_set1_epi32((int)b90_load_le32(aKey + 28));
    /* lane0=counter, lane1=c+1, lane2=c+2, lane3=c+3
     * _mm_set_epi32 packs high→low: (e3,e2,e1,e0) */
    s12 = x12 = _mm_set_epi32((int)(uCounter + 3u), (int)(uCounter + 2u),
                              (int)(uCounter + 1u), (int)uCounter);
    s13 = x13 = _mm_set1_epi32((int)b90_load_le32(aNonce + 0));
    s14 = x14 = _mm_set1_epi32((int)b90_load_le32(aNonce + 4));
    s15 = x15 = _mm_set1_epi32((int)b90_load_le32(aNonce + 8));

    for (r = 0u; r < 10u; r++) {
        /* column rounds */
        b90_qr(&x0, &x4, &x8, &x12);
        b90_qr(&x1, &x5, &x9, &x13);
        b90_qr(&x2, &x6, &x10, &x14);
        b90_qr(&x3, &x7, &x11, &x15);
        /* diagonal rounds */
        b90_qr(&x0, &x5, &x10, &x15);
        b90_qr(&x1, &x6, &x11, &x12);
        b90_qr(&x2, &x7, &x8, &x13);
        b90_qr(&x3, &x4, &x9, &x14);
    }

    x0 = _mm_add_epi32(x0, s0);
    x1 = _mm_add_epi32(x1, s1);
    x2 = _mm_add_epi32(x2, s2);
    x3 = _mm_add_epi32(x3, s3);
    x4 = _mm_add_epi32(x4, s4);
    x5 = _mm_add_epi32(x5, s5);
    x6 = _mm_add_epi32(x6, s6);
    x7 = _mm_add_epi32(x7, s7);
    x8 = _mm_add_epi32(x8, s8);
    x9 = _mm_add_epi32(x9, s9);
    x10 = _mm_add_epi32(x10, s10);
    x11 = _mm_add_epi32(x11, s11);
    x12 = _mm_add_epi32(x12, s12);
    x13 = _mm_add_epi32(x13, s13);
    x14 = _mm_add_epi32(x14, s14);
    x15 = _mm_add_epi32(x15, s15);

    b90_scatter_word(aBlk, 0u, x0);
    b90_scatter_word(aBlk, 1u, x1);
    b90_scatter_word(aBlk, 2u, x2);
    b90_scatter_word(aBlk, 3u, x3);
    b90_scatter_word(aBlk, 4u, x4);
    b90_scatter_word(aBlk, 5u, x5);
    b90_scatter_word(aBlk, 6u, x6);
    b90_scatter_word(aBlk, 7u, x7);
    b90_scatter_word(aBlk, 8u, x8);
    b90_scatter_word(aBlk, 9u, x9);
    b90_scatter_word(aBlk, 10u, x10);
    b90_scatter_word(aBlk, 11u, x11);
    b90_scatter_word(aBlk, 12u, x12);
    b90_scatter_word(aBlk, 13u, x13);
    b90_scatter_word(aBlk, 14u, x14);
    b90_scatter_word(aBlk, 15u, x15);

    for (i = 0u; i < 4u; i++) {
        unsigned j;
        for (j = 0u; j < 64u; j++) {
            aKs[i * 64u + j] = aBlk[i][j];
        }
    }
}

/*
 * chacha20_xor_sse2 — XOR keystream over in → out.
 * 256-byte chunks use vertical SSE2 (4 parallel blocks); any remainder
 * (or total len < 256) uses batch42 chacha20_xor.
 */
void
chacha20_xor_sse2(unsigned char *out, const unsigned char *in, size_t len,
                  const unsigned char key[32], const unsigned char nonce[12],
                  uint32_t counter)
{
    if (key == NULL || nonce == NULL) {
        return;
    }
    if (len == 0u) {
        return;
    }
    if (out == NULL || in == NULL) {
        return;
    }

    while (len >= 256u) {
        unsigned char aKs[256];
        size_t i;

        b90_chacha20_blocks4(aKs, key, nonce, counter);
        /* Soft deepen: XOR 16-byte lanes with SSE2, then scalar residual. */
        i = 0u;
        while (i + 16u <= 256u) {
            __m128i vIn =
                _mm_loadu_si128((const __m128i *)(const void *)(in + i));
            __m128i vKs =
                _mm_loadu_si128((const __m128i *)(const void *)(aKs + i));
            _mm_storeu_si128((__m128i *)(void *)(out + i),
                             _mm_xor_si128(vIn, vKs));
            i += 16u;
        }
        for (; i < 256u; i++) {
            out[i] = in[i] ^ aKs[i];
        }
        out += 256u;
        in += 256u;
        len -= 256u;
        counter += 4u;
    }

    if (len > 0u) {
        (void)chacha20_xor(key, nonce, counter, in, out, len);
    }
}

void __chacha20_xor_sse2(unsigned char *out, const unsigned char *in,
                         size_t len, const unsigned char key[32],
                         const unsigned char nonce[12], uint32_t counter)
    __attribute__((alias("chacha20_xor_sse2")));
