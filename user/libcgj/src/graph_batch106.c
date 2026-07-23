/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch106: underscored arc4random family.
 *
 * Public getrandom / arc4random / arc4random_buf / arc4random_uniform already
 * live in syscall_linux.c and stdlib.c — this TU does NOT redefine them.
 * Fills the missing glibc-shaped underscored gap with a clean-room ChaCha20
 * CSPRNG (RFC 8439 §2.3 block), seeded by getrandom when linked, else a
 * weak local mix for freestanding bring-up.
 *
 * Surface (unique — no multi-def with existing public symbols):
 *   void     __arc4random_buf(void *pBuf, size_t cb);
 *   uint32_t __arc4random(void);
 *   uint32_t __arc4random_uniform(uint32_t uUpper);
 *   void     __arc4random_addrandom(unsigned char *pDat, int nDat);
 *   __libcgj_batch106_marker = "libcgj-batch106"
 *
 * Integer/pointer only (no SSE required; -msse2 build is fine).
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch106_marker[] = "libcgj-batch106";

/* Seed helper when product getrandom is linked (syscall_linux.c). */
ssize_t getrandom(void *pBuf, size_t cb, unsigned int uFlags);

/* ---- ChaCha20 quarter-round / block (RFC 8439) ------------------------- */

static uint32_t
b106_rol32(uint32_t uX, unsigned n)
{
	return (uX << n) | (uX >> (32u - n));
}

static uint32_t
b106_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static void
b106_store_le32(unsigned char *p, uint32_t uX)
{
	p[0] = (unsigned char)uX;
	p[1] = (unsigned char)(uX >> 8);
	p[2] = (unsigned char)(uX >> 16);
	p[3] = (unsigned char)(uX >> 24);
}

static void
b106_qr(uint32_t *pA, uint32_t *pB, uint32_t *pC, uint32_t *pD)
{
	*pA += *pB;
	*pD ^= *pA;
	*pD = b106_rol32(*pD, 16);
	*pC += *pD;
	*pB ^= *pC;
	*pB = b106_rol32(*pB, 12);
	*pA += *pB;
	*pD ^= *pA;
	*pD = b106_rol32(*pD, 8);
	*pC += *pD;
	*pB ^= *pC;
	*pB = b106_rol32(*pB, 7);
}

/*
 * One 64-byte keystream block into aOut from key/nonce/counter.
 * aKey: 32 bytes, aNonce: 12 bytes (IETF).
 */
static void
b106_chacha20_block(const unsigned char aKey[32],
                    const unsigned char aNonce[12], uint32_t uCounter,
                    unsigned char aOut[64])
{
	uint32_t aX[16];
	uint32_t aW[16];
	unsigned iRound;
	unsigned i;

	aX[0] = 0x61707865u;
	aX[1] = 0x3320646eu;
	aX[2] = 0x79622d32u;
	aX[3] = 0x6b206574u;
	aX[4] = b106_load_le32(aKey + 0);
	aX[5] = b106_load_le32(aKey + 4);
	aX[6] = b106_load_le32(aKey + 8);
	aX[7] = b106_load_le32(aKey + 12);
	aX[8] = b106_load_le32(aKey + 16);
	aX[9] = b106_load_le32(aKey + 20);
	aX[10] = b106_load_le32(aKey + 24);
	aX[11] = b106_load_le32(aKey + 28);
	aX[12] = uCounter;
	aX[13] = b106_load_le32(aNonce + 0);
	aX[14] = b106_load_le32(aNonce + 4);
	aX[15] = b106_load_le32(aNonce + 8);

	for (i = 0; i < 16u; i++) {
		aW[i] = aX[i];
	}
	for (iRound = 0; iRound < 10u; iRound++) {
		b106_qr(&aW[0], &aW[4], &aW[8], &aW[12]);
		b106_qr(&aW[1], &aW[5], &aW[9], &aW[13]);
		b106_qr(&aW[2], &aW[6], &aW[10], &aW[14]);
		b106_qr(&aW[3], &aW[7], &aW[11], &aW[15]);
		b106_qr(&aW[0], &aW[5], &aW[10], &aW[15]);
		b106_qr(&aW[1], &aW[6], &aW[11], &aW[12]);
		b106_qr(&aW[2], &aW[7], &aW[8], &aW[13]);
		b106_qr(&aW[3], &aW[4], &aW[9], &aW[14]);
	}
	for (i = 0; i < 16u; i++) {
		b106_store_le32(aOut + i * 4u, aW[i] + aX[i]);
	}
}

/* ---- CSPRNG state ------------------------------------------------------ */

struct b106_rng {
	unsigned char aKey[32];
	unsigned char aNonce[12];
	uint32_t uCounter;
	unsigned char aBuf[64];
	unsigned cbHave; /* valid bytes remaining at end of aBuf */
	unsigned fInit;
	uint64_t cbSinceRekey;
};

static struct b106_rng g_b106Rng;

/* Rekey after this many output bytes (OpenBSD-shaped hygiene). */
#define B106_REKEY_BYTES (1024u * 1024u)

static void
b106_zero(void *p, size_t cb)
{
	volatile unsigned char *pV = (volatile unsigned char *)p;
	size_t i;

	for (i = 0; i < cb; i++) {
		pV[i] = 0;
	}
}

/*
 * Weak freestanding seed when getrandom is unavailable.
 * Mixes stack address, static state address, and x86 TSC if present.
 */
static void
b106_weak_seed(unsigned char *pOut, size_t cb)
{
	uint64_t u = 0xA5A5C3C3D2D2E1E1ull;
	size_t i;
	unsigned char aTmp;

#if defined(__x86_64__) || defined(__i386__)
	{
		uint32_t uLo = 0, uHi = 0;
		__asm__ volatile("rdtsc" : "=a"(uLo), "=d"(uHi));
		u ^= ((uint64_t)uHi << 32) | (uint64_t)uLo;
	}
#endif
	u ^= (uint64_t)(uintptr_t)pOut;
	u ^= (uint64_t)(uintptr_t)&g_b106Rng;
	u ^= (uint64_t)(uintptr_t)&b106_weak_seed;
	aTmp = 0;
	u ^= (uint64_t)(uintptr_t)&aTmp;

	for (i = 0; i < cb; i++) {
		u = u * 6364136223846793005ull + 1ull;
		pOut[i] = (unsigned char)(u >> 56);
	}
}

/* Fill pOut fully: prefer getrandom, else weak mix. */
static void
b106_entropy(unsigned char *pOut, size_t cb)
{
	size_t cbOff = 0;

	if (pOut == NULL || cb == 0u) {
		return;
	}
	while (cbOff < cb) {
		ssize_t n;

		n = getrandom(pOut + cbOff, cb - cbOff, 0u);
		if (n <= 0) {
			b106_weak_seed(pOut + cbOff, cb - cbOff);
			return;
		}
		cbOff += (size_t)n;
	}
}

static void
b106_rekey(struct b106_rng *pR)
{
	unsigned char aSeed[40];
	unsigned i;

	b106_entropy(aSeed, sizeof(aSeed));
	for (i = 0; i < 32u; i++) {
		pR->aKey[i] = aSeed[i];
	}
	for (i = 0; i < 8u; i++) {
		pR->aNonce[i] = aSeed[32u + i];
	}
	/* Keep upper nonce word non-zero-ish for domain separation. */
	pR->aNonce[8] = 0x6au;
	pR->aNonce[9] = 0x61u;
	pR->aNonce[10] = 0x64u;
	pR->aNonce[11] = 0x65u; /* "jade" tag bytes */
	pR->uCounter = 0;
	pR->cbHave = 0;
	pR->cbSinceRekey = 0;
	pR->fInit = 1u;
	b106_zero(aSeed, sizeof(aSeed));
	b106_zero(pR->aBuf, sizeof(pR->aBuf));
}

static void
b106_ensure(struct b106_rng *pR)
{
	if (pR->fInit == 0u) {
		b106_rekey(pR);
	}
	if (pR->cbSinceRekey >= (uint64_t)B106_REKEY_BYTES) {
		/* Fold current keystream into next key before reseed. */
		unsigned char aMix[32];
		unsigned i;

		b106_chacha20_block(pR->aKey, pR->aNonce, pR->uCounter + 1u,
		                    pR->aBuf);
		for (i = 0; i < 32u; i++) {
			aMix[i] = pR->aBuf[i] ^ pR->aKey[i];
		}
		for (i = 0; i < 32u; i++) {
			pR->aKey[i] = aMix[i];
		}
		b106_zero(aMix, sizeof(aMix));
		b106_rekey(pR);
	}
}

static void
b106_refill(struct b106_rng *pR)
{
	b106_chacha20_block(pR->aKey, pR->aNonce, pR->uCounter, pR->aBuf);
	pR->uCounter++;
	pR->cbHave = 64u;
}

void
__arc4random_buf(void *pBuf, size_t cb)
{
	unsigned char *p;
	size_t cbLeft;

	if (pBuf == NULL || cb == 0u) {
		return;
	}
	p = (unsigned char *)pBuf;
	cbLeft = cb;
	b106_ensure(&g_b106Rng);

	while (cbLeft > 0u) {
		unsigned cbTake;
		unsigned i;
		unsigned cbStart;

		if (g_b106Rng.cbHave == 0u) {
			b106_refill(&g_b106Rng);
		}
		cbTake = g_b106Rng.cbHave;
		if ((size_t)cbTake > cbLeft) {
			cbTake = (unsigned)cbLeft;
		}
		cbStart = 64u - g_b106Rng.cbHave;
		for (i = 0; i < cbTake; i++) {
			p[i] = g_b106Rng.aBuf[cbStart + i];
			g_b106Rng.aBuf[cbStart + i] = 0;
		}
		g_b106Rng.cbHave -= cbTake;
		g_b106Rng.cbSinceRekey += (uint64_t)cbTake;
		p += cbTake;
		cbLeft -= (size_t)cbTake;
		if (g_b106Rng.cbSinceRekey >= (uint64_t)B106_REKEY_BYTES) {
			b106_ensure(&g_b106Rng);
		}
	}
}

uint32_t
__arc4random(void)
{
	uint32_t u = 0;

	__arc4random_buf(&u, sizeof(u));
	return u;
}

uint32_t
__arc4random_uniform(uint32_t uUpper)
{
	uint32_t uMin;
	uint32_t u;

	if (uUpper < 2u) {
		return 0;
	}
	/* min = 2^32 % upper via (-upper) % upper */
	uMin = (uint32_t)(-uUpper) % uUpper;
	do {
		u = __arc4random();
	} while (u < uMin);
	return u % uUpper;
}

/*
 * Mix caller bytes into CSPRNG key (fills gap: public arc4random_addrandom
 * is a no-op stub; underscored form actually stirs this ChaCha state).
 */
void
__arc4random_addrandom(unsigned char *pDat, int nDat)
{
	unsigned char aBlock[64];
	unsigned i;
	int n;

	if (pDat == NULL || nDat <= 0) {
		return;
	}
	b106_ensure(&g_b106Rng);
	b106_chacha20_block(g_b106Rng.aKey, g_b106Rng.aNonce,
	                    g_b106Rng.uCounter + 7u, aBlock);
	n = nDat;
	if (n > 32) {
		n = 32;
	}
	for (i = 0; i < (unsigned)n; i++) {
		g_b106Rng.aKey[i] ^= pDat[i] ^ aBlock[i];
	}
	for (i = 0; i < 12u; i++) {
		g_b106Rng.aNonce[i] ^= aBlock[32u + i];
	}
	g_b106Rng.uCounter ^= b106_load_le32(aBlock + 44);
	g_b106Rng.cbHave = 0;
	g_b106Rng.cbSinceRekey = 0;
	b106_zero(aBlock, sizeof(aBlock));
}
