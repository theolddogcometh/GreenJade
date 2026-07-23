/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch58: Twofish-128 block cipher (128-bit block;
 * 128/192/256-bit keys). Clean-room from the public Schneier et al.
 * "Twofish: A 128-Bit Block Cipher" specification (15 June 1998).
 * Integer/pointer only (no SSE / no FP). Dual MIT OR Apache-2.0.
 *
 * Schedule size (document for callers):
 *   - Twofish_ctx holds K[40] subkeys + S[4] RS words + k (= N/64).
 *   - flat skey[] needs at least TWOFISH_SKEY_WORDS (45) uint32_t words:
 *       skey[0..39]  = K0..K39 (round + whitening subkeys)
 *       skey[40..43] = S vector words (up to 4; unused zeroed)
 *       skey[44]     = k in {2,3,4}
 *   sizeof(Twofish_ctx) == 45 * sizeof(uint32_t) on typical ILP32/LP64.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch58_marker[] = "libcgj-batch58";

/* ---- public schedule layout -------------------------------------------- */

#define TWOFISH_SKEY_WORDS 45u
#define TWOFISH_BLOCK_BYTES 16u

typedef struct Twofish_ctx {
	uint32_t K[40]; /* K0..K39 */
	uint32_t S[4];  /* RS-derived S vector (k words used) */
	uint32_t k;     /* N/64: 2, 3, or 4 */
} Twofish_ctx;

/* ---- local helpers ----------------------------------------------------- */

static uint32_t
b58_rol32(uint32_t u32X, unsigned n)
{
	n &= 31u;
	return (u32X << n) | (u32X >> (32u - n));
}

static uint32_t
b58_ror32(uint32_t u32X, unsigned n)
{
	n &= 31u;
	return (u32X >> n) | (u32X << (32u - n));
}

static uint32_t
b58_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0]
	     | ((uint32_t)p[1] << 8)
	     | ((uint32_t)p[2] << 16)
	     | ((uint32_t)p[3] << 24);
}

static void
b58_store_le32(unsigned char *p, uint32_t u32V)
{
	p[0] = (unsigned char)(u32V);
	p[1] = (unsigned char)(u32V >> 8);
	p[2] = (unsigned char)(u32V >> 16);
	p[3] = (unsigned char)(u32V >> 24);
}

/* ---- q0 / q1 fixed permutations (paper §4.3.5) ------------------------- */

static const unsigned char s_b58_q0_t0[16] = {
	0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2,
	0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4
};
static const unsigned char s_b58_q0_t1[16] = {
	0xE, 0xC, 0xB, 0x8, 0x1, 0x2, 0x3, 0x5,
	0xF, 0x4, 0xA, 0x6, 0x7, 0x0, 0x9, 0xD
};
static const unsigned char s_b58_q0_t2[16] = {
	0xB, 0xA, 0x5, 0xE, 0x6, 0xD, 0x9, 0x0,
	0xC, 0x8, 0xF, 0x3, 0x2, 0x4, 0x7, 0x1
};
static const unsigned char s_b58_q0_t3[16] = {
	0xD, 0x7, 0xF, 0x4, 0x1, 0x2, 0x6, 0xE,
	0x9, 0xB, 0x3, 0x0, 0x8, 0x5, 0xC, 0xA
};

static const unsigned char s_b58_q1_t0[16] = {
	0x2, 0x8, 0xB, 0xD, 0xF, 0x7, 0x6, 0xE,
	0x3, 0x1, 0x9, 0x4, 0x0, 0xA, 0xC, 0x5
};
static const unsigned char s_b58_q1_t1[16] = {
	0x1, 0xE, 0x2, 0xB, 0x4, 0xC, 0x3, 0x7,
	0x6, 0xD, 0xA, 0x5, 0xF, 0x9, 0x0, 0x8
};
static const unsigned char s_b58_q1_t2[16] = {
	0x4, 0xC, 0x7, 0x5, 0x1, 0x6, 0x9, 0xA,
	0x0, 0xE, 0xD, 0x8, 0x2, 0xB, 0x3, 0xF
};
static const unsigned char s_b58_q1_t3[16] = {
	0xB, 0x9, 0x5, 0x1, 0xC, 0x3, 0xD, 0xE,
	0x6, 0x4, 0x7, 0xF, 0x2, 0x0, 0x8, 0xA
};

static unsigned char
b58_ror4(unsigned char uNib)
{
	return (unsigned char)(((uNib >> 1) | (uNib << 3)) & 0xFu);
}

static unsigned char
b58_q_perm(unsigned char u8X,
           const unsigned char *pT0, const unsigned char *pT1,
           const unsigned char *pT2, const unsigned char *pT3)
{
	unsigned char uA0, uB0, uA1, uB1, uA2, uB2, uA3, uB3, uA4, uB4;

	uA0 = (unsigned char)(u8X >> 4);
	uB0 = (unsigned char)(u8X & 0xFu);
	uA1 = (unsigned char)(uA0 ^ uB0);
	uB1 = (unsigned char)(uA0 ^ b58_ror4(uB0) ^ ((uA0 << 3) & 0xFu));
	uA2 = pT0[uA1];
	uB2 = pT1[uB1];
	uA3 = (unsigned char)(uA2 ^ uB2);
	uB3 = (unsigned char)(uA2 ^ b58_ror4(uB2) ^ ((uA2 << 3) & 0xFu));
	uA4 = pT2[uA3];
	uB4 = pT3[uB3];
	return (unsigned char)((uB4 << 4) | uA4);
}

static unsigned char
b58_q0(unsigned char u8X)
{
	return b58_q_perm(u8X, s_b58_q0_t0, s_b58_q0_t1,
	                  s_b58_q0_t2, s_b58_q0_t3);
}

static unsigned char
b58_q1(unsigned char u8X)
{
	return b58_q_perm(u8X, s_b58_q1_t0, s_b58_q1_t1,
	                  s_b58_q1_t2, s_b58_q1_t3);
}

/* ---- GF(2^8) multiply -------------------------------------------------- */
/* MDS poly v(x) = x^8+x^6+x^5+x^3+1  → 0x169
 * RS  poly w(x) = x^8+x^6+x^3+x^2+1  → 0x14D
 */

static unsigned char
b58_gf_mul(unsigned char uA, unsigned char uB, unsigned uPoly)
{
	unsigned uP = 0;
	unsigned uAa = uA;
	unsigned uBb = uB;
	unsigned i;

	for (i = 0; i < 8u; i++) {
		if (uBb & 1u) {
			uP ^= uAa;
		}
		uBb >>= 1;
		if (uAa & 0x80u) {
			uAa = ((uAa << 1) ^ uPoly) & 0xFFu;
		} else {
			uAa = (uAa << 1) & 0xFFu;
		}
	}
	return (unsigned char)uP;
}

static unsigned char
b58_mds_mul(unsigned char uA, unsigned char uB)
{
	return b58_gf_mul(uA, uB, 0x169u);
}

static unsigned char
b58_rs_mul(unsigned char uA, unsigned char uB)
{
	return b58_gf_mul(uA, uB, 0x14Du);
}

/* MDS matrix (paper §4.2): Z = MDS · Y (little-endian word) */
static uint32_t
b58_mds(unsigned char uY0, unsigned char uY1,
        unsigned char uY2, unsigned char uY3)
{
	unsigned char uZ0, uZ1, uZ2, uZ3;

	uZ0 = (unsigned char)(b58_mds_mul(0x01u, uY0) ^ b58_mds_mul(0xEFu, uY1)
	                    ^ b58_mds_mul(0x5Bu, uY2) ^ b58_mds_mul(0x5Bu, uY3));
	uZ1 = (unsigned char)(b58_mds_mul(0x5Bu, uY0) ^ b58_mds_mul(0xEFu, uY1)
	                    ^ b58_mds_mul(0xEFu, uY2) ^ b58_mds_mul(0x01u, uY3));
	uZ2 = (unsigned char)(b58_mds_mul(0xEFu, uY0) ^ b58_mds_mul(0x5Bu, uY1)
	                    ^ b58_mds_mul(0x01u, uY2) ^ b58_mds_mul(0xEFu, uY3));
	uZ3 = (unsigned char)(b58_mds_mul(0xEFu, uY0) ^ b58_mds_mul(0x01u, uY1)
	                    ^ b58_mds_mul(0xEFu, uY2) ^ b58_mds_mul(0x5Bu, uY3));
	return (uint32_t)uZ0
	     | ((uint32_t)uZ1 << 8)
	     | ((uint32_t)uZ2 << 16)
	     | ((uint32_t)uZ3 << 24);
}

/* RS 4×8 matrix (paper §4.3); m[8] → 32-bit Si */
static const unsigned char s_b58_rs[4][8] = {
	{ 0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E },
	{ 0xA4, 0x56, 0x82, 0xF3, 0x1E, 0xC6, 0x68, 0xE5 },
	{ 0x02, 0xA1, 0xFC, 0xC1, 0x47, 0xAE, 0x3D, 0x19 },
	{ 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E, 0x03 }
};

static uint32_t
b58_rs_encode(const unsigned char aM[8])
{
	unsigned char aS[4];
	unsigned iRow, iCol;
	unsigned char uAcc;

	for (iRow = 0; iRow < 4u; iRow++) {
		uAcc = 0;
		for (iCol = 0; iCol < 8u; iCol++) {
			uAcc = (unsigned char)(uAcc
			    ^ b58_rs_mul(s_b58_rs[iRow][iCol], aM[iCol]));
		}
		aS[iRow] = uAcc;
	}
	return (uint32_t)aS[0]
	     | ((uint32_t)aS[1] << 8)
	     | ((uint32_t)aS[2] << 16)
	     | ((uint32_t)aS[3] << 24);
}

/*
 * h(X, L[0..k-1]) — paper §4.3.2 / Figure 2 / §7.2.1.
 *
 * Common tail (all k):
 *   y0 = q1[ q0[ q0[y] ^ l1 ] ^ l0 ]
 *   y1 = q0[ q0[ q1[y] ^ l1 ] ^ l0 ]
 *   y2 = q1[ q1[ q0[y] ^ l1 ] ^ l0 ]
 *   y3 = q0[ q1[ q1[y] ^ l1 ] ^ l0 ]
 * Optional k=3 / k=4 stages precede that tail; result through MDS.
 */
static uint32_t
b58_h(uint32_t u32X, const uint32_t *pL, unsigned uK)
{
	unsigned char uY0, uY1, uY2, uY3;
	unsigned char uL0, uL1, uL2, uL3;
	uint32_t u32W;

	uY0 = (unsigned char)(u32X);
	uY1 = (unsigned char)(u32X >> 8);
	uY2 = (unsigned char)(u32X >> 16);
	uY3 = (unsigned char)(u32X >> 24);

	if (uK == 4u) {
		u32W = pL[3];
		uL0 = (unsigned char)(u32W);
		uL1 = (unsigned char)(u32W >> 8);
		uL2 = (unsigned char)(u32W >> 16);
		uL3 = (unsigned char)(u32W >> 24);
		uY0 = (unsigned char)(b58_q1(uY0) ^ uL0);
		uY1 = (unsigned char)(b58_q0(uY1) ^ uL1);
		uY2 = (unsigned char)(b58_q0(uY2) ^ uL2);
		uY3 = (unsigned char)(b58_q1(uY3) ^ uL3);
	}

	if (uK >= 3u) {
		u32W = pL[2];
		uL0 = (unsigned char)(u32W);
		uL1 = (unsigned char)(u32W >> 8);
		uL2 = (unsigned char)(u32W >> 16);
		uL3 = (unsigned char)(u32W >> 24);
		uY0 = (unsigned char)(b58_q1(uY0) ^ uL0);
		uY1 = (unsigned char)(b58_q1(uY1) ^ uL1);
		uY2 = (unsigned char)(b58_q0(uY2) ^ uL2);
		uY3 = (unsigned char)(b58_q0(uY3) ^ uL3);
	}

	/*
	 * Common tail (paper §4.3.2 / §7.2.1):
	 *   y0 = q1[ q0[ q0[y] ^ l1 ] ^ l0 ]
	 *   y1 = q0[ q0[ q1[y] ^ l1 ] ^ l0 ]
	 *   y2 = q1[ q1[ q0[y] ^ l1 ] ^ l0 ]
	 *   y3 = q0[ q1[ q1[y] ^ l1 ] ^ l0 ]
	 */
	u32W = pL[1];
	uL0 = (unsigned char)(u32W);
	uL1 = (unsigned char)(u32W >> 8);
	uL2 = (unsigned char)(u32W >> 16);
	uL3 = (unsigned char)(u32W >> 24);
	uY0 = (unsigned char)(b58_q0(uY0) ^ uL0);
	uY1 = (unsigned char)(b58_q1(uY1) ^ uL1);
	uY2 = (unsigned char)(b58_q0(uY2) ^ uL2);
	uY3 = (unsigned char)(b58_q1(uY3) ^ uL3);

	u32W = pL[0];
	uL0 = (unsigned char)(u32W);
	uL1 = (unsigned char)(u32W >> 8);
	uL2 = (unsigned char)(u32W >> 16);
	uL3 = (unsigned char)(u32W >> 24);
	uY0 = (unsigned char)(b58_q0(uY0) ^ uL0);
	uY1 = (unsigned char)(b58_q0(uY1) ^ uL1);
	uY2 = (unsigned char)(b58_q1(uY2) ^ uL2);
	uY3 = (unsigned char)(b58_q1(uY3) ^ uL3);

	/* outer q before MDS */
	uY0 = b58_q1(uY0);
	uY1 = b58_q0(uY1);
	uY2 = b58_q1(uY2);
	uY3 = b58_q0(uY3);

	return b58_mds(uY0, uY1, uY2, uY3);
}

static uint32_t
b58_g(const Twofish_ctx *pCtx, uint32_t u32X)
{
	return b58_h(u32X, pCtx->S, (unsigned)pCtx->k);
}

/* ---- key setup --------------------------------------------------------- */

static void
b58_ctx_zero(Twofish_ctx *pCtx)
{
	unsigned i;

	for (i = 0; i < 40u; i++) {
		pCtx->K[i] = 0;
	}
	for (i = 0; i < 4u; i++) {
		pCtx->S[i] = 0;
	}
	pCtx->k = 0;
}

void
Twofish_set_key(void *pCtxIn, const unsigned char *pKey, unsigned uBits)
{
	Twofish_ctx *pCtx = (Twofish_ctx *)pCtxIn;
	unsigned char aKey[32];
	uint32_t aMe[4], aMo[4], aSi[4];
	unsigned uK, uKeyBytes, uPadTo, i, j;
	uint32_t u32A, u32B;
	const uint32_t u32Rho = 0x01010101u;

	if (pCtx == NULL) {
		return;
	}
	b58_ctx_zero(pCtx);
	if (pKey == NULL || uBits == 0u || uBits > 256u) {
		return;
	}

	/* Pad to next defined size N ∈ {128,192,256}; k = N/64 */
	if (uBits <= 128u) {
		uK = 2u;
	} else if (uBits <= 192u) {
		uK = 3u;
	} else {
		uK = 4u;
	}
	uPadTo = uK * 8u;
	uKeyBytes = (uBits + 7u) / 8u;
	if (uKeyBytes > uPadTo) {
		uKeyBytes = uPadTo;
	}

	for (i = 0; i < 32u; i++) {
		aKey[i] = 0;
	}
	for (i = 0; i < uKeyBytes; i++) {
		aKey[i] = pKey[i];
	}

	pCtx->k = uK;

	/* Me / Mo from even/odd 32-bit little-endian words of key */
	for (i = 0; i < uK; i++) {
		aMe[i] = b58_load_le32(aKey + (8u * i));
		aMo[i] = b58_load_le32(aKey + (8u * i) + 4u);
	}

	/* RS → Si for i=0..k-1; S = (S_{k-1}, ..., S_0) */
	for (i = 0; i < uK; i++) {
		aSi[i] = b58_rs_encode(aKey + (8u * i));
	}
	for (i = 0; i < uK; i++) {
		pCtx->S[i] = aSi[uK - 1u - i];
	}

	/* Expanded subkeys K0..K39 via h */
	for (i = 0; i < 20u; i++) {
		u32A = b58_h((2u * i) * u32Rho, aMe, uK);
		u32B = b58_rol32(b58_h((2u * i + 1u) * u32Rho, aMo, uK), 8u);
		pCtx->K[2u * i] = u32A + u32B;
		pCtx->K[2u * i + 1u] = b58_rol32(u32A + 2u * u32B, 9u);
	}

	/* wipe stack key material */
	for (i = 0; i < 32u; i++) {
		aKey[i] = 0;
	}
	for (j = 0; j < 4u; j++) {
		aMe[j] = 0;
		aMo[j] = 0;
		aSi[j] = 0;
	}
}

void
Twofish_encrypt(const void *pCtxIn, const unsigned char aIn[16],
                unsigned char aOut[16])
{
	const Twofish_ctx *pCtx = (const Twofish_ctx *)pCtxIn;
	uint32_t uR0, uR1, uR2, uR3;
	uint32_t uT0, uT1, uF0, uF1, uTmp0, uTmp1;
	unsigned uR;

	if (pCtx == NULL || aIn == NULL || aOut == NULL) {
		return;
	}
	if (pCtx->k < 2u || pCtx->k > 4u) {
		return;
	}

	uR0 = b58_load_le32(aIn) ^ pCtx->K[0];
	uR1 = b58_load_le32(aIn + 4) ^ pCtx->K[1];
	uR2 = b58_load_le32(aIn + 8) ^ pCtx->K[2];
	uR3 = b58_load_le32(aIn + 12) ^ pCtx->K[3];

	for (uR = 0; uR < 16u; uR++) {
		uT0 = b58_g(pCtx, uR0);
		uT1 = b58_g(pCtx, b58_rol32(uR1, 8u));
		uF0 = uT0 + uT1 + pCtx->K[2u * uR + 8u];
		uF1 = uT0 + 2u * uT1 + pCtx->K[2u * uR + 9u];

		uR2 = b58_ror32(uR2 ^ uF0, 1u);
		uR3 = b58_rol32(uR3, 1u) ^ uF1;

		/* swap halves (undone after last round) */
		uTmp0 = uR0;
		uTmp1 = uR1;
		uR0 = uR2;
		uR1 = uR3;
		uR2 = uTmp0;
		uR3 = uTmp1;
	}

	/* undo last swap + output whitening */
	b58_store_le32(aOut, uR2 ^ pCtx->K[4]);
	b58_store_le32(aOut + 4, uR3 ^ pCtx->K[5]);
	b58_store_le32(aOut + 8, uR0 ^ pCtx->K[6]);
	b58_store_le32(aOut + 12, uR1 ^ pCtx->K[7]);
}

void
Twofish_decrypt(const void *pCtxIn, const unsigned char aIn[16],
                unsigned char aOut[16])
{
	const Twofish_ctx *pCtx = (const Twofish_ctx *)pCtxIn;
	uint32_t uR0, uR1, uR2, uR3;
	uint32_t uT0, uT1, uF0, uF1, uTmp0, uTmp1;
	unsigned uR;

	if (pCtx == NULL || aIn == NULL || aOut == NULL) {
		return;
	}
	if (pCtx->k < 2u || pCtx->k > 4u) {
		return;
	}

	/* strip output whitening into post-final-swap word order */
	uR2 = b58_load_le32(aIn) ^ pCtx->K[4];
	uR3 = b58_load_le32(aIn + 4) ^ pCtx->K[5];
	uR0 = b58_load_le32(aIn + 8) ^ pCtx->K[6];
	uR1 = b58_load_le32(aIn + 12) ^ pCtx->K[7];

	for (uR = 16u; uR > 0u; uR--) {
		/* inverse swap */
		uTmp0 = uR0;
		uTmp1 = uR1;
		uR0 = uR2;
		uR1 = uR3;
		uR2 = uTmp0;
		uR3 = uTmp1;

		uT0 = b58_g(pCtx, uR0);
		uT1 = b58_g(pCtx, b58_rol32(uR1, 8u));
		uF0 = uT0 + uT1 + pCtx->K[2u * (uR - 1u) + 8u];
		uF1 = uT0 + 2u * uT1 + pCtx->K[2u * (uR - 1u) + 9u];

		uR2 = b58_rol32(uR2, 1u) ^ uF0;
		uR3 = b58_ror32(uR3 ^ uF1, 1u);
	}

	b58_store_le32(aOut, uR0 ^ pCtx->K[0]);
	b58_store_le32(aOut + 4, uR1 ^ pCtx->K[1]);
	b58_store_le32(aOut + 8, uR2 ^ pCtx->K[2]);
	b58_store_le32(aOut + 12, uR3 ^ pCtx->K[3]);
}

/* ---- flat skey API (same layout as Twofish_ctx) ------------------------ */

void
twofish_setkey(const unsigned char *pKey, unsigned uKeyBits, uint32_t *pSkey)
{
	/* pSkey must provide at least TWOFISH_SKEY_WORDS uint32_t words. */
	Twofish_set_key((void *)pSkey, pKey, uKeyBits);
}

void
twofish_encrypt(const uint32_t *pSkey, const unsigned char aIn[16],
                unsigned char aOut[16])
{
	Twofish_encrypt((const void *)pSkey, aIn, aOut);
}

void
twofish_decrypt(const uint32_t *pSkey, const unsigned char aIn[16],
                unsigned char aOut[16])
{
	Twofish_decrypt((const void *)pSkey, aIn, aOut);
}

/* ---- __ aliases -------------------------------------------------------- */

void __Twofish_set_key(void *pCtx, const unsigned char *pKey, unsigned uBits)
    __attribute__((alias("Twofish_set_key")));
void __Twofish_encrypt(const void *pCtx, const unsigned char aIn[16],
                       unsigned char aOut[16])
    __attribute__((alias("Twofish_encrypt")));
void __Twofish_decrypt(const void *pCtx, const unsigned char aIn[16],
                       unsigned char aOut[16])
    __attribute__((alias("Twofish_decrypt")));

void __twofish_setkey(const unsigned char *pKey, unsigned uKeyBits,
                      uint32_t *pSkey)
    __attribute__((alias("twofish_setkey")));
void __twofish_encrypt(const uint32_t *pSkey, const unsigned char aIn[16],
                       unsigned char aOut[16])
    __attribute__((alias("twofish_encrypt")));
void __twofish_decrypt(const uint32_t *pSkey, const unsigned char aIn[16],
                       unsigned char aOut[16])
    __attribute__((alias("twofish_decrypt")));
