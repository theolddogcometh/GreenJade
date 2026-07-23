/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch60: AES-GCM lite (NIST SP 800-38D) with GHASH.
 * Uses AES_set_encrypt_key / AES_encrypt from batch45 (no multi-def AES).
 * Integer/pointer only (no SSE). Clean-room from public NIST docs.
 * Does NOT define poly1305_* (batch42 owns those names).
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch60_marker[] = "libcgj-batch60";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45 / batch50. */
#define AES_MAXNR 14

typedef struct aes_key_st {
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
} AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *pKey);
void AES_encrypt(const unsigned char *pIn, unsigned char *pOut,
                 const AES_KEY *pKey);

/* ---- freestanding mem helpers ------------------------------------------ */

static void
b60_memzero(void *pDst, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static void
b60_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pOut = (unsigned char *)pDst;
    const unsigned char *pIn = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        pOut[i] = pIn[i];
    }
}

static int
b60_ct_memcmp(const unsigned char *pA, const unsigned char *pB, size_t cb)
{
    unsigned char uDiff = 0;
    size_t i;

    for (i = 0; i < cb; i++) {
        uDiff |= (unsigned char)(pA[i] ^ pB[i]);
    }
    return (int)uDiff; /* 0 iff equal */
}

static uint64_t
b60_load_be64(const unsigned char *p)
{
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b60_store_be64(unsigned char *p, uint64_t u)
{
    p[0] = (unsigned char)(u >> 56);
    p[1] = (unsigned char)(u >> 48);
    p[2] = (unsigned char)(u >> 40);
    p[3] = (unsigned char)(u >> 32);
    p[4] = (unsigned char)(u >> 24);
    p[5] = (unsigned char)(u >> 16);
    p[6] = (unsigned char)(u >> 8);
    p[7] = (unsigned char)u;
}

/* ========================================================================
 * GHASH — GF(2^128) multiply-accumulate (NIST SP 800-38D §6.4)
 * Irreducible: x^128 + x^7 + x^2 + x + 1  (R = 0xe1 || 0^120)
 * ======================================================================== */

/*
 * Multiply X * Y in GF(2^128). X/Y are big-endian field elements as two
 * uint64 halves (high = bytes 0..7, low = bytes 8..15).
 */
static void
b60_gf_mul(uint64_t *pZh, uint64_t *pZl, uint64_t uXh, uint64_t uXl,
           uint64_t uYh, uint64_t uYl)
{
    uint64_t uZh = 0;
    uint64_t uZl = 0;
    uint64_t uVh = uXh;
    uint64_t uVl = uXl;
    unsigned iBit;

    /* NIST: Z←0, V←X; for i=0..127 if Y_i then Z←Z⊕V; rightshift V (⊕R). */
    for (iBit = 0; iBit < 128u; iBit++) {
        uint64_t uYbit;

        if (iBit < 64u) {
            uYbit = (uYh >> (63u - iBit)) & 1ull;
        } else {
            uYbit = (uYl >> (127u - iBit)) & 1ull;
        }
        if (uYbit != 0ull) {
            uZh ^= uVh;
            uZl ^= uVl;
        }
        {
            uint64_t uLsb = uVl & 1ull;

            uVl = (uVl >> 1) | (uVh << 63);
            uVh = uVh >> 1;
            if (uLsb != 0ull) {
                uVh ^= 0xe100000000000000ull;
            }
        }
    }
    *pZh = uZh;
    *pZl = uZl;
}

struct b60_ghash {
    uint64_t uYh;
    uint64_t uYl;
    uint64_t uHh;
    uint64_t uHl;
    unsigned char aBuf[16];
    size_t cbBuf;
};

static void
b60_ghash_init(struct b60_ghash *pCtx, const unsigned char aH[16])
{
    pCtx->uYh = 0;
    pCtx->uYl = 0;
    pCtx->uHh = b60_load_be64(aH);
    pCtx->uHl = b60_load_be64(aH + 8);
    pCtx->cbBuf = 0;
    b60_memzero(pCtx->aBuf, 16);
}

static void
b60_ghash_block(struct b60_ghash *pCtx, const unsigned char aBlk[16])
{
    uint64_t uXh;
    uint64_t uXl;

    uXh = pCtx->uYh ^ b60_load_be64(aBlk);
    uXl = pCtx->uYl ^ b60_load_be64(aBlk + 8);
    b60_gf_mul(&pCtx->uYh, &pCtx->uYl, uXh, uXl, pCtx->uHh, pCtx->uHl);
}

static void
b60_ghash_update(struct b60_ghash *pCtx, const unsigned char *pData,
                 size_t cbData)
{
    size_t cbOff = 0;

    if (pData == NULL || cbData == 0) {
        return;
    }

    if (pCtx->cbBuf != 0) {
        size_t cbNeed = 16u - pCtx->cbBuf;
        size_t cbTake = (cbData < cbNeed) ? cbData : cbNeed;
        size_t i;

        for (i = 0; i < cbTake; i++) {
            pCtx->aBuf[pCtx->cbBuf + i] = pData[i];
        }
        pCtx->cbBuf += cbTake;
        cbOff = cbTake;
        if (pCtx->cbBuf == 16u) {
            b60_ghash_block(pCtx, pCtx->aBuf);
            pCtx->cbBuf = 0;
        }
    }

    while (cbOff + 16u <= cbData) {
        b60_ghash_block(pCtx, pData + cbOff);
        cbOff += 16u;
    }

    if (cbOff < cbData) {
        size_t cbRem = cbData - cbOff;
        size_t i;

        for (i = 0; i < cbRem; i++) {
            pCtx->aBuf[i] = pData[cbOff + i];
        }
        pCtx->cbBuf = cbRem;
    }
}

/* Zero-pad any partial block and absorb (no length block). */
static void
b60_ghash_pad(struct b60_ghash *pCtx)
{
    if (pCtx->cbBuf != 0) {
        size_t i;

        for (i = pCtx->cbBuf; i < 16u; i++) {
            pCtx->aBuf[i] = 0;
        }
        b60_ghash_block(pCtx, pCtx->aBuf);
        pCtx->cbBuf = 0;
    }
}

static void
b60_ghash_export(const struct b60_ghash *pCtx, unsigned char aOut[16])
{
    b60_store_be64(aOut, pCtx->uYh);
    b60_store_be64(aOut + 8, pCtx->uYl);
}

/*
 * One-shot GHASH_H(data) with zero-padding of a short final block.
 * Does not append bit-length blocks (caller builds full GCM input if needed).
 */
void
ghash(const unsigned char aH[16], const unsigned char *pData, size_t cbData,
      unsigned char aOut[16])
{
    struct b60_ghash stCtx;

    if (aH == NULL || aOut == NULL) {
        return;
    }
    b60_ghash_init(&stCtx, aH);
    if (pData != NULL && cbData != 0) {
        b60_ghash_update(&stCtx, pData, cbData);
    }
    b60_ghash_pad(&stCtx);
    b60_ghash_export(&stCtx, aOut);
    b60_memzero(&stCtx, sizeof(stCtx));
}

/* ========================================================================
 * AES-GCM (NIST SP 800-38D) — AES-128 only (batch45 schedule)
 * ======================================================================== */

static void
b60_inc32(unsigned char aCtr[16])
{
    int i;

    for (i = 15; i >= 12; i--) {
        aCtr[i] = (unsigned char)(aCtr[i] + 1u);
        if (aCtr[i] != 0) {
            break;
        }
    }
}

static void
b60_xor16(unsigned char *pDst, const unsigned char *pA,
          const unsigned char *pB, size_t cb)
{
    size_t i;

    for (i = 0; i < cb; i++) {
        pDst[i] = (unsigned char)(pA[i] ^ pB[i]);
    }
}

/* GCTR: encrypt with AES-CTR; counter is 32-bit BE at bytes 12..15. */
static void
b60_gctr(const AES_KEY *pKey, const unsigned char aIcb[16],
         const unsigned char *pIn, size_t cbIn, unsigned char *pOut)
{
    unsigned char aCtr[16];
    unsigned char aE[16];
    size_t cbOff = 0;

    b60_memcpy(aCtr, aIcb, 16);
    while (cbOff < cbIn) {
        size_t cbChunk = cbIn - cbOff;

        if (cbChunk > 16u) {
            cbChunk = 16u;
        }
        AES_encrypt(aCtr, aE, pKey);
        b60_xor16(pOut + cbOff, pIn + cbOff, aE, cbChunk);
        b60_inc32(aCtr);
        cbOff += cbChunk;
    }
    b60_memzero(aCtr, 16);
    b60_memzero(aE, 16);
}

/* J0 from IV (SP 800-38D §7.1). */
static void
b60_derive_j0(const unsigned char aH[16], const unsigned char *pIv,
              size_t cbIv, unsigned char aJ0[16])
{
    if (cbIv == 12u) {
        b60_memcpy(aJ0, pIv, 12);
        aJ0[12] = 0;
        aJ0[13] = 0;
        aJ0[14] = 0;
        aJ0[15] = 1;
    } else {
        struct b60_ghash stCtx;
        unsigned char aLen[16];

        b60_ghash_init(&stCtx, aH);
        b60_ghash_update(&stCtx, pIv, cbIv);
        b60_ghash_pad(&stCtx);
        b60_memzero(aLen, 16);
        b60_store_be64(aLen + 8, (uint64_t)cbIv * 8ull);
        b60_ghash_update(&stCtx, aLen, 16);
        b60_ghash_export(&stCtx, aJ0);
        b60_memzero(&stCtx, sizeof(stCtx));
        b60_memzero(aLen, 16);
    }
}

/*
 * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]_64 || [len(C)]_64)
 * lengths in bits, big-endian.
 */
static void
b60_ghash_aad_ct(const unsigned char aH[16], const unsigned char *pAad,
                 size_t cbAad, const unsigned char *pCt, size_t cbCt,
                 unsigned char aS[16])
{
    struct b60_ghash stCtx;
    unsigned char aLen[16];

    b60_ghash_init(&stCtx, aH);
    if (pAad != NULL && cbAad != 0) {
        b60_ghash_update(&stCtx, pAad, cbAad);
    }
    b60_ghash_pad(&stCtx);
    if (pCt != NULL && cbCt != 0) {
        b60_ghash_update(&stCtx, pCt, cbCt);
    }
    b60_ghash_pad(&stCtx);
    b60_store_be64(aLen, (uint64_t)cbAad * 8ull);
    b60_store_be64(aLen + 8, (uint64_t)cbCt * 8ull);
    b60_ghash_update(&stCtx, aLen, 16);
    b60_ghash_export(&stCtx, aS);
    b60_memzero(&stCtx, sizeof(stCtx));
    b60_memzero(aLen, 16);
}

static int
b60_gcm_setup(const unsigned char *pKey, size_t cbKey, const unsigned char *pIv,
              size_t cbIv, AES_KEY *pAes, unsigned char aH[16],
              unsigned char aJ0[16])
{
    unsigned char aZero[16];

    if (pKey == NULL || pIv == NULL || cbIv == 0 || cbKey != 16u) {
        return -1;
    }
    if (AES_set_encrypt_key(pKey, 128, pAes) != 0) {
        return -1;
    }
    b60_memzero(aZero, 16);
    AES_encrypt(aZero, aH, pAes);
    b60_derive_j0(aH, pIv, cbIv, aJ0);
    b60_memzero(aZero, 16);
    return 0;
}

/*
 * AES-GCM encrypt (AES-128). Writes pt_len ciphertext bytes and 16-byte tag.
 * Returns 0 on success, -1 on bad arguments / unsupported key length.
 */
int
aes_gcm_encrypt(const unsigned char *pKey, size_t cbKey,
                const unsigned char *pIv, size_t cbIv,
                const unsigned char *pAad, size_t cbAad,
                const unsigned char *pPt, size_t cbPt, unsigned char *pCt,
                unsigned char aTag[16])
{
    AES_KEY stKey;
    unsigned char aH[16];
    unsigned char aJ0[16];
    unsigned char aCtr0[16];
    unsigned char aS[16];
    unsigned char aE0[16];

    if (aTag == NULL) {
        return -1;
    }
    if (cbPt != 0 && (pPt == NULL || pCt == NULL)) {
        return -1;
    }
    if (cbAad != 0 && pAad == NULL) {
        return -1;
    }
    if (b60_gcm_setup(pKey, cbKey, pIv, cbIv, &stKey, aH, aJ0) != 0) {
        return -1;
    }

    /* C = GCTR_K(inc32(J0), P) */
    b60_memcpy(aCtr0, aJ0, 16);
    b60_inc32(aCtr0);
    if (cbPt != 0) {
        b60_gctr(&stKey, aCtr0, pPt, cbPt, pCt);
    }

    b60_ghash_aad_ct(aH, pAad, cbAad, pCt, cbPt, aS);

    /* T = MSB_128(GCTR_K(J0, S)) = AES_K(J0) XOR S */
    AES_encrypt(aJ0, aE0, &stKey);
    b60_xor16(aTag, aE0, aS, 16);

    b60_memzero(&stKey, sizeof(stKey));
    b60_memzero(aH, 16);
    b60_memzero(aJ0, 16);
    b60_memzero(aCtr0, 16);
    b60_memzero(aS, 16);
    b60_memzero(aE0, 16);
    return 0;
}

/*
 * AES-GCM decrypt (AES-128). Returns 0 if tag ok (writes plaintext),
 * -1 on bad tag or arguments. Constant-time tag compare.
 */
int
aes_gcm_decrypt(const unsigned char *pKey, size_t cbKey,
                const unsigned char *pIv, size_t cbIv,
                const unsigned char *pAad, size_t cbAad,
                const unsigned char *pCt, size_t cbCt,
                const unsigned char aTag[16], unsigned char *pPt)
{
    AES_KEY stKey;
    unsigned char aH[16];
    unsigned char aJ0[16];
    unsigned char aCtr0[16];
    unsigned char aS[16];
    unsigned char aE0[16];
    unsigned char aExpect[16];
    int nBad;

    if (aTag == NULL) {
        return -1;
    }
    if (cbCt != 0 && (pCt == NULL || pPt == NULL)) {
        return -1;
    }
    if (cbAad != 0 && pAad == NULL) {
        return -1;
    }
    if (b60_gcm_setup(pKey, cbKey, pIv, cbIv, &stKey, aH, aJ0) != 0) {
        return -1;
    }

    b60_ghash_aad_ct(aH, pAad, cbAad, pCt, cbCt, aS);
    AES_encrypt(aJ0, aE0, &stKey);
    b60_xor16(aExpect, aE0, aS, 16);
    nBad = b60_ct_memcmp(aExpect, aTag, 16);

    if (nBad == 0 && cbCt != 0) {
        b60_memcpy(aCtr0, aJ0, 16);
        b60_inc32(aCtr0);
        b60_gctr(&stKey, aCtr0, pCt, cbCt, pPt);
    }

    b60_memzero(&stKey, sizeof(stKey));
    b60_memzero(aH, 16);
    b60_memzero(aJ0, 16);
    b60_memzero(aCtr0, 16);
    b60_memzero(aS, 16);
    b60_memzero(aE0, 16);
    b60_memzero(aExpect, 16);

    return (nBad == 0) ? 0 : -1;
}

void __ghash(const unsigned char aH[16], const unsigned char *pData,
             size_t cbData, unsigned char aOut[16])
    __attribute__((alias("ghash")));
int __aes_gcm_encrypt(const unsigned char *pKey, size_t cbKey,
                      const unsigned char *pIv, size_t cbIv,
                      const unsigned char *pAad, size_t cbAad,
                      const unsigned char *pPt, size_t cbPt,
                      unsigned char *pCt, unsigned char aTag[16])
    __attribute__((alias("aes_gcm_encrypt")));
int __aes_gcm_decrypt(const unsigned char *pKey, size_t cbKey,
                      const unsigned char *pIv, size_t cbIv,
                      const unsigned char *pAad, size_t cbAad,
                      const unsigned char *pCt, size_t cbCt,
                      const unsigned char aTag[16], unsigned char *pPt)
    __attribute__((alias("aes_gcm_decrypt")));
