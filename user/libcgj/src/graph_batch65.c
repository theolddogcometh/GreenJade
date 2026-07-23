/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch65: Argon2i / Argon2id (RFC 9106) working
 * memory-hard password hashes for modest parameters.
 *
 * Clean-room from public RFC 9106 only (no third-party source).
 * Integer/pointer only (no SSE/FP). Dual MIT/Apache.
 *
 * Surface (unique names — do NOT collide with batch46 ENOSYS stubs
 * argon2id_hash_raw / argon2i_hash_raw):
 *   argon2id_hash(...)
 *   argon2i_hash(...)
 *   __argon2id_hash / __argon2i_hash aliases
 *   __libcgj_batch65_marker = "libcgj-batch65"
 *
 * Limits (bring-up soft caps; algorithm structure is full RFC):
 *   - m_cost_kib * 1024 <= B65_MAX_MEM_BYTES (1 MiB)
 *   - parallelism p in [1, B65_MAX_LANES]
 *   - t_cost >= 1, outlen in [4, B65_MAX_OUT]
 *   - salt may be empty (length field still encoded); secret/AD omitted
 *
 * BLAKE2b: batch38 exports fixed-64 BLAKE2b_Init/Update/Final. Argon2's
 * H^x requires variable digest length nn (RFC 7693 param block). This TU
 * carries a private variable-nn BLAKE2b used only as Argon2 H / H'.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch65_marker[] = "libcgj-batch65";

void *malloc(size_t cb);
void free(void *p);

/* Soft caps so bring-up cannot request unbounded RAM. */
#define B65_MAX_MEM_BYTES (1u * 1024u * 1024u)
#define B65_MAX_LANES     8u
#define B65_MAX_OUT       1024u
#define B65_SYNC_POINTS   4u
#define B65_BLOCK_BYTES   1024u
#define B65_QWORDS        128u
#define B65_VERSION       0x13u /* Argon2 v1.3 */

enum {
    B65_ARGON2_D = 0,
    B65_ARGON2_I = 1,
    B65_ARGON2_ID = 2
};

/* ---- freestanding mem helpers ------------------------------------------ */

static void
b65_memzero(void *pDst, size_t cb)
{
    volatile unsigned char *p = (volatile unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static void
b65_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pD = (unsigned char *)pDst;
    const unsigned char *pS = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
}

static void
b65_store_le32(unsigned char *p, uint32_t u)
{
    p[0] = (unsigned char)u;
    p[1] = (unsigned char)(u >> 8);
    p[2] = (unsigned char)(u >> 16);
    p[3] = (unsigned char)(u >> 24);
}

static void
b65_store_le64(unsigned char *p, uint64_t u)
{
    unsigned i;

    for (i = 0; i < 8u; i++) {
        p[i] = (unsigned char)(u >> (8u * i));
    }
}

static uint64_t
b65_load_le64(const unsigned char *p)
{
    return (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) | ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) | ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

static uint64_t
b65_rotr64(uint64_t u, unsigned n)
{
    return (u >> n) | (u << (64u - n));
}

/* ---- Private BLAKE2b with variable nn (Argon2 H^x) --------------------- */

struct b65_blake2b {
    uint64_t aH[8];
    uint64_t aT[2];
    unsigned char aBuf[128];
    size_t cbBuf;
    size_t cbOut;
    int fFinalized;
};

static const uint64_t s_b65_b2b_iv[8] = {
    0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull, 0x3c6ef372fe94f82bull,
    0xa54ff53a5f1d36f1ull, 0x510e527fade682d1ull, 0x9b05688c2b3e6c1full,
    0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull};

static const unsigned char s_b65_b2b_sigma[12][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

static void
b65_b2b_g(uint64_t *v, unsigned a, unsigned b, unsigned c, unsigned d,
          uint64_t x, uint64_t y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = b65_rotr64(v[d] ^ v[a], 32);
    v[c] = v[c] + v[d];
    v[b] = b65_rotr64(v[b] ^ v[c], 24);
    v[a] = v[a] + v[b] + y;
    v[d] = b65_rotr64(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = b65_rotr64(v[b] ^ v[c], 63);
}

static void
b65_b2b_compress(struct b65_blake2b *p, int fLast)
{
    uint64_t v[16];
    uint64_t m[16];
    unsigned i;
    unsigned r;

    for (i = 0; i < 8u; i++) {
        v[i] = p->aH[i];
        v[i + 8u] = s_b65_b2b_iv[i];
    }
    v[12] ^= p->aT[0];
    v[13] ^= p->aT[1];
    if (fLast) {
        v[14] = ~v[14];
    }
    for (i = 0; i < 16u; i++) {
        m[i] = b65_load_le64(p->aBuf + i * 8u);
    }
    for (r = 0; r < 12u; r++) {
        const unsigned char *s = s_b65_b2b_sigma[r];

        b65_b2b_g(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        b65_b2b_g(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        b65_b2b_g(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        b65_b2b_g(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        b65_b2b_g(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        b65_b2b_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        b65_b2b_g(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        b65_b2b_g(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }
    for (i = 0; i < 8u; i++) {
        p->aH[i] ^= v[i] ^ v[i + 8u];
    }
}

/* Init BLAKE2b with digest length nn bytes (1..64). */
static void
b65_blake2b_init(struct b65_blake2b *p, size_t cbOut)
{
    unsigned i;

    b65_memzero(p, sizeof(*p));
    for (i = 0; i < 8u; i++) {
        p->aH[i] = s_b65_b2b_iv[i];
    }
    p->cbOut = cbOut;
    /* param: digest_length=cbOut, key_length=0, fanout=1, depth=1 */
    p->aH[0] ^= 0x01010000ull ^ (uint64_t)cbOut;
}

static void
b65_blake2b_update(struct b65_blake2b *p, const void *pData, size_t cb)
{
    const unsigned char *q = (const unsigned char *)pData;

    if (pData == NULL && cb != 0u) {
        return;
    }
    while (cb > 0u) {
        size_t nLeft = 128u - p->cbBuf;
        size_t nTake = (cb < nLeft) ? cb : nLeft;

        b65_memcpy(p->aBuf + p->cbBuf, q, nTake);
        p->cbBuf += nTake;
        q += nTake;
        cb -= nTake;
        if (p->cbBuf == 128u) {
            p->aT[0] += 128u;
            if (p->aT[0] < 128u) {
                p->aT[1]++;
            }
            b65_b2b_compress(p, 0);
            p->cbBuf = 0;
        }
    }
}

static void
b65_blake2b_final(struct b65_blake2b *p, unsigned char *pOut, size_t cbOut)
{
    unsigned char aFull[64];
    unsigned i;
    size_t n;

    p->aT[0] += (uint64_t)p->cbBuf;
    if (p->aT[0] < (uint64_t)p->cbBuf) {
        p->aT[1]++;
    }
    b65_memzero(p->aBuf + p->cbBuf, 128u - p->cbBuf);
    b65_b2b_compress(p, 1);
    for (i = 0; i < 8u; i++) {
        b65_store_le64(aFull + i * 8u, p->aH[i]);
    }
    n = (cbOut < p->cbOut) ? cbOut : p->cbOut;
    if (n > 64u) {
        n = 64u;
    }
    b65_memcpy(pOut, aFull, n);
    b65_memzero(aFull, sizeof(aFull));
    b65_memzero(p, sizeof(*p));
}

/* One-shot H^nn(msg). */
static void
b65_H(unsigned char *pOut, size_t cbOut, const void *pMsg, size_t cbMsg)
{
    struct b65_blake2b ctx;

    b65_blake2b_init(&ctx, cbOut);
    b65_blake2b_update(&ctx, pMsg, cbMsg);
    b65_blake2b_final(&ctx, pOut, cbOut);
}

/*
 * Variable-length H' (RFC 9106 §3.3).
 * Writes cbOut bytes to pOut from input A.
 */
static void
b65_Hprime(unsigned char *pOut, size_t cbOut, const void *pA, size_t cbA)
{
    unsigned char aLen[4];
    unsigned char aV[64];
    unsigned char aTmp[64];
    size_t r;
    size_t i;
    size_t cbLast;

    b65_store_le32(aLen, (uint32_t)cbOut);

    if (cbOut <= 64u) {
        struct b65_blake2b ctx;

        b65_blake2b_init(&ctx, cbOut);
        b65_blake2b_update(&ctx, aLen, 4u);
        b65_blake2b_update(&ctx, pA, cbA);
        b65_blake2b_final(&ctx, pOut, cbOut);
        return;
    }

    /* r = ceil(T/32) - 2 */
    r = ((cbOut + 31u) / 32u) - 2u;

    /* V1 = H^64(LE32(T) || A) */
    {
        struct b65_blake2b ctx;

        b65_blake2b_init(&ctx, 64u);
        b65_blake2b_update(&ctx, aLen, 4u);
        b65_blake2b_update(&ctx, pA, cbA);
        b65_blake2b_final(&ctx, aV, 64u);
    }
    b65_memcpy(pOut, aV, 32u); /* W1 */

    for (i = 1; i < r; i++) {
        b65_H(aTmp, 64u, aV, 64u);
        b65_memcpy(aV, aTmp, 64u);
        b65_memcpy(pOut + i * 32u, aV, 32u); /* Wi */
    }

    /* V_{r+1} = H^(T - 32*r)(V_r) */
    cbLast = cbOut - 32u * r;
    b65_H(aTmp, cbLast, aV, 64u);
    b65_memcpy(pOut + r * 32u, aTmp, cbLast);

    b65_memzero(aV, sizeof(aV));
    b65_memzero(aTmp, sizeof(aTmp));
}

/* ---- Argon2 block / compression G -------------------------------------- */

typedef struct {
    uint64_t aV[B65_QWORDS];
} b65_block;

static void
b65_block_zero(b65_block *p)
{
    b65_memzero(p->aV, sizeof(p->aV));
}

static void
b65_block_copy(b65_block *pDst, const b65_block *pSrc)
{
    b65_memcpy(pDst->aV, pSrc->aV, sizeof(pDst->aV));
}

static void
b65_block_xor(b65_block *pDst, const b65_block *pSrc)
{
    unsigned i;

    for (i = 0; i < B65_QWORDS; i++) {
        pDst->aV[i] ^= pSrc->aV[i];
    }
}

/* BlaMka: a + b + 2*trunc32(a)*trunc32(b)  (RFC 9106 §3.6) */
static uint64_t
b65_blamka(uint64_t uA, uint64_t uB)
{
    uint64_t uA32 = uA & 0xffffffffull;
    uint64_t uB32 = uB & 0xffffffffull;

    return uA + uB + 2ull * uA32 * uB32;
}

static void
b65_GB(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
{
    *a = b65_blamka(*a, *b);
    *d = b65_rotr64(*d ^ *a, 32);
    *c = b65_blamka(*c, *d);
    *b = b65_rotr64(*b ^ *c, 24);

    *a = b65_blamka(*a, *b);
    *d = b65_rotr64(*d ^ *a, 16);
    *c = b65_blamka(*c, *d);
    *b = b65_rotr64(*b ^ *c, 63);
}

/* P: one BLAKE2b-style round on 16 words (8 x 16-byte registers). */
static void
b65_P(uint64_t *v)
{
    b65_GB(&v[0], &v[4], &v[8], &v[12]);
    b65_GB(&v[1], &v[5], &v[9], &v[13]);
    b65_GB(&v[2], &v[6], &v[10], &v[14]);
    b65_GB(&v[3], &v[7], &v[11], &v[15]);

    b65_GB(&v[0], &v[5], &v[10], &v[15]);
    b65_GB(&v[1], &v[6], &v[11], &v[12]);
    b65_GB(&v[2], &v[7], &v[8], &v[13]);
    b65_GB(&v[3], &v[4], &v[9], &v[14]);
}

/*
 * G(prev, ref) -> out.  R = prev XOR ref; apply P on 8x16-qword rows then
 * on interleaved columns (RFC 9106 §3.5 / public PHC layout); out = Z XOR R.
 * If fXorOut, also XOR existing *pOut (multi-pass overwrite-XOR).
 */
static void
b65_G(const b65_block *pPrev, const b65_block *pRef, b65_block *pOut,
      int fXorOut)
{
    b65_block blkR;
    b65_block blkTmp;
    unsigned i;

    b65_block_copy(&blkR, pRef);
    b65_block_xor(&blkR, pPrev);
    b65_block_copy(&blkTmp, &blkR);
    if (fXorOut) {
        b65_block_xor(&blkTmp, pOut);
    }

    /* Rows: (0..15), (16..31), ... (112..127) */
    for (i = 0; i < 8u; i++) {
        uint64_t *v = &blkR.aV[16u * i];

        b65_P(v);
    }

    /* Columns: (0,1,16,17,...,112,113), (2,3,18,19,...), ... */
    for (i = 0; i < 8u; i++) {
        uint64_t aTmp[16];

        aTmp[0] = blkR.aV[2u * i];
        aTmp[1] = blkR.aV[2u * i + 1u];
        aTmp[2] = blkR.aV[2u * i + 16u];
        aTmp[3] = blkR.aV[2u * i + 17u];
        aTmp[4] = blkR.aV[2u * i + 32u];
        aTmp[5] = blkR.aV[2u * i + 33u];
        aTmp[6] = blkR.aV[2u * i + 48u];
        aTmp[7] = blkR.aV[2u * i + 49u];
        aTmp[8] = blkR.aV[2u * i + 64u];
        aTmp[9] = blkR.aV[2u * i + 65u];
        aTmp[10] = blkR.aV[2u * i + 80u];
        aTmp[11] = blkR.aV[2u * i + 81u];
        aTmp[12] = blkR.aV[2u * i + 96u];
        aTmp[13] = blkR.aV[2u * i + 97u];
        aTmp[14] = blkR.aV[2u * i + 112u];
        aTmp[15] = blkR.aV[2u * i + 113u];
        b65_P(aTmp);
        blkR.aV[2u * i] = aTmp[0];
        blkR.aV[2u * i + 1u] = aTmp[1];
        blkR.aV[2u * i + 16u] = aTmp[2];
        blkR.aV[2u * i + 17u] = aTmp[3];
        blkR.aV[2u * i + 32u] = aTmp[4];
        blkR.aV[2u * i + 33u] = aTmp[5];
        blkR.aV[2u * i + 48u] = aTmp[6];
        blkR.aV[2u * i + 49u] = aTmp[7];
        blkR.aV[2u * i + 64u] = aTmp[8];
        blkR.aV[2u * i + 65u] = aTmp[9];
        blkR.aV[2u * i + 80u] = aTmp[10];
        blkR.aV[2u * i + 81u] = aTmp[11];
        blkR.aV[2u * i + 96u] = aTmp[12];
        blkR.aV[2u * i + 97u] = aTmp[13];
        blkR.aV[2u * i + 112u] = aTmp[14];
        blkR.aV[2u * i + 113u] = aTmp[15];
        b65_memzero(aTmp, sizeof(aTmp));
    }

    b65_block_copy(pOut, &blkTmp);
    b65_block_xor(pOut, &blkR);
    b65_block_zero(&blkR);
    b65_block_zero(&blkTmp);
}

/* ---- Indexing (RFC 9106 §3.4) ------------------------------------------ */

typedef struct {
    uint32_t uPass;
    uint32_t uLane;
    uint32_t uSlice;
    uint32_t uIndex; /* position within segment */
} b65_pos;

typedef struct {
    b65_block *pMem;
    uint32_t uPasses;
    uint32_t uMemBlocks; /* m' */
    uint32_t uLanes;
    uint32_t uLaneLen; /* q = m'/p */
    uint32_t uSegLen;  /* q / 4 */
    uint32_t uType;    /* 0/1/2 */
} b65_inst;

static uint32_t
b65_index_alpha(const b65_inst *pInst, const b65_pos *pPos, uint32_t uPseudo,
                int fSameLane)
{
    uint32_t uRefArea;
    uint64_t uRel;
    uint32_t uStart;
    uint64_t uX;
    uint64_t uY;

    if (pPos->uPass == 0u) {
        if (pPos->uSlice == 0u) {
            /* All previous blocks in this segment except the one being built.
             * index >= 1 for data-dependent; starting_index is 2 for pass0/slice0
             * so index-1 >= 1. */
            uRefArea = pPos->uIndex - 1u;
        } else if (fSameLane) {
            uRefArea = pPos->uSlice * pInst->uSegLen + pPos->uIndex - 1u;
        } else {
            uRefArea = pPos->uSlice * pInst->uSegLen;
            if (pPos->uIndex == 0u) {
                uRefArea -= 1u;
            }
        }
    } else if (fSameLane) {
        uRefArea = pInst->uLaneLen - pInst->uSegLen + pPos->uIndex - 1u;
    } else {
        uRefArea = pInst->uLaneLen - pInst->uSegLen;
        if (pPos->uIndex == 0u) {
            uRefArea -= 1u;
        }
    }

    /* Map uPseudo uniformly over [0, uRefArea) with bias toward recent. */
    uX = ((uint64_t)uPseudo * (uint64_t)uPseudo) >> 32;
    uY = (uRefArea * uX) >> 32;
    uRel = (uint64_t)uRefArea - 1ull - uY;

    uStart = 0u;
    if (pPos->uPass != 0u) {
        uStart = (pPos->uSlice == B65_SYNC_POINTS - 1u)
                     ? 0u
                     : (pPos->uSlice + 1u) * pInst->uSegLen;
    }

    return (uint32_t)(((uint64_t)uStart + uRel) % (uint64_t)pInst->uLaneLen);
}

static void
b65_next_addresses(b65_block *pAddr, b65_block *pInput, const b65_block *pZero)
{
    b65_block blkTmp;

    pInput->aV[6]++;
    b65_G(pZero, pInput, &blkTmp, 0);
    b65_G(pZero, &blkTmp, pAddr, 0);
    b65_block_zero(&blkTmp);
}

static void
b65_fill_segment(b65_inst *pInst, b65_pos pos)
{
    b65_block blkAddr;
    b65_block blkInput;
    b65_block blkZero;
    uint32_t uStartIdx;
    uint32_t i;
    uint32_t uPrevOff;
    uint32_t uCurrOff;
    int fDataIndep;

    b65_block_zero(&blkZero);
    b65_block_zero(&blkAddr);
    b65_block_zero(&blkInput);

    /* Argon2i always data-independent; Argon2id first half of first pass. */
    fDataIndep = 0;
    if (pInst->uType == (uint32_t)B65_ARGON2_I) {
        fDataIndep = 1;
    } else if (pInst->uType == (uint32_t)B65_ARGON2_ID) {
        if (pos.uPass == 0u && pos.uSlice < (B65_SYNC_POINTS / 2u)) {
            fDataIndep = 1;
        }
    }

    if (fDataIndep) {
        blkInput.aV[0] = (uint64_t)pos.uPass;
        blkInput.aV[1] = (uint64_t)pos.uLane;
        blkInput.aV[2] = (uint64_t)pos.uSlice;
        blkInput.aV[3] = (uint64_t)pInst->uMemBlocks;
        blkInput.aV[4] = (uint64_t)pInst->uPasses;
        blkInput.aV[5] = (uint64_t)pInst->uType;
        /* v[6] counter starts at 0; next_addresses increments before use */
    }

    uStartIdx = 0u;
    if (pos.uPass == 0u && pos.uSlice == 0u) {
        uStartIdx = 2u; /* first two blocks already from H' */
        if (fDataIndep) {
            /* First address block for indices 0..127 (use from index 2). */
            b65_next_addresses(&blkAddr, &blkInput, &blkZero);
        }
    }

    uCurrOff = pos.uLane * pInst->uLaneLen + pos.uSlice * pInst->uSegLen +
               uStartIdx;
    if (uCurrOff % pInst->uLaneLen == 0u) {
        uPrevOff = uCurrOff + pInst->uLaneLen - 1u;
    } else {
        uPrevOff = uCurrOff - 1u;
    }

    for (i = uStartIdx; i < pInst->uSegLen; i++, uCurrOff++, uPrevOff++) {
        uint64_t uPseudo;
        uint32_t uRefLane;
        uint32_t uRefIndex;
        b65_block *pRef;
        b65_block *pPrev;
        b65_block *pCurr;
        int fXor;

        if (uCurrOff % pInst->uLaneLen == 1u) {
            uPrevOff = uCurrOff - 1u;
        }

        pos.uIndex = i;

        if (fDataIndep) {
            if (i % B65_QWORDS == 0u) {
                b65_next_addresses(&blkAddr, &blkInput, &blkZero);
            }
            uPseudo = blkAddr.aV[i % B65_QWORDS];
        } else {
            uPseudo = pInst->pMem[uPrevOff].aV[0];
        }

        uRefLane = (uint32_t)(uPseudo >> 32) % pInst->uLanes;
        if (pos.uPass == 0u && pos.uSlice == 0u) {
            uRefLane = pos.uLane;
        }

        uRefIndex = b65_index_alpha(pInst, &pos,
                                    (uint32_t)(uPseudo & 0xffffffffull),
                                    uRefLane == pos.uLane);

        pRef = &pInst->pMem[uRefLane * pInst->uLaneLen + uRefIndex];
        pPrev = &pInst->pMem[uPrevOff];
        pCurr = &pInst->pMem[uCurrOff];
        fXor = (pos.uPass != 0u) ? 1 : 0;
        b65_G(pPrev, pRef, pCurr, fXor);
    }

    b65_block_zero(&blkAddr);
    b65_block_zero(&blkInput);
}

/* ---- Top-level Argon2 -------------------------------------------------- */

static int
b65_argon2_hash(uint32_t uType, const char *pPwd, size_t cbPwd,
                const void *pSalt, size_t cbSalt, uint32_t uTCost,
                uint32_t uMCostKib, uint32_t uParallel, void *pOut,
                size_t cbOut)
{
    b65_inst inst;
    b65_block *pMem = NULL;
    unsigned char aH0[64];
    unsigned char aTmp[72];
    unsigned char *pTag = NULL;
    uint32_t uMemBlocks;
    uint32_t uLaneLen;
    uint32_t uSegLen;
    uint32_t iLane;
    uint32_t uPass;
    uint32_t uSlice;
    size_t cbMem;
    int nRc = -1;

    if (pOut == NULL || cbOut < 4u || cbOut > B65_MAX_OUT) {
        errno = EINVAL;
        return -1;
    }
    if (uTCost < 1u || uParallel < 1u || uParallel > B65_MAX_LANES) {
        errno = EINVAL;
        return -1;
    }
    if (uMCostKib < 8u * uParallel) {
        errno = EINVAL;
        return -1;
    }
    if (pPwd == NULL && cbPwd != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (pSalt == NULL && cbSalt != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (cbPwd > 0xffffffffull || cbSalt > 0xffffffffull) {
        errno = EINVAL;
        return -1;
    }

    /* m' = 4 * p * floor(m / (4*p)) */
    uMemBlocks = 4u * uParallel * (uMCostKib / (4u * uParallel));
    if (uMemBlocks < 8u * uParallel) {
        errno = EINVAL;
        return -1;
    }
    uLaneLen = uMemBlocks / uParallel;
    uSegLen = uLaneLen / B65_SYNC_POINTS;
    if (uSegLen == 0u) {
        errno = EINVAL;
        return -1;
    }

    cbMem = (size_t)uMemBlocks * (size_t)B65_BLOCK_BYTES;
    if (cbMem > B65_MAX_MEM_BYTES ||
        (size_t)uMCostKib * 1024u > B65_MAX_MEM_BYTES) {
        errno = ENOMEM;
        return -1;
    }

    pMem = (b65_block *)malloc(cbMem);
    if (pMem == NULL) {
        errno = ENOMEM;
        return -1;
    }
    b65_memzero(pMem, cbMem);

    /*
     * H0 = H^64( LE32(p) || LE32(T) || LE32(m) || LE32(t) || LE32(v) ||
     *            LE32(y) || LE32(len(P)) || P || LE32(len(S)) || S ||
     *            LE32(0) || LE32(0) )   — secret and AD empty
     */
    {
        struct b65_blake2b ctx;
        unsigned char aLe[4];

        b65_blake2b_init(&ctx, 64u);
        b65_store_le32(aLe, uParallel);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, (uint32_t)cbOut);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, uMCostKib);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, uTCost);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, B65_VERSION);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, uType);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, (uint32_t)cbPwd);
        b65_blake2b_update(&ctx, aLe, 4u);
        if (cbPwd > 0u) {
            b65_blake2b_update(&ctx, pPwd, cbPwd);
        }
        b65_store_le32(aLe, (uint32_t)cbSalt);
        b65_blake2b_update(&ctx, aLe, 4u);
        if (cbSalt > 0u) {
            b65_blake2b_update(&ctx, pSalt, cbSalt);
        }
        /* length(K)=0, length(X)=0 */
        b65_store_le32(aLe, 0u);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_store_le32(aLe, 0u);
        b65_blake2b_update(&ctx, aLe, 4u);
        b65_blake2b_final(&ctx, aH0, 64u);
    }

    inst.pMem = pMem;
    inst.uPasses = uTCost;
    inst.uMemBlocks = uMemBlocks;
    inst.uLanes = uParallel;
    inst.uLaneLen = uLaneLen;
    inst.uSegLen = uSegLen;
    inst.uType = uType;

    /* B[i][0] = H'^1024(H0 || LE32(0) || LE32(i))
     * B[i][1] = H'^1024(H0 || LE32(1) || LE32(i))
     */
    for (iLane = 0; iLane < uParallel; iLane++) {
        unsigned char aBlock[B65_BLOCK_BYTES];

        b65_memcpy(aTmp, aH0, 64u);
        b65_store_le32(aTmp + 64, 0u);
        b65_store_le32(aTmp + 68, iLane);
        b65_Hprime(aBlock, B65_BLOCK_BYTES, aTmp, 72u);
        {
            unsigned k;

            for (k = 0; k < B65_QWORDS; k++) {
                pMem[iLane * uLaneLen + 0u].aV[k] =
                    b65_load_le64(aBlock + k * 8u);
            }
        }

        b65_store_le32(aTmp + 64, 1u);
        b65_store_le32(aTmp + 68, iLane);
        b65_Hprime(aBlock, B65_BLOCK_BYTES, aTmp, 72u);
        {
            unsigned k;

            for (k = 0; k < B65_QWORDS; k++) {
                pMem[iLane * uLaneLen + 1u].aV[k] =
                    b65_load_le64(aBlock + k * 8u);
            }
        }
        b65_memzero(aBlock, sizeof(aBlock));
    }

    for (uPass = 0; uPass < uTCost; uPass++) {
        for (uSlice = 0; uSlice < B65_SYNC_POINTS; uSlice++) {
            for (iLane = 0; iLane < uParallel; iLane++) {
                b65_pos pos;

                pos.uPass = uPass;
                pos.uLane = iLane;
                pos.uSlice = uSlice;
                pos.uIndex = 0;
                b65_fill_segment(&inst, pos);
            }
        }
    }

    /* C = XOR of last column */
    {
        b65_block blkC;
        unsigned char aC[B65_BLOCK_BYTES];
        unsigned k;

        b65_block_copy(&blkC, &pMem[0u * uLaneLen + (uLaneLen - 1u)]);
        for (iLane = 1; iLane < uParallel; iLane++) {
            b65_block_xor(&blkC, &pMem[iLane * uLaneLen + (uLaneLen - 1u)]);
        }
        for (k = 0; k < B65_QWORDS; k++) {
            b65_store_le64(aC + k * 8u, blkC.aV[k]);
        }
        pTag = (unsigned char *)malloc(cbOut);
        if (pTag == NULL) {
            errno = ENOMEM;
            goto cleanup;
        }
        b65_Hprime(pTag, cbOut, aC, B65_BLOCK_BYTES);
        b65_memcpy(pOut, pTag, cbOut);
        b65_memzero(aC, sizeof(aC));
        b65_block_zero(&blkC);
    }

    nRc = 0;

cleanup:
    if (pTag != NULL) {
        b65_memzero(pTag, cbOut);
        free(pTag);
    }
    if (pMem != NULL) {
        b65_memzero(pMem, cbMem);
        free(pMem);
    }
    b65_memzero(aH0, sizeof(aH0));
    b65_memzero(aTmp, sizeof(aTmp));
    return nRc;
}

/*
 * argon2id_hash — Argon2id (type y=2), RFC 9106.
 * m_cost_kib is memory in KiB. Recommended bring-up: m=8, t=1, p=1.
 * Returns 0 on success, -1 with errno on error.
 */
int
argon2id_hash(const char *pwd, size_t pwdlen, const void *salt, size_t saltlen,
              uint32_t t_cost, uint32_t m_cost_kib, uint32_t parallelism,
              void *out, size_t outlen)
{
    return b65_argon2_hash((uint32_t)B65_ARGON2_ID, pwd, pwdlen, salt, saltlen,
                           t_cost, m_cost_kib, parallelism, out, outlen);
}

/*
 * argon2i_hash — Argon2i (type y=1), RFC 9106.
 */
int
argon2i_hash(const char *pwd, size_t pwdlen, const void *salt, size_t saltlen,
             uint32_t t_cost, uint32_t m_cost_kib, uint32_t parallelism,
             void *out, size_t outlen)
{
    return b65_argon2_hash((uint32_t)B65_ARGON2_I, pwd, pwdlen, salt, saltlen,
                           t_cost, m_cost_kib, parallelism, out, outlen);
}

int __argon2id_hash(const char *pwd, size_t pwdlen, const void *salt,
                    size_t saltlen, uint32_t t_cost, uint32_t m_cost_kib,
                    uint32_t parallelism, void *out, size_t outlen)
    __attribute__((alias("argon2id_hash")));
int __argon2i_hash(const char *pwd, size_t pwdlen, const void *salt,
                   size_t saltlen, uint32_t t_cost, uint32_t m_cost_kib,
                   uint32_t parallelism, void *out, size_t outlen)
    __attribute__((alias("argon2i_hash")));
