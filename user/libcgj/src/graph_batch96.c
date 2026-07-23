/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch96: AES-GCM fast path (AES-NI + PCLMUL).
 * Same NIST SP 800-38D AES-128 GCM shape as batch60 aes_gcm_encrypt /
 * aes_gcm_decrypt. When CPUID reports both AES-NI and PCLMULQDQ, CTR/tag
 * AES uses AES_encrypt_aesni (batch83) and GHASH uses ghash_pclmul
 * (batch85). Otherwise falls back to extern aes_gcm_encrypt / decrypt.
 *
 * Does NOT redefine: aes_gcm_encrypt/decrypt, AES_encrypt, ghash,
 * ghash_pclmul, gj_cpu_has_aesni, gj_cpu_has_pclmul.
 *
 * Surface:
 *   int aes_gcm_encrypt_fast(...);  // same args as aes_gcm_encrypt
 *   int aes_gcm_decrypt_fast(...);  // same args as aes_gcm_decrypt
 *   __aes_gcm_encrypt_fast / __aes_gcm_decrypt_fast
 *   __libcgj_batch96_marker = "libcgj-batch96"
 *
 * Clean-room freestanding; integer/pointer + calls into existing AES/GHASH.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch96_marker[] = "libcgj-batch96";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45/60/83. */
#define AES_MAXNR 14

typedef struct aes_key_st {
	unsigned int rd_key[4 * (AES_MAXNR + 1)];
	int rounds;
} AES_KEY;

/* ---- externs from prior batches (no multi-def) ------------------------- */

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *pKey);
void AES_encrypt(const unsigned char *pIn, unsigned char *pOut,
                 const AES_KEY *pKey);
int AES_encrypt_aesni(const unsigned char *pIn, unsigned char *pOut,
                      const AES_KEY *pKey);

void ghash_pclmul(const unsigned char aH[16], const unsigned char *pData,
                  size_t cbData, unsigned char aOut[16]);
/* ghash(batch60) remains the software GHASH; aes_gcm_* fallback uses it. */

int gj_cpu_has_aesni(void);
int gj_cpu_has_pclmul(void);

int aes_gcm_encrypt(const unsigned char *pKey, size_t cbKey,
                    const unsigned char *pIv, size_t cbIv,
                    const unsigned char *pAad, size_t cbAad,
                    const unsigned char *pPt, size_t cbPt,
                    unsigned char *pCt, unsigned char aTag[16]);
int aes_gcm_decrypt(const unsigned char *pKey, size_t cbKey,
                    const unsigned char *pIv, size_t cbIv,
                    const unsigned char *pAad, size_t cbAad,
                    const unsigned char *pCt, size_t cbCt,
                    const unsigned char aTag[16], unsigned char *pPt);

/* ---- freestanding helpers ---------------------------------------------- */

static void
b96_memzero(void *pDst, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0; i < cb; i++) {
		p[i] = 0;
	}
}

static void
b96_memcpy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pOut = (unsigned char *)pDst;
	const unsigned char *pIn = (const unsigned char *)pSrc;
	size_t i;

	for (i = 0; i < cb; i++) {
		pOut[i] = pIn[i];
	}
}

static int
b96_ct_memcmp(const unsigned char *pA, const unsigned char *pB, size_t cb)
{
	unsigned char uDiff = 0;
	size_t i;

	for (i = 0; i < cb; i++) {
		uDiff |= (unsigned char)(pA[i] ^ pB[i]);
	}
	return (int)uDiff;
}

static void
b96_store_be64(unsigned char *p, uint64_t u)
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

static void
b96_inc32(unsigned char aCtr[16])
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
b96_xor16(unsigned char *pDst, const unsigned char *pA,
          const unsigned char *pB, size_t cb)
{
	size_t i;

	for (i = 0; i < cb; i++) {
		pDst[i] = (unsigned char)(pA[i] ^ pB[i]);
	}
}

/* AES block encrypt: prefer AES-NI wrapper; software AES_encrypt if needed. */
static void
b96_aes_block(const unsigned char *pIn, unsigned char *pOut,
              const AES_KEY *pKey)
{
	if (AES_encrypt_aesni(pIn, pOut, pKey) != 0) {
		AES_encrypt(pIn, pOut, pKey);
	}
}

/* ========================================================================
 * Streaming GHASH via one-shot ghash_pclmul on single blocks.
 * Y ← (Y ⊕ block) · H  ≡  ghash_pclmul(H, Y⊕block, 16, Y)
 * ======================================================================== */

struct b96_ghash {
	unsigned char aY[16];
	unsigned char aH[16];
	unsigned char aBuf[16];
	size_t cbBuf;
};

static void
b96_ghash_init(struct b96_ghash *pCtx, const unsigned char aH[16])
{
	b96_memzero(pCtx->aY, 16);
	b96_memcpy(pCtx->aH, aH, 16);
	b96_memzero(pCtx->aBuf, 16);
	pCtx->cbBuf = 0;
}

static void
b96_ghash_block(struct b96_ghash *pCtx, const unsigned char aBlk[16])
{
	unsigned char aTmp[16];
	size_t i;

	for (i = 0; i < 16u; i++) {
		aTmp[i] = (unsigned char)(pCtx->aY[i] ^ aBlk[i]);
	}
	/* Y ← (Y ⊕ B) · H via one-shot GHASH of a single block. */
	ghash_pclmul(pCtx->aH, aTmp, 16, pCtx->aY);
	b96_memzero(aTmp, 16);
}

static void
b96_ghash_update(struct b96_ghash *pCtx, const unsigned char *pData,
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
			b96_ghash_block(pCtx, pCtx->aBuf);
			pCtx->cbBuf = 0;
		}
	}

	while (cbOff + 16u <= cbData) {
		b96_ghash_block(pCtx, pData + cbOff);
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

static void
b96_ghash_pad(struct b96_ghash *pCtx)
{
	if (pCtx->cbBuf != 0) {
		size_t i;

		for (i = pCtx->cbBuf; i < 16u; i++) {
			pCtx->aBuf[i] = 0;
		}
		b96_ghash_block(pCtx, pCtx->aBuf);
		pCtx->cbBuf = 0;
	}
}

static void
b96_ghash_export(const struct b96_ghash *pCtx, unsigned char aOut[16])
{
	b96_memcpy(aOut, pCtx->aY, 16);
}

/* GCTR with AES-NI (or software) block encrypt. */
static void
b96_gctr(const AES_KEY *pKey, const unsigned char aIcb[16],
         const unsigned char *pIn, size_t cbIn, unsigned char *pOut)
{
	unsigned char aCtr[16];
	unsigned char aE[16];
	size_t cbOff = 0;

	b96_memcpy(aCtr, aIcb, 16);
	while (cbOff < cbIn) {
		size_t cbChunk = cbIn - cbOff;

		if (cbChunk > 16u) {
			cbChunk = 16u;
		}
		b96_aes_block(aCtr, aE, pKey);
		b96_xor16(pOut + cbOff, pIn + cbOff, aE, cbChunk);
		b96_inc32(aCtr);
		cbOff += cbChunk;
	}
	b96_memzero(aCtr, 16);
	b96_memzero(aE, 16);
}

/* J0 from IV (SP 800-38D §7.1). */
static void
b96_derive_j0(const unsigned char aH[16], const unsigned char *pIv,
              size_t cbIv, unsigned char aJ0[16])
{
	if (cbIv == 12u) {
		b96_memcpy(aJ0, pIv, 12);
		aJ0[12] = 0;
		aJ0[13] = 0;
		aJ0[14] = 0;
		aJ0[15] = 1;
	} else {
		struct b96_ghash stCtx;
		unsigned char aLen[16];

		b96_ghash_init(&stCtx, aH);
		b96_ghash_update(&stCtx, pIv, cbIv);
		b96_ghash_pad(&stCtx);
		b96_memzero(aLen, 16);
		b96_store_be64(aLen + 8, (uint64_t)cbIv * 8ull);
		b96_ghash_update(&stCtx, aLen, 16);
		b96_ghash_export(&stCtx, aJ0);
		b96_memzero(&stCtx, sizeof(stCtx));
		b96_memzero(aLen, 16);
	}
}

/* S = GHASH_H(A || 0^v || C || 0^u || [len(A)]_64 || [len(C)]_64) */
static void
b96_ghash_aad_ct(const unsigned char aH[16], const unsigned char *pAad,
                 size_t cbAad, const unsigned char *pCt, size_t cbCt,
                 unsigned char aS[16])
{
	struct b96_ghash stCtx;
	unsigned char aLen[16];

	b96_ghash_init(&stCtx, aH);
	if (pAad != NULL && cbAad != 0) {
		b96_ghash_update(&stCtx, pAad, cbAad);
	}
	b96_ghash_pad(&stCtx);
	if (pCt != NULL && cbCt != 0) {
		b96_ghash_update(&stCtx, pCt, cbCt);
	}
	b96_ghash_pad(&stCtx);
	b96_store_be64(aLen, (uint64_t)cbAad * 8ull);
	b96_store_be64(aLen + 8, (uint64_t)cbCt * 8ull);
	b96_ghash_update(&stCtx, aLen, 16);
	b96_ghash_export(&stCtx, aS);
	b96_memzero(&stCtx, sizeof(stCtx));
	b96_memzero(aLen, 16);
}

static int
b96_gcm_setup(const unsigned char *pKey, size_t cbKey,
              const unsigned char *pIv, size_t cbIv, AES_KEY *pAes,
              unsigned char aH[16], unsigned char aJ0[16])
{
	unsigned char aZero[16];

	if (pKey == NULL || pIv == NULL || cbIv == 0 || cbKey != 16u) {
		return -1;
	}
	if (AES_set_encrypt_key(pKey, 128, pAes) != 0) {
		return -1;
	}
	b96_memzero(aZero, 16);
	b96_aes_block(aZero, aH, pAes);
	b96_derive_j0(aH, pIv, cbIv, aJ0);
	b96_memzero(aZero, 16);
	return 0;
}

static int
b96_cpu_fast(void)
{
	return (gj_cpu_has_aesni() != 0 && gj_cpu_has_pclmul() != 0) ? 1 : 0;
}

/*
 * AES-GCM encrypt (AES-128). Same shape/semantics as aes_gcm_encrypt.
 * AES-NI + PCLMUL path when both present; else software batch60.
 */
int
aes_gcm_encrypt_fast(const unsigned char *pKey, size_t cbKey,
                     const unsigned char *pIv, size_t cbIv,
                     const unsigned char *pAad, size_t cbAad,
                     const unsigned char *pPt, size_t cbPt,
                     unsigned char *pCt, unsigned char aTag[16])
{
	AES_KEY stKey;
	unsigned char aH[16];
	unsigned char aJ0[16];
	unsigned char aCtr0[16];
	unsigned char aS[16];
	unsigned char aE0[16];

	if (b96_cpu_fast() == 0) {
		return aes_gcm_encrypt(pKey, cbKey, pIv, cbIv, pAad, cbAad, pPt,
		                       cbPt, pCt, aTag);
	}

	if (aTag == NULL) {
		return -1;
	}
	if (cbPt != 0 && (pPt == NULL || pCt == NULL)) {
		return -1;
	}
	if (cbAad != 0 && pAad == NULL) {
		return -1;
	}
	if (b96_gcm_setup(pKey, cbKey, pIv, cbIv, &stKey, aH, aJ0) != 0) {
		return -1;
	}

	b96_memcpy(aCtr0, aJ0, 16);
	b96_inc32(aCtr0);
	if (cbPt != 0) {
		b96_gctr(&stKey, aCtr0, pPt, cbPt, pCt);
	}

	b96_ghash_aad_ct(aH, pAad, cbAad, pCt, cbPt, aS);
	b96_aes_block(aJ0, aE0, &stKey);
	b96_xor16(aTag, aE0, aS, 16);

	b96_memzero(&stKey, sizeof(stKey));
	b96_memzero(aH, 16);
	b96_memzero(aJ0, 16);
	b96_memzero(aCtr0, 16);
	b96_memzero(aS, 16);
	b96_memzero(aE0, 16);
	return 0;
}

/*
 * AES-GCM decrypt (AES-128). Same shape/semantics as aes_gcm_decrypt.
 * Constant-time tag compare on the fast path.
 */
int
aes_gcm_decrypt_fast(const unsigned char *pKey, size_t cbKey,
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

	if (b96_cpu_fast() == 0) {
		return aes_gcm_decrypt(pKey, cbKey, pIv, cbIv, pAad, cbAad, pCt,
		                       cbCt, aTag, pPt);
	}

	if (aTag == NULL) {
		return -1;
	}
	if (cbCt != 0 && (pCt == NULL || pPt == NULL)) {
		return -1;
	}
	if (cbAad != 0 && pAad == NULL) {
		return -1;
	}
	if (b96_gcm_setup(pKey, cbKey, pIv, cbIv, &stKey, aH, aJ0) != 0) {
		return -1;
	}

	b96_ghash_aad_ct(aH, pAad, cbAad, pCt, cbCt, aS);
	b96_aes_block(aJ0, aE0, &stKey);
	b96_xor16(aExpect, aE0, aS, 16);
	nBad = b96_ct_memcmp(aExpect, aTag, 16);

	if (nBad == 0 && cbCt != 0) {
		b96_memcpy(aCtr0, aJ0, 16);
		b96_inc32(aCtr0);
		b96_gctr(&stKey, aCtr0, pCt, cbCt, pPt);
	}

	b96_memzero(&stKey, sizeof(stKey));
	b96_memzero(aH, 16);
	b96_memzero(aJ0, 16);
	b96_memzero(aCtr0, 16);
	b96_memzero(aS, 16);
	b96_memzero(aE0, 16);
	b96_memzero(aExpect, 16);

	return (nBad == 0) ? 0 : -1;
}

int __aes_gcm_encrypt_fast(const unsigned char *pKey, size_t cbKey,
                           const unsigned char *pIv, size_t cbIv,
                           const unsigned char *pAad, size_t cbAad,
                           const unsigned char *pPt, size_t cbPt,
                           unsigned char *pCt, unsigned char aTag[16])
    __attribute__((alias("aes_gcm_encrypt_fast")));
int __aes_gcm_decrypt_fast(const unsigned char *pKey, size_t cbKey,
                           const unsigned char *pIv, size_t cbIv,
                           const unsigned char *pAad, size_t cbAad,
                           const unsigned char *pCt, size_t cbCt,
                           const unsigned char aTag[16], unsigned char *pPt)
    __attribute__((alias("aes_gcm_decrypt_fast")));
