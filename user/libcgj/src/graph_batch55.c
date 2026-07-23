/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch55: Whirlpool (ISO/IEC 10118-3) 512-bit digest.
 * Integer/pointer only (no SSE). Clean-room from public specification;
 * S-box and MDS matrix are the ISO final (2003) parameters.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const char __libcgj_batch55_marker[] = "libcgj-batch55";

/* ---- Whirlpool (ISO/IEC 10118-3 final) ---------------------------------- */

#define B55_WP_R 10
#define B55_WP_N 64

struct b55_whirlpool {
	unsigned char aH[B55_WP_N];
	unsigned char aBuf[B55_WP_N];
	size_t cbBuf;
	/* 256-bit bit-length, little-endian 64-bit limbs (aBitLen[0] = LSW). */
	uint64_t aBitLen[4];
};

/* ISO final S-box (public NESSIE / ISO table). */
static const unsigned char s_b55_s[256] = {
	0x18u, 0x23u, 0xc6u, 0xe8u, 0x87u, 0xb8u, 0x01u, 0x4fu, 0x36u, 0xa6u,
	0xd2u, 0xf5u, 0x79u, 0x6fu, 0x91u, 0x52u, 0x60u, 0xbcu, 0x9bu, 0x8eu,
	0xa3u, 0x0cu, 0x7bu, 0x35u, 0x1du, 0xe0u, 0xd7u, 0xc2u, 0x2eu, 0x4bu,
	0xfeu, 0x57u, 0x15u, 0x77u, 0x37u, 0xe5u, 0x9fu, 0xf0u, 0x4au, 0xdau,
	0x58u, 0xc9u, 0x29u, 0x0au, 0xb1u, 0xa0u, 0x6bu, 0x85u, 0xbdu, 0x5du,
	0x10u, 0xf4u, 0xcbu, 0x3eu, 0x05u, 0x67u, 0xe4u, 0x27u, 0x41u, 0x8bu,
	0xa7u, 0x7du, 0x95u, 0xd8u, 0xfbu, 0xeeu, 0x7cu, 0x66u, 0xddu, 0x17u,
	0x47u, 0x9eu, 0xcau, 0x2du, 0xbfu, 0x07u, 0xadu, 0x5au, 0x83u, 0x33u,
	0x63u, 0x02u, 0xaau, 0x71u, 0xc8u, 0x19u, 0x49u, 0xd9u, 0xf2u, 0xe3u,
	0x5bu, 0x88u, 0x9au, 0x26u, 0x32u, 0xb0u, 0xe9u, 0x0fu, 0xd5u, 0x80u,
	0xbeu, 0xcdu, 0x34u, 0x48u, 0xffu, 0x7au, 0x90u, 0x5fu, 0x20u, 0x68u,
	0x1au, 0xaeu, 0xb4u, 0x54u, 0x93u, 0x22u, 0x64u, 0xf1u, 0x73u, 0x12u,
	0x40u, 0x08u, 0xc3u, 0xecu, 0xdbu, 0xa1u, 0x8du, 0x3du, 0x97u, 0x00u,
	0xcfu, 0x2bu, 0x76u, 0x82u, 0xd6u, 0x1bu, 0xb5u, 0xafu, 0x6au, 0x50u,
	0x45u, 0xf3u, 0x30u, 0xefu, 0x3fu, 0x55u, 0xa2u, 0xeau, 0x65u, 0xbau,
	0x2fu, 0xc0u, 0xdeu, 0x1cu, 0xfdu, 0x4du, 0x92u, 0x75u, 0x06u, 0x8au,
	0xb2u, 0xe6u, 0x0eu, 0x1fu, 0x62u, 0xd4u, 0xa8u, 0x96u, 0xf9u, 0xc5u,
	0x25u, 0x59u, 0x84u, 0x72u, 0x39u, 0x4cu, 0x5eu, 0x78u, 0x38u, 0x8cu,
	0xd1u, 0xa5u, 0xe2u, 0x61u, 0xb3u, 0x21u, 0x9cu, 0x1eu, 0x43u, 0xc7u,
	0xfcu, 0x04u, 0x51u, 0x99u, 0x6du, 0x0du, 0xfau, 0xdfu, 0x7eu, 0x24u,
	0x3bu, 0xabu, 0xceu, 0x11u, 0x8fu, 0x4eu, 0xb7u, 0xebu, 0x3cu, 0x81u,
	0x94u, 0xf7u, 0xb9u, 0x13u, 0x2cu, 0xd3u, 0xe7u, 0x6eu, 0xc4u, 0x03u,
	0x56u, 0x44u, 0x7fu, 0xa9u, 0x2au, 0xbbu, 0xc1u, 0x53u, 0xdcu, 0x0bu,
	0x9du, 0x6cu, 0x31u, 0x74u, 0xf6u, 0x46u, 0xacu, 0x89u, 0x14u, 0xe1u,
	0x16u, 0x3au, 0x69u, 0x09u, 0x70u, 0xb6u, 0xd0u, 0xedu, 0xccu, 0x42u,
	0x98u, 0xa4u, 0x28u, 0x5cu, 0xf8u, 0x86u};

/* Circulant first row of the MDS matrix (ISO final: 1,1,4,1,8,5,2,9). */
static const unsigned char s_b55_cir[8] = {
	0x01u, 0x01u, 0x04u, 0x01u, 0x08u, 0x05u, 0x02u, 0x09u};

/* GF(2^8) mul, reduction poly x^8+x^4+x^3+x^2+1 (0x11d). */
static unsigned char
b55_gf_mul(unsigned char uA, unsigned char uB)
{
	unsigned char uP = 0;
	unsigned i;

	for (i = 0; i < 8u; i++) {
		if ((uB & 1u) != 0u) {
			uP = (unsigned char)(uP ^ uA);
		}
		{
			unsigned char uHi = (unsigned char)(uA & 0x80u);

			uA = (unsigned char)(uA << 1);
			if (uHi != 0u) {
				uA = (unsigned char)(uA ^ 0x1du);
			}
		}
		uB = (unsigned char)(uB >> 1);
	}
	return uP;
}

/* rho[k] = sigma[k] o theta o pi o gamma */
static void
b55_rho(unsigned char aSt[B55_WP_N], const unsigned char aK[B55_WP_N])
{
	unsigned char aTmp[B55_WP_N];
	unsigned iRow;
	unsigned iCol;
	unsigned iK;

	/* gamma: SubBytes */
	for (iK = 0; iK < B55_WP_N; iK++) {
		aSt[iK] = s_b55_s[aSt[iK]];
	}

	/* pi: ShiftColumns — b[i][j] = a[(i-j) mod 8][j] */
	for (iRow = 0; iRow < 8u; iRow++) {
		for (iCol = 0; iCol < 8u; iCol++) {
			unsigned iSrc = (unsigned)((iRow + 8u - iCol) & 7u);

			aTmp[iRow * 8u + iCol] = aSt[iSrc * 8u + iCol];
		}
	}

	/* theta: MixRows — right-multiply by circulant C */
	for (iRow = 0; iRow < 8u; iRow++) {
		for (iCol = 0; iCol < 8u; iCol++) {
			unsigned char uS = 0;

			for (iK = 0; iK < 8u; iK++) {
				/* C[k][j] = CIR[(j-k) mod 8] */
				unsigned char uC =
				    s_b55_cir[(iCol + 8u - iK) & 7u];

				uS = (unsigned char)(uS ^
				                     b55_gf_mul(aTmp[iRow * 8u + iK],
				                                uC));
			}
			aSt[iRow * 8u + iCol] = uS;
		}
	}

	/* sigma: AddRoundKey */
	for (iK = 0; iK < B55_WP_N; iK++) {
		aSt[iK] = (unsigned char)(aSt[iK] ^ aK[iK]);
	}
}

/* Miyaguchi–Preneel: H' = W[H](M) xor H xor M. */
static void
b55_compress(unsigned char aH[B55_WP_N], const unsigned char aM[B55_WP_N])
{
	unsigned char aK[B55_WP_N];
	unsigned char aState[B55_WP_N];
	unsigned char aRc[B55_WP_N];
	unsigned iR;
	unsigned i;

	memcpy(aK, aH, B55_WP_N);
	for (i = 0; i < B55_WP_N; i++) {
		aState[i] = (unsigned char)(aM[i] ^ aK[i]);
	}

	for (iR = 1u; iR <= (unsigned)B55_WP_R; iR++) {
		memset(aRc, 0, B55_WP_N);
		for (i = 0; i < 8u; i++) {
			aRc[i] = s_b55_s[8u * (iR - 1u) + i];
		}
		b55_rho(aK, aRc);
		b55_rho(aState, aK);
	}

	for (i = 0; i < B55_WP_N; i++) {
		aH[i] = (unsigned char)(aState[i] ^ aH[i] ^ aM[i]);
	}
}

/* Add cb message bytes to the 256-bit bit-length counter. */
static void
b55_add_bits(struct b55_whirlpool *p, size_t cb)
{
	uint64_t uLo = ((uint64_t)cb) << 3;
	uint64_t uHi = ((uint64_t)cb) >> 61;
	uint64_t uOld;
	unsigned i;

	uOld = p->aBitLen[0];
	p->aBitLen[0] = uOld + uLo;
	if (p->aBitLen[0] < uOld) {
		uHi++;
	}
	for (i = 1u; i < 4u; i++) {
		uOld = p->aBitLen[i];
		p->aBitLen[i] = uOld + uHi;
		if (p->aBitLen[i] >= uOld) {
			return;
		}
		uHi = 1;
	}
}

void
WHIRLPOOL_Init(void *pCtx)
{
	if (pCtx != NULL) {
		memset(pCtx, 0, sizeof(struct b55_whirlpool));
	}
}

void
WHIRLPOOL_Update(void *pCtx, const void *pData, size_t cbLen)
{
	struct b55_whirlpool *p = (struct b55_whirlpool *)pCtx;
	const unsigned char *pQ = (const unsigned char *)pData;
	size_t cb;

	if (p == NULL || (pData == NULL && cbLen != 0u)) {
		return;
	}
	b55_add_bits(p, cbLen);

	while (cbLen > 0u) {
		cb = B55_WP_N - p->cbBuf;
		if (cb > cbLen) {
			cb = cbLen;
		}
		memcpy(p->aBuf + p->cbBuf, pQ, cb);
		p->cbBuf += cb;
		pQ += cb;
		cbLen -= cb;
		if (p->cbBuf == B55_WP_N) {
			b55_compress(p->aH, p->aBuf);
			p->cbBuf = 0;
		}
	}
}

void
WHIRLPOOL_Final(unsigned char aMd[64], void *pCtx)
{
	struct b55_whirlpool *p = (struct b55_whirlpool *)pCtx;
	unsigned char aLenBe[32];
	unsigned iLimb;
	unsigned iByte;
	size_t cbBuf;

	if (p == NULL || aMd == NULL) {
		return;
	}

	/* Snapshot 256-bit BE bit length (pre-padding). */
	for (iLimb = 0; iLimb < 4u; iLimb++) {
		uint64_t uV = p->aBitLen[3u - iLimb];

		for (iByte = 0; iByte < 8u; iByte++) {
			aLenBe[iLimb * 8u + iByte] =
			    (unsigned char)(uV >> (56u - iByte * 8u));
		}
	}

	/*
	 * Pad without touching aBitLen: 0x80, zeros so room for 32-byte
	 * length remains (or spill into next block), then append length.
	 */
	cbBuf = p->cbBuf;
	p->aBuf[cbBuf++] = 0x80u;
	if (cbBuf > 32u) {
		while (cbBuf < B55_WP_N) {
			p->aBuf[cbBuf++] = 0;
		}
		b55_compress(p->aH, p->aBuf);
		cbBuf = 0;
	}
	while (cbBuf < 32u) {
		p->aBuf[cbBuf++] = 0;
	}
	memcpy(p->aBuf + 32, aLenBe, 32u);
	b55_compress(p->aH, p->aBuf);

	memcpy(aMd, p->aH, B55_WP_N);
	memset(p, 0, sizeof(*p));
}

unsigned char *
WHIRLPOOL(const unsigned char *pData, size_t cbLen, unsigned char aMd[64])
{
	struct b55_whirlpool ctx;

	if (aMd == NULL) {
		return NULL;
	}
	WHIRLPOOL_Init(&ctx);
	WHIRLPOOL_Update(&ctx, pData, cbLen);
	WHIRLPOOL_Final(aMd, &ctx);
	return aMd;
}

void __WHIRLPOOL_Init(void *pCtx) __attribute__((alias("WHIRLPOOL_Init")));
void __WHIRLPOOL_Update(void *pCtx, const void *pData, size_t cbLen)
    __attribute__((alias("WHIRLPOOL_Update")));
void __WHIRLPOOL_Final(unsigned char aMd[64], void *pCtx)
    __attribute__((alias("WHIRLPOOL_Final")));
unsigned char *__WHIRLPOOL(const unsigned char *pData, size_t cbLen,
                           unsigned char aMd[64])
    __attribute__((alias("WHIRLPOOL")));
