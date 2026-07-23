/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch70: ChaCha20-Poly1305 AEAD (RFC 8439 §2.8).
 * Clean-room freestanding pure C — integer/pointer only (no SSE).
 *
 * Uses batch42 primitives (declare extern; do not redefine):
 *   chacha20_xor, chacha20_block, poly1305_auth
 *
 * Surface:
 *   chacha20_poly1305_encrypt / chacha20_poly1305_decrypt
 *   __chacha20_poly1305_encrypt / __chacha20_poly1305_decrypt
 *   __libcgj_batch70_marker = "libcgj-batch70"
 *
 * Construction (RFC 8439 §2.8):
 *   otk = first 32 bytes of ChaCha20 block(key, nonce, counter=0)
 *   ct  = ChaCha20-XOR(key, nonce, counter=1, pt)
 *   tag = Poly1305(otk, pad16(aad) || pad16(ct) || le64(aad_len)
 *                              || le64(ct_len))
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch70_marker[] = "libcgj-batch70";

/* batch42 — ChaCha20 stream + one-shot Poly1305 (do not redefine) */
int chacha20_xor(const unsigned char aKey[32], const unsigned char aNonce[12],
                 uint32_t uCounter, const unsigned char *pIn,
                 unsigned char *pOut, size_t cb);
int chacha20_block(const unsigned char aKey[32], const unsigned char aNonce[12],
                   uint32_t uCounter, unsigned char aOut[64]);
void poly1305_auth(const unsigned char aKey[32], const unsigned char *pMsg,
                   size_t cbMsg, unsigned char aOut[16]);

/* ---- freestanding helpers ---------------------------------------------- */

static void
b70_memzero(void *pDst, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static void
b70_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pOut = (unsigned char *)pDst;
    const unsigned char *pIn = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        pOut[i] = pIn[i];
    }
}

/* Constant-time: 0 iff equal. */
static int
b70_ct_memcmp(const unsigned char *pA, const unsigned char *pB, size_t cb)
{
    unsigned char uDiff = 0;
    size_t i;

    for (i = 0; i < cb; i++) {
        uDiff |= (unsigned char)(pA[i] ^ pB[i]);
    }
    return (int)uDiff;
}

static void
b70_store_le64(unsigned char *p, uint64_t u)
{
    p[0] = (unsigned char)u;
    p[1] = (unsigned char)(u >> 8);
    p[2] = (unsigned char)(u >> 16);
    p[3] = (unsigned char)(u >> 24);
    p[4] = (unsigned char)(u >> 32);
    p[5] = (unsigned char)(u >> 40);
    p[6] = (unsigned char)(u >> 48);
    p[7] = (unsigned char)(u >> 56);
}

/*
 * pad16 length: bytes of zero padding to reach a multiple of 16.
 * pad16(empty) = empty → 0.
 */
static size_t
b70_pad16(size_t cb)
{
    return (16u - (cb & 15u)) & 15u;
}

/*
 * MAC input = pad16(aad) || pad16(ct) || le64(aad_len) || le64(ct_len).
 * If total size ≤ B70_MAC_STACK, materialise and call batch42 poly1305_auth.
 * Larger inputs use private incremental Poly1305 (RFC 8439 §2.5 limbs).
 */
#define B70_MAC_STACK 4096u

/* ---- incremental Poly1305 (RFC 8439 §2.5), matches batch42 one-shot ---- */

struct b70_poly {
    uint32_t uR0, uR1, uR2, uR3, uR4;
    uint32_t uS1, uS2, uS3, uS4;
    uint32_t uH0, uH1, uH2, uH3, uH4;
    uint32_t uS0, uS1x, uS2x, uS3x; /* final s (key[16..31]) */
    unsigned char aBuf[16];
    size_t cbBuf;
    int fReady;
};

static uint32_t
b70_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b70_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

static void
b70_poly_blocks(struct b70_poly *pCtx, const unsigned char *p, size_t cBlocks,
                int fHibit)
{
    size_t iBlk;

    for (iBlk = 0; iBlk < cBlocks; iBlk++) {
        uint32_t t0 = b70_load_le32(p + 0);
        uint32_t t1 = b70_load_le32(p + 4);
        uint32_t t2 = b70_load_le32(p + 8);
        uint32_t t3 = b70_load_le32(p + 12);
        uint64_t d0, d1, d2, d3, d4;
        uint64_t c;
        uint32_t h0 = pCtx->uH0;
        uint32_t h1 = pCtx->uH1;
        uint32_t h2 = pCtx->uH2;
        uint32_t h3 = pCtx->uH3;
        uint32_t h4 = pCtx->uH4;

        h0 += t0 & 0x3ffffffu;
        h1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffffu;
        h2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffffu;
        h3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffffu;
        h4 += (t3 >> 8) | (fHibit ? (1u << 24) : 0u);

        d0 = (uint64_t)h0 * pCtx->uR0 + (uint64_t)h1 * pCtx->uS4 +
             (uint64_t)h2 * pCtx->uS3 + (uint64_t)h3 * pCtx->uS2 +
             (uint64_t)h4 * pCtx->uS1;
        d1 = (uint64_t)h0 * pCtx->uR1 + (uint64_t)h1 * pCtx->uR0 +
             (uint64_t)h2 * pCtx->uS4 + (uint64_t)h3 * pCtx->uS3 +
             (uint64_t)h4 * pCtx->uS2;
        d2 = (uint64_t)h0 * pCtx->uR2 + (uint64_t)h1 * pCtx->uR1 +
             (uint64_t)h2 * pCtx->uR0 + (uint64_t)h3 * pCtx->uS4 +
             (uint64_t)h4 * pCtx->uS3;
        d3 = (uint64_t)h0 * pCtx->uR3 + (uint64_t)h1 * pCtx->uR2 +
             (uint64_t)h2 * pCtx->uR1 + (uint64_t)h3 * pCtx->uR0 +
             (uint64_t)h4 * pCtx->uS4;
        d4 = (uint64_t)h0 * pCtx->uR4 + (uint64_t)h1 * pCtx->uR3 +
             (uint64_t)h2 * pCtx->uR2 + (uint64_t)h3 * pCtx->uR1 +
             (uint64_t)h4 * pCtx->uR0;

        c = d0 >> 26;
        h0 = (uint32_t)d0 & 0x3ffffffu;
        d1 += c;
        c = d1 >> 26;
        h1 = (uint32_t)d1 & 0x3ffffffu;
        d2 += c;
        c = d2 >> 26;
        h2 = (uint32_t)d2 & 0x3ffffffu;
        d3 += c;
        c = d3 >> 26;
        h3 = (uint32_t)d3 & 0x3ffffffu;
        d4 += c;
        c = d4 >> 26;
        h4 = (uint32_t)d4 & 0x3ffffffu;
        h0 += (uint32_t)c * 5u;
        c = h0 >> 26;
        h0 &= 0x3ffffffu;
        h1 += (uint32_t)c;

        pCtx->uH0 = h0;
        pCtx->uH1 = h1;
        pCtx->uH2 = h2;
        pCtx->uH3 = h3;
        pCtx->uH4 = h4;
        p += 16;
    }
}

static void
b70_poly_init(struct b70_poly *pCtx, const unsigned char aKey[32])
{
    uint32_t t0 = b70_load_le32(aKey + 0);
    uint32_t t1 = b70_load_le32(aKey + 4);
    uint32_t t2 = b70_load_le32(aKey + 8);
    uint32_t t3 = b70_load_le32(aKey + 12);

    pCtx->uR0 = t0 & 0x3ffffffu;
    pCtx->uR1 = ((t0 >> 26) | (t1 << 6)) & 0x3ffff03u;
    pCtx->uR2 = ((t1 >> 20) | (t2 << 12)) & 0x3ffc0ffu;
    pCtx->uR3 = ((t2 >> 14) | (t3 << 18)) & 0x3f03fffu;
    pCtx->uR4 = (t3 >> 8) & 0x00fffffu;
    pCtx->uS1 = pCtx->uR1 * 5u;
    pCtx->uS2 = pCtx->uR2 * 5u;
    pCtx->uS3 = pCtx->uR3 * 5u;
    pCtx->uS4 = pCtx->uR4 * 5u;
    pCtx->uH0 = pCtx->uH1 = pCtx->uH2 = pCtx->uH3 = pCtx->uH4 = 0;
    pCtx->uS0 = b70_load_le32(aKey + 16);
    pCtx->uS1x = b70_load_le32(aKey + 20);
    pCtx->uS2x = b70_load_le32(aKey + 24);
    pCtx->uS3x = b70_load_le32(aKey + 28);
    pCtx->cbBuf = 0;
    pCtx->fReady = 1;
    b70_memzero(pCtx->aBuf, 16);
}

static void
b70_poly_update(struct b70_poly *pCtx, const unsigned char *pData, size_t cb)
{
    size_t cbOff = 0;

    if (!pCtx->fReady || pData == NULL || cb == 0u) {
        return;
    }

    if (pCtx->cbBuf != 0u) {
        size_t cbNeed = 16u - pCtx->cbBuf;
        size_t cbTake = (cb < cbNeed) ? cb : cbNeed;
        size_t i;

        for (i = 0; i < cbTake; i++) {
            pCtx->aBuf[pCtx->cbBuf + i] = pData[i];
        }
        pCtx->cbBuf += cbTake;
        cbOff = cbTake;
        if (pCtx->cbBuf == 16u) {
            b70_poly_blocks(pCtx, pCtx->aBuf, 1u, 1);
            pCtx->cbBuf = 0;
        }
    }

    if (cbOff + 16u <= cb) {
        size_t cFull = (cb - cbOff) / 16u;

        b70_poly_blocks(pCtx, pData + cbOff, cFull, 1);
        cbOff += cFull * 16u;
    }

    if (cbOff < cb) {
        size_t cbRem = cb - cbOff;
        size_t i;

        for (i = 0; i < cbRem; i++) {
            pCtx->aBuf[i] = pData[cbOff + i];
        }
        pCtx->cbBuf = cbRem;
    }
}

static void
b70_poly_finish(struct b70_poly *pCtx, unsigned char aOut[16])
{
    uint64_t c;
    uint32_t h0, h1, h2, h3, h4;
    uint32_t g0, g1, g2, g3, g4;
    uint32_t uMask;
    uint64_t f0, f1, f2, f3;

    if (!pCtx->fReady || aOut == NULL) {
        return;
    }

    /* Final partial block: pad with 0x01 then zeros (RFC / donna style). */
    if (pCtx->cbBuf > 0u) {
        size_t i;

        pCtx->aBuf[pCtx->cbBuf] = 1u;
        for (i = pCtx->cbBuf + 1u; i < 16u; i++) {
            pCtx->aBuf[i] = 0;
        }
        /* hibit already in the 0x01 byte; do not set 1<<24 */
        b70_poly_blocks(pCtx, pCtx->aBuf, 1u, 0);
        pCtx->cbBuf = 0;
    }

    h0 = pCtx->uH0;
    h1 = pCtx->uH1;
    h2 = pCtx->uH2;
    h3 = pCtx->uH3;
    h4 = pCtx->uH4;

    c = h1 >> 26;
    h1 &= 0x3ffffffu;
    h2 += (uint32_t)c;
    c = h2 >> 26;
    h2 &= 0x3ffffffu;
    h3 += (uint32_t)c;
    c = h3 >> 26;
    h3 &= 0x3ffffffu;
    h4 += (uint32_t)c;
    c = h4 >> 26;
    h4 &= 0x3ffffffu;
    h0 += (uint32_t)c * 5u;
    c = h0 >> 26;
    h0 &= 0x3ffffffu;
    h1 += (uint32_t)c;

    g0 = h0 + 5u;
    c = g0 >> 26;
    g0 &= 0x3ffffffu;
    g1 = h1 + (uint32_t)c;
    c = g1 >> 26;
    g1 &= 0x3ffffffu;
    g2 = h2 + (uint32_t)c;
    c = g2 >> 26;
    g2 &= 0x3ffffffu;
    g3 = h3 + (uint32_t)c;
    c = g3 >> 26;
    g3 &= 0x3ffffffu;
    g4 = h4 + (uint32_t)c - (1u << 26);

    uMask = (g4 >> 31) - 1u;
    g0 &= uMask;
    g1 &= uMask;
    g2 &= uMask;
    g3 &= uMask;
    g4 &= uMask;
    uMask = ~uMask;
    h0 = (h0 & uMask) | g0;
    h1 = (h1 & uMask) | g1;
    h2 = (h2 & uMask) | g2;
    h3 = (h3 & uMask) | g3;
    h4 = (h4 & uMask) | g4;

    f0 = ((uint64_t)h0) | ((uint64_t)h1 << 26);
    f1 = ((uint64_t)(h1 >> 6)) | ((uint64_t)h2 << 20);
    f2 = ((uint64_t)(h2 >> 12)) | ((uint64_t)h3 << 14);
    f3 = ((uint64_t)(h3 >> 18)) | ((uint64_t)h4 << 8);

    f0 = (f0 & 0xffffffffull) + pCtx->uS0;
    f1 = (f1 & 0xffffffffull) + pCtx->uS1x + (f0 >> 32);
    f2 = (f2 & 0xffffffffull) + pCtx->uS2x + (f1 >> 32);
    f3 = (f3 & 0xffffffffull) + pCtx->uS3x + (f2 >> 32);

    b70_store_le32(aOut + 0, (uint32_t)f0);
    b70_store_le32(aOut + 4, (uint32_t)f1);
    b70_store_le32(aOut + 8, (uint32_t)f2);
    b70_store_le32(aOut + 12, (uint32_t)f3);

    b70_memzero(pCtx, sizeof(*pCtx));
}

/*
 * AEAD tag: Poly1305(otk, pad16(aad) || pad16(ct) || le64(aad)||le64(ct)).
 * Prefer batch42 poly1305_auth when the MAC input fits on the stack so the
 * exported one-shot path is exercised; otherwise stream.
 */
static void
b70_aead_tag(const unsigned char aOtk[32], const unsigned char *pAad,
             size_t cbAad, const unsigned char *pCt, size_t cbCt,
             unsigned char aTag[16])
{
    size_t cbAadPad = b70_pad16(cbAad);
    size_t cbCtPad = b70_pad16(cbCt);
    size_t cbMac = cbAad + cbAadPad + cbCt + cbCtPad + 16u;
    unsigned char aLen[16];
    unsigned char aZero[16];

    b70_memzero(aZero, 16);
    b70_store_le64(aLen + 0, (uint64_t)cbAad);
    b70_store_le64(aLen + 8, (uint64_t)cbCt);

    if (cbMac <= B70_MAC_STACK) {
        unsigned char aMac[B70_MAC_STACK];
        size_t cbOff = 0;

        if (cbAad != 0u && pAad != NULL) {
            b70_memcpy(aMac + cbOff, pAad, cbAad);
            cbOff += cbAad;
        }
        if (cbAadPad != 0u) {
            b70_memcpy(aMac + cbOff, aZero, cbAadPad);
            cbOff += cbAadPad;
        }
        if (cbCt != 0u && pCt != NULL) {
            b70_memcpy(aMac + cbOff, pCt, cbCt);
            cbOff += cbCt;
        }
        if (cbCtPad != 0u) {
            b70_memcpy(aMac + cbOff, aZero, cbCtPad);
            cbOff += cbCtPad;
        }
        b70_memcpy(aMac + cbOff, aLen, 16);
        cbOff += 16u;
        poly1305_auth(aOtk, aMac, cbOff, aTag);
        b70_memzero(aMac, cbOff);
    } else {
        struct b70_poly stPoly;

        b70_poly_init(&stPoly, aOtk);
        if (cbAad != 0u && pAad != NULL) {
            b70_poly_update(&stPoly, pAad, cbAad);
        }
        if (cbAadPad != 0u) {
            b70_poly_update(&stPoly, aZero, cbAadPad);
        }
        if (cbCt != 0u && pCt != NULL) {
            b70_poly_update(&stPoly, pCt, cbCt);
        }
        if (cbCtPad != 0u) {
            b70_poly_update(&stPoly, aZero, cbCtPad);
        }
        b70_poly_update(&stPoly, aLen, 16);
        b70_poly_finish(&stPoly, aTag);
    }

    b70_memzero(aLen, 16);
    b70_memzero(aZero, 16);
}

/* OTK = ChaCha20_block(key, nonce, 0)[0..31]  (RFC 8439 §2.6) */
static int
b70_poly_key_gen(const unsigned char aKey[32], const unsigned char aNonce[12],
                 unsigned char aOtk[32])
{
    unsigned char aBlock[64];
    int nRc;

    nRc = chacha20_block(aKey, aNonce, 0u, aBlock);
    if (nRc != 0) {
        b70_memzero(aBlock, 64);
        return -1;
    }
    b70_memcpy(aOtk, aBlock, 32);
    b70_memzero(aBlock, 64);
    return 0;
}

static int
b70_args_ok(const unsigned char *pKey, const unsigned char *pNonce,
            const unsigned char *pAad, size_t cbAad,
            const unsigned char *pBody, size_t cbBody,
            const unsigned char *pPeer, const unsigned char *pTag)
{
    if (pKey == NULL || pNonce == NULL || pTag == NULL) {
        return 0;
    }
    if (cbAad != 0u && pAad == NULL) {
        return 0;
    }
    if (cbBody != 0u && (pBody == NULL || pPeer == NULL)) {
        return 0;
    }
    return 1;
}

/*
 * chacha20_poly1305_encrypt — RFC 8439 AEAD encrypt.
 * Writes pt_len ciphertext bytes to ct and 16-byte tag.
 * Returns 0 on success, -1 on bad arguments.
 */
int
chacha20_poly1305_encrypt(const unsigned char aKey[32],
                          const unsigned char aNonce[12],
                          const unsigned char *pAad, size_t cbAad,
                          const unsigned char *pPt, size_t cbPt,
                          unsigned char *pCt, unsigned char aTag[16])
{
    unsigned char aOtk[32];

    if (!b70_args_ok(aKey, aNonce, pAad, cbAad, pPt, cbPt, pCt, aTag)) {
        return -1;
    }

    if (b70_poly_key_gen(aKey, aNonce, aOtk) != 0) {
        return -1;
    }

    /* Ciphertext from counter=1 (counter=0 reserved for OTK). */
    if (cbPt != 0u) {
        if (chacha20_xor(aKey, aNonce, 1u, pPt, pCt, cbPt) != 0) {
            b70_memzero(aOtk, 32);
            return -1;
        }
    }

    b70_aead_tag(aOtk, pAad, cbAad, pCt, cbPt, aTag);
    b70_memzero(aOtk, 32);
    return 0;
}

/*
 * chacha20_poly1305_decrypt — RFC 8439 AEAD decrypt.
 * Returns 0 if tag verifies (writes plaintext), -1 on bad tag or args.
 * Constant-time tag compare; plaintext only written after success.
 */
int
chacha20_poly1305_decrypt(const unsigned char aKey[32],
                          const unsigned char aNonce[12],
                          const unsigned char *pAad, size_t cbAad,
                          const unsigned char *pCt, size_t cbCt,
                          const unsigned char aTag[16], unsigned char *pPt)
{
    unsigned char aOtk[32];
    unsigned char aExpect[16];

    if (!b70_args_ok(aKey, aNonce, pAad, cbAad, pCt, cbCt, pPt, aTag)) {
        return -1;
    }

    if (b70_poly_key_gen(aKey, aNonce, aOtk) != 0) {
        return -1;
    }

    b70_aead_tag(aOtk, pAad, cbAad, pCt, cbCt, aExpect);
    b70_memzero(aOtk, 32);

    if (b70_ct_memcmp(aExpect, aTag, 16) != 0) {
        b70_memzero(aExpect, 16);
        return -1;
    }
    b70_memzero(aExpect, 16);

    if (cbCt != 0u) {
        if (chacha20_xor(aKey, aNonce, 1u, pCt, pPt, cbCt) != 0) {
            return -1;
        }
    }
    return 0;
}

int __chacha20_poly1305_encrypt(const unsigned char aKey[32],
                                const unsigned char aNonce[12],
                                const unsigned char *pAad, size_t cbAad,
                                const unsigned char *pPt, size_t cbPt,
                                unsigned char *pCt, unsigned char aTag[16])
    __attribute__((alias("chacha20_poly1305_encrypt")));

int __chacha20_poly1305_decrypt(const unsigned char aKey[32],
                                const unsigned char aNonce[12],
                                const unsigned char *pAad, size_t cbAad,
                                const unsigned char *pCt, size_t cbCt,
                                const unsigned char aTag[16],
                                unsigned char *pPt)
    __attribute__((alias("chacha20_poly1305_decrypt")));
