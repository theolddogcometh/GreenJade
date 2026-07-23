/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch293: HMAC-SHA256 (RFC 2104 over FIPS 180-4).
 *
 * Surface (unique symbols):
 *   int gj_hmac_sha256(const void *key, size_t klen,
 *                      const void *msg, size_t mlen,
 *                      uint8_t out[32]);
 *     — HMAC-SHA256(key, msg) → out[0..31].
 *       Returns 0 on success, -1 on error (out NULL, or NULL key/msg
 *       with non-zero length). Empty key (klen==0) and empty message
 *       (mlen==0) are valid; NULL is allowed only when the matching
 *       length is 0.
 *   __gj_hmac_sha256  (alias)
 *   __libcgj_batch293_marker = "libcgj-batch293"
 *
 * Self-contained static mini SHA-256 + HMAC (no extern SHA256_* link
 * deps). Clean-room freestanding pure C (integer/pointer only). Compiles
 * with -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Does not redefine hmac_sha256 (batch38).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch293_marker[] = "libcgj-batch293";

/* SHA-256 block size and digest size. */
#define B293_BLK 64u
#define B293_DIG 32u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b293_memset(void *pDst, int nFill, size_t cb)
{
	uint8_t *p = (uint8_t *)pDst;
	uint8_t u8Fill = (uint8_t)nFill;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = u8Fill;
	}
}

static void
b293_memcpy(void *pDst, const void *pSrc, size_t cb)
{
	uint8_t *pD = (uint8_t *)pDst;
	const uint8_t *pS = (const uint8_t *)pSrc;
	size_t i;

	for (i = 0u; i < cb; i++) {
		pD[i] = pS[i];
	}
}

static uint32_t
b293_ror32(uint32_t u32X, unsigned uN)
{
	return (u32X >> uN) | (u32X << (32u - uN));
}

static uint32_t
b293_get_be32(const uint8_t *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void
b293_put_be32(uint8_t *p, uint32_t u32V)
{
	p[0] = (uint8_t)(u32V >> 24);
	p[1] = (uint8_t)(u32V >> 16);
	p[2] = (uint8_t)(u32V >> 8);
	p[3] = (uint8_t)u32V;
}

/* ---- mini SHA-256 (static) --------------------------------------------- */

struct b293_sha256 {
	uint32_t aState[8];
	uint64_t u64Bits; /* total bits processed */
	uint8_t aBuf[B293_BLK];
	size_t cbBuf; /* bytes in aBuf */
};

/* FIPS 180-4 round constants. */
static const uint32_t s_b293_k[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

static void
b293_sha256_init(struct b293_sha256 *pCtx)
{
	pCtx->aState[0] = 0x6a09e667u;
	pCtx->aState[1] = 0xbb67ae85u;
	pCtx->aState[2] = 0x3c6ef372u;
	pCtx->aState[3] = 0xa54ff53au;
	pCtx->aState[4] = 0x510e527fu;
	pCtx->aState[5] = 0x9b05688cu;
	pCtx->aState[6] = 0x1f83d9abu;
	pCtx->aState[7] = 0x5be0cd19u;
	pCtx->u64Bits = 0ull;
	pCtx->cbBuf = 0u;
}

static void
b293_sha256_block(struct b293_sha256 *pCtx, const uint8_t *pBlk)
{
	uint32_t aW[64];
	uint32_t u32A, u32B, u32C, u32D, u32E, u32F, u32G, u32H;
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		aW[i] = b293_get_be32(pBlk + i * 4u);
	}
	for (i = 16u; i < 64u; i++) {
		uint32_t u32S0 =
		    b293_ror32(aW[i - 15u], 7u) ^ b293_ror32(aW[i - 15u], 18u) ^
		    (aW[i - 15u] >> 3);
		uint32_t u32S1 =
		    b293_ror32(aW[i - 2u], 17u) ^ b293_ror32(aW[i - 2u], 19u) ^
		    (aW[i - 2u] >> 10);
		aW[i] = aW[i - 16u] + u32S0 + aW[i - 7u] + u32S1;
	}

	u32A = pCtx->aState[0];
	u32B = pCtx->aState[1];
	u32C = pCtx->aState[2];
	u32D = pCtx->aState[3];
	u32E = pCtx->aState[4];
	u32F = pCtx->aState[5];
	u32G = pCtx->aState[6];
	u32H = pCtx->aState[7];

	for (i = 0u; i < 64u; i++) {
		uint32_t u32S1 = b293_ror32(u32E, 6u) ^ b293_ror32(u32E, 11u) ^
		                 b293_ror32(u32E, 25u);
		uint32_t u32Ch = (u32E & u32F) ^ ((~u32E) & u32G);
		uint32_t u32T1 = u32H + u32S1 + u32Ch + s_b293_k[i] + aW[i];
		uint32_t u32S0 = b293_ror32(u32A, 2u) ^ b293_ror32(u32A, 13u) ^
		                 b293_ror32(u32A, 22u);
		uint32_t u32Maj = (u32A & u32B) ^ (u32A & u32C) ^ (u32B & u32C);
		uint32_t u32T2 = u32S0 + u32Maj;

		u32H = u32G;
		u32G = u32F;
		u32F = u32E;
		u32E = u32D + u32T1;
		u32D = u32C;
		u32C = u32B;
		u32B = u32A;
		u32A = u32T1 + u32T2;
	}

	pCtx->aState[0] += u32A;
	pCtx->aState[1] += u32B;
	pCtx->aState[2] += u32C;
	pCtx->aState[3] += u32D;
	pCtx->aState[4] += u32E;
	pCtx->aState[5] += u32F;
	pCtx->aState[6] += u32G;
	pCtx->aState[7] += u32H;
}

static void
b293_sha256_update(struct b293_sha256 *pCtx, const void *pData, size_t cb)
{
	const uint8_t *pQ = (const uint8_t *)pData;
	size_t cbPart;

	if (pData == NULL || cb == 0u) {
		return;
	}

	pCtx->u64Bits += (uint64_t)cb * 8ull;

	if (pCtx->cbBuf > 0u) {
		cbPart = B293_BLK - pCtx->cbBuf;
		if (cbPart > cb) {
			cbPart = cb;
		}
		b293_memcpy(pCtx->aBuf + pCtx->cbBuf, pQ, cbPart);
		pCtx->cbBuf += cbPart;
		pQ += cbPart;
		cb -= cbPart;
		if (pCtx->cbBuf == B293_BLK) {
			b293_sha256_block(pCtx, pCtx->aBuf);
			pCtx->cbBuf = 0u;
		}
	}

	while (cb >= B293_BLK) {
		b293_sha256_block(pCtx, pQ);
		pQ += B293_BLK;
		cb -= B293_BLK;
	}

	if (cb > 0u) {
		b293_memcpy(pCtx->aBuf, pQ, cb);
		pCtx->cbBuf = cb;
	}
}

static void
b293_sha256_final(struct b293_sha256 *pCtx, uint8_t aDig[B293_DIG])
{
	uint8_t aLen[8];
	size_t i;
	uint64_t u64Bits = pCtx->u64Bits;
	uint8_t u8One;
	uint8_t u8Z;

	/* Big-endian bit length (saved before padding updates). */
	for (i = 0u; i < 8u; i++) {
		aLen[7u - i] = (uint8_t)(u64Bits >> (i * 8u));
	}

	/* Pad: 0x80, zeros, then 8-byte length. */
	u8One = 0x80u;
	b293_sha256_update(pCtx, &u8One, 1u);
	u8Z = 0u;
	while (pCtx->cbBuf != 56u) {
		b293_sha256_update(pCtx, &u8Z, 1u);
	}
	b293_sha256_update(pCtx, aLen, 8u);

	for (i = 0u; i < 8u; i++) {
		b293_put_be32(aDig + i * 4u, pCtx->aState[i]);
	}
	b293_memset(pCtx, 0, sizeof(*pCtx));
}

/* One-shot SHA-256 into aOut[32]. */
static void
b293_sha256(const void *pData, size_t cb, uint8_t aOut[B293_DIG])
{
	struct b293_sha256 ctx;

	b293_sha256_init(&ctx);
	b293_sha256_update(&ctx, pData, cb);
	b293_sha256_final(&ctx, aOut);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hmac_sha256 — RFC 2104 HMAC with SHA-256.
 *
 * K' = SHA256(key) if klen > 64, else key padded with zeros to 64.
 * HMAC = SHA256( (K' xor opad) || SHA256( (K' xor ipad) || msg ) )
 * with ipad = 0x36 * 64, opad = 0x5c * 64.
 */
int
gj_hmac_sha256(const void *key, size_t klen, const void *msg, size_t mlen,
               uint8_t out[32])
{
	uint8_t aK[B293_BLK];
	uint8_t aIpad[B293_BLK];
	uint8_t aOpad[B293_BLK];
	uint8_t aInner[B293_DIG];
	struct b293_sha256 ctx;
	size_t i;
	const uint8_t *pKey = (const uint8_t *)key;

	if (out == NULL) {
		return -1;
	}
	if (key == NULL && klen != 0u) {
		return -1;
	}
	if (msg == NULL && mlen != 0u) {
		return -1;
	}

	/* Normalize key into aK[0..63]. */
	b293_memset(aK, 0, B293_BLK);
	if (klen > B293_BLK) {
		b293_sha256(pKey, klen, aK);
		/* aK[32..63] already zero from memset. */
	} else if (klen > 0u) {
		b293_memcpy(aK, pKey, klen);
	}

	for (i = 0u; i < B293_BLK; i++) {
		aIpad[i] = (uint8_t)(aK[i] ^ 0x36u);
		aOpad[i] = (uint8_t)(aK[i] ^ 0x5cu);
	}

	/* Inner hash: H(ipad || msg). */
	b293_sha256_init(&ctx);
	b293_sha256_update(&ctx, aIpad, B293_BLK);
	if (mlen > 0u) {
		b293_sha256_update(&ctx, msg, mlen);
	}
	b293_sha256_final(&ctx, aInner);

	/* Outer hash: H(opad || inner). */
	b293_sha256_init(&ctx);
	b293_sha256_update(&ctx, aOpad, B293_BLK);
	b293_sha256_update(&ctx, aInner, B293_DIG);
	b293_sha256_final(&ctx, out);

	b293_memset(aK, 0, sizeof(aK));
	b293_memset(aIpad, 0, sizeof(aIpad));
	b293_memset(aOpad, 0, sizeof(aOpad));
	b293_memset(aInner, 0, sizeof(aInner));

	return 0;
}

int __gj_hmac_sha256(const void *key, size_t klen, const void *msg, size_t mlen,
                     uint8_t out[32])
    __attribute__((alias("gj_hmac_sha256")));
