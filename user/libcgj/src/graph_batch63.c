/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63: BLAKE3 hash (256-bit default XOF output).
 * Clean-room reimplementation from the public BLAKE3 specification
 * (https://github.com/BLAKE3-team/BLAKE3-specs) — algorithm constants and
 * tree rules only; no code copied from any reference implementation.
 * Integer/pointer only (no SSE / no FP). Dual MIT OR Apache-2.0.
 *
 * Opaque hasher buffer:
 *   Caller must provide at least BLAKE3_HASHER_SIZE (2560) bytes.
 *   Compact internal state needs ~1912 bytes (54-level CV stack + chunk);
 *   2560 is the documented fixed minimum for ABI safety/alignment.
 *
 * Public API (+ __ aliases):
 *   blake3_hasher_init / blake3_hasher_update / blake3_hasher_finalize
 *   blake3_hash (one-shot)
 * Marker: __libcgj_batch63_marker = "libcgj-batch63"
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch63_marker[] = "libcgj-batch63";

/* Documented minimum opaque hasher size (bytes). */
#define BLAKE3_HASHER_SIZE 2560u

#define B63_OUT_LEN 32u
#define B63_BLOCK_LEN 64u
#define B63_CHUNK_LEN 1024u
#define B63_CV_STACK_MAX 54u

#define B63_CHUNK_START ((uint32_t)(1u << 0))
#define B63_CHUNK_END ((uint32_t)(1u << 1))
#define B63_PARENT ((uint32_t)(1u << 2))
#define B63_ROOT ((uint32_t)(1u << 3))

/* BLAKE2s / BLAKE3 IV (public constants, same as BLAKE2s). */
static const uint32_t s_b63_iv[8] = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
    0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

/* Message-word permutation applied between the 7 compression rounds. */
static const unsigned char s_b63_msg_perm[16] = {
    2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8};

/*
 * Compact hasher layout packed into the caller-supplied opaque buffer.
 * sizeof(struct b63_hasher) <= BLAKE3_HASHER_SIZE.
 */
struct b63_hasher {
    /* Current chunk state */
    uint32_t aCv[8];
    uint64_t uChunkCounter;
    unsigned char aBlock[B63_BLOCK_LEN];
    unsigned char nBlockLen;
    unsigned char nBlocksCompressed;
    uint32_t uFlags; /* mode flags; plain hash = 0 */
    /* Tree / key */
    uint32_t aKey[8];
    unsigned char nCvStackLen;
    uint32_t aCvStack[B63_CV_STACK_MAX][8];
};

/* ---- freestanding memory helpers ---------------------------------------- */

static void
b63_memzero(void *p, size_t n)
{
    unsigned char *q = (unsigned char *)p;
    size_t i;

    for (i = 0; i < n; i++) {
        q[i] = 0;
    }
}

static void
b63_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    size_t i;

    for (i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

static uint32_t
b63_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b63_store_le32(unsigned char *p, uint32_t w)
{
    p[0] = (unsigned char)w;
    p[1] = (unsigned char)(w >> 8);
    p[2] = (unsigned char)(w >> 16);
    p[3] = (unsigned char)(w >> 24);
}

static uint32_t
b63_rotr32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

/* G mixing step (column or diagonal); same as BLAKE2s. */
static void
b63_g(uint32_t *v, unsigned a, unsigned b, unsigned c, unsigned d, uint32_t mx,
      uint32_t my)
{
    v[a] = v[a] + v[b] + mx;
    v[d] = b63_rotr32(v[d] ^ v[a], 16u);
    v[c] = v[c] + v[d];
    v[b] = b63_rotr32(v[b] ^ v[c], 12u);
    v[a] = v[a] + v[b] + my;
    v[d] = b63_rotr32(v[d] ^ v[a], 8u);
    v[c] = v[c] + v[d];
    v[b] = b63_rotr32(v[b] ^ v[c], 7u);
}

static void
b63_round(uint32_t *v, const uint32_t *m)
{
    b63_g(v, 0, 4, 8, 12, m[0], m[1]);
    b63_g(v, 1, 5, 9, 13, m[2], m[3]);
    b63_g(v, 2, 6, 10, 14, m[4], m[5]);
    b63_g(v, 3, 7, 11, 15, m[6], m[7]);
    b63_g(v, 0, 5, 10, 15, m[8], m[9]);
    b63_g(v, 1, 6, 11, 12, m[10], m[11]);
    b63_g(v, 2, 7, 8, 13, m[12], m[13]);
    b63_g(v, 3, 4, 9, 14, m[14], m[15]);
}

static void
b63_permute(uint32_t *m)
{
    uint32_t tmp[16];
    unsigned i;

    for (i = 0; i < 16u; i++) {
        tmp[i] = m[s_b63_msg_perm[i]];
    }
    for (i = 0; i < 16u; i++) {
        m[i] = tmp[i];
    }
}

/*
 * Compression: 7 rounds of keyed permutation.
 * Output is 16 words: chaining value in [0..7], XOF second half in [8..15].
 */
static void
b63_compress(const uint32_t *cv, const uint32_t *block_words, uint64_t counter,
             uint32_t block_len, uint32_t flags, uint32_t *out16)
{
    uint32_t v[16];
    uint32_t m[16];
    unsigned i, r;

    for (i = 0; i < 8u; i++) {
        v[i] = cv[i];
        v[i + 8u] = s_b63_iv[i];
    }
    /* BLAKE3 places counter/block_len/flags directly (not XOR'd into IV). */
    v[12] = (uint32_t)counter;
    v[13] = (uint32_t)(counter >> 32);
    v[14] = block_len;
    v[15] = flags;

    for (i = 0; i < 16u; i++) {
        m[i] = block_words[i];
    }

    for (r = 0; r < 7u; r++) {
        b63_round(v, m);
        if (r + 1u < 7u) {
            b63_permute(m);
        }
    }

    for (i = 0; i < 8u; i++) {
        out16[i] = v[i] ^ v[i + 8u];
        out16[i + 8u] = v[i + 8u] ^ cv[i];
    }
}

static void
b63_words_from_block(const unsigned char *block, uint32_t *words)
{
    unsigned i;

    for (i = 0; i < 16u; i++) {
        words[i] = b63_load_le32(block + i * 4u);
    }
}

/* Snapshot of a node ready for chaining_value() or root_output(). */
struct b63_output {
    uint32_t aInputCv[8];
    uint32_t aBlockWords[16];
    uint64_t uCounter;
    uint32_t uBlockLen;
    uint32_t uFlags;
};

static void
b63_output_cv(const struct b63_output *o, uint32_t *cv_out)
{
    uint32_t out16[16];
    unsigned i;

    b63_compress(o->aInputCv, o->aBlockWords, o->uCounter, o->uBlockLen,
                 o->uFlags, out16);
    for (i = 0; i < 8u; i++) {
        cv_out[i] = out16[i];
    }
}

static void
b63_root_output_bytes(const struct b63_output *o, uint8_t *out, size_t out_len)
{
    size_t done = 0;
    uint64_t out_counter = 0;

    while (done < out_len) {
        uint32_t out16[16];
        unsigned char aBuf[64];
        unsigned i;
        size_t nTake;

        b63_compress(o->aInputCv, o->aBlockWords, out_counter, o->uBlockLen,
                     o->uFlags | B63_ROOT, out16);
        for (i = 0; i < 16u; i++) {
            b63_store_le32(aBuf + i * 4u, out16[i]);
        }
        nTake = out_len - done;
        if (nTake > 64u) {
            nTake = 64u;
        }
        b63_memcpy(out + done, aBuf, nTake);
        done += nTake;
        out_counter++;
    }
}

static void
b63_chunk_reset(struct b63_hasher *h, uint64_t chunk_counter)
{
    unsigned i;

    for (i = 0; i < 8u; i++) {
        h->aCv[i] = h->aKey[i];
    }
    h->uChunkCounter = chunk_counter;
    b63_memzero(h->aBlock, B63_BLOCK_LEN);
    h->nBlockLen = 0;
    h->nBlocksCompressed = 0;
}

static size_t
b63_chunk_len(const struct b63_hasher *h)
{
    return (size_t)h->nBlocksCompressed * B63_BLOCK_LEN + (size_t)h->nBlockLen;
}

static uint32_t
b63_chunk_start_flag(const struct b63_hasher *h)
{
    return (h->nBlocksCompressed == 0) ? B63_CHUNK_START : 0u;
}

static void
b63_chunk_update(struct b63_hasher *h, const unsigned char *input, size_t n)
{
    while (n > 0u) {
        uint32_t words[16];
        uint32_t out16[16];
        size_t want, take;
        unsigned i;

        if (h->nBlockLen == (unsigned char)B63_BLOCK_LEN) {
            b63_words_from_block(h->aBlock, words);
            b63_compress(h->aCv, words, h->uChunkCounter, (uint32_t)B63_BLOCK_LEN,
                         h->uFlags | b63_chunk_start_flag(h), out16);
            for (i = 0; i < 8u; i++) {
                h->aCv[i] = out16[i];
            }
            h->nBlocksCompressed = (unsigned char)(h->nBlocksCompressed + 1u);
            b63_memzero(h->aBlock, B63_BLOCK_LEN);
            h->nBlockLen = 0;
        }

        want = B63_BLOCK_LEN - (size_t)h->nBlockLen;
        take = (n < want) ? n : want;
        b63_memcpy(h->aBlock + h->nBlockLen, input, take);
        h->nBlockLen = (unsigned char)(h->nBlockLen + (unsigned char)take);
        input += take;
        n -= take;
    }
}

static void
b63_chunk_output(const struct b63_hasher *h, struct b63_output *o)
{
    unsigned i;

    for (i = 0; i < 8u; i++) {
        o->aInputCv[i] = h->aCv[i];
    }
    b63_words_from_block(h->aBlock, o->aBlockWords);
    /* Zero-pad remainder of last partial block words already via zeroed buffer. */
    o->uCounter = h->uChunkCounter;
    o->uBlockLen = (uint32_t)h->nBlockLen;
    o->uFlags = h->uFlags | b63_chunk_start_flag(h) | B63_CHUNK_END;
}

static void
b63_parent_output(const uint32_t *left_cv, const uint32_t *right_cv,
                  const uint32_t *key, uint32_t flags, struct b63_output *o)
{
    unsigned i;

    for (i = 0; i < 8u; i++) {
        o->aInputCv[i] = key[i];
        o->aBlockWords[i] = left_cv[i];
        o->aBlockWords[i + 8u] = right_cv[i];
    }
    o->uCounter = 0;
    o->uBlockLen = (uint32_t)B63_BLOCK_LEN;
    o->uFlags = B63_PARENT | flags;
}

static void
b63_parent_cv(const uint32_t *left_cv, const uint32_t *right_cv,
              const uint32_t *key, uint32_t flags, uint32_t *cv_out)
{
    struct b63_output o;

    b63_parent_output(left_cv, right_cv, key, flags, &o);
    b63_output_cv(&o, cv_out);
}

static void
b63_push_cv(struct b63_hasher *h, const uint32_t *cv)
{
    unsigned i;

    if (h->nCvStackLen >= B63_CV_STACK_MAX) {
        return; /* should never happen for < 2^64 bytes */
    }
    for (i = 0; i < 8u; i++) {
        h->aCvStack[h->nCvStackLen][i] = cv[i];
    }
    h->nCvStackLen = (unsigned char)(h->nCvStackLen + 1u);
}

static void
b63_pop_cv(struct b63_hasher *h, uint32_t *cv_out)
{
    unsigned i;

    h->nCvStackLen = (unsigned char)(h->nCvStackLen - 1u);
    for (i = 0; i < 8u; i++) {
        cv_out[i] = h->aCvStack[h->nCvStackLen][i];
    }
}

/* Merge completed subtree CVs per trailing-zero rule (spec §5.1.2). */
static void
b63_add_chunk_cv(struct b63_hasher *h, uint32_t *new_cv, uint64_t total_chunks)
{
    while ((total_chunks & 1u) == 0u) {
        uint32_t left[8];

        b63_pop_cv(h, left);
        b63_parent_cv(left, new_cv, h->aKey, h->uFlags, new_cv);
        total_chunks >>= 1;
    }
    b63_push_cv(h, new_cv);
}

/* ---- public API --------------------------------------------------------- */

void
blake3_hasher_init(void *hasher)
{
    struct b63_hasher *h = (struct b63_hasher *)hasher;
    unsigned i;

    if (h == NULL) {
        return;
    }
    b63_memzero(h, sizeof(*h));
    for (i = 0; i < 8u; i++) {
        h->aKey[i] = s_b63_iv[i];
        h->aCv[i] = s_b63_iv[i];
    }
    h->uFlags = 0;
    h->nCvStackLen = 0;
    h->uChunkCounter = 0;
    h->nBlockLen = 0;
    h->nBlocksCompressed = 0;
}

void
blake3_hasher_update(void *hasher, const void *input, size_t input_len)
{
    struct b63_hasher *h = (struct b63_hasher *)hasher;
    const unsigned char *p = (const unsigned char *)input;

    if (h == NULL || (input == NULL && input_len != 0u)) {
        return;
    }
    while (input_len > 0u) {
        size_t want, take;
        struct b63_output o;
        uint32_t chunk_cv[8];
        uint64_t total_chunks;

        if (b63_chunk_len(h) == B63_CHUNK_LEN) {
            b63_chunk_output(h, &o);
            b63_output_cv(&o, chunk_cv);
            total_chunks = h->uChunkCounter + 1u;
            b63_add_chunk_cv(h, chunk_cv, total_chunks);
            b63_chunk_reset(h, total_chunks);
        }

        want = B63_CHUNK_LEN - b63_chunk_len(h);
        take = (input_len < want) ? input_len : want;
        b63_chunk_update(h, p, take);
        p += take;
        input_len -= take;
    }
}

void
blake3_hasher_finalize(const void *hasher, uint8_t *out, size_t out_len)
{
    const struct b63_hasher *h = (const struct b63_hasher *)hasher;
    struct b63_output output;
    unsigned parent_remaining;
    uint32_t right_cv[8];

    if (h == NULL || out == NULL || out_len == 0u) {
        return;
    }

    b63_chunk_output(h, &output);
    parent_remaining = h->nCvStackLen;
    while (parent_remaining > 0u) {
        parent_remaining--;
        b63_output_cv(&output, right_cv);
        b63_parent_output(h->aCvStack[parent_remaining], right_cv, h->aKey,
                          h->uFlags, &output);
    }
    b63_root_output_bytes(&output, out, out_len);
}

void
blake3_hash(const void *input, size_t input_len, uint8_t *out, size_t out_len)
{
    unsigned char aCtx[BLAKE3_HASHER_SIZE];

    if (out == NULL || out_len == 0u) {
        return;
    }
    if (input == NULL && input_len != 0u) {
        return;
    }
    blake3_hasher_init(aCtx);
    blake3_hasher_update(aCtx, input, input_len);
    blake3_hasher_finalize(aCtx, out, out_len);
}

/* ---- __ aliases (glibc-style dual export) ------------------------------- */

void __blake3_hasher_init(void *hasher)
    __attribute__((alias("blake3_hasher_init")));
void __blake3_hasher_update(void *hasher, const void *input, size_t input_len)
    __attribute__((alias("blake3_hasher_update")));
void __blake3_hasher_finalize(const void *hasher, uint8_t *out, size_t out_len)
    __attribute__((alias("blake3_hasher_finalize")));
void __blake3_hash(const void *input, size_t input_len, uint8_t *out,
                   size_t out_len) __attribute__((alias("blake3_hash")));
