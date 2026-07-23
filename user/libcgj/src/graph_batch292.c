/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch292: BLAKE2s-256 one-shot under a unique gj_ name.
 *
 * Surface (unique symbols only — do NOT redefine BLAKE2s_* / BLAKE2b_*):
 *   void gj_blake2s_256(const void *data, size_t n, uint8_t out[32]);
 *     — RFC 7693 BLAKE2s-256 (unkeyed) one-shot over data[0..n).
 *       Writes 32-byte little-endian digest to out.
 *       data may be NULL only when n == 0; NULL data with n > 0 is a no-op
 *       (out left unchanged). out == NULL is a no-op.
 *   __gj_blake2s_256  (alias)
 *   __libcgj_batch292_marker = "libcgj-batch292"
 *
 * Avoid multi-def with graph_batch34.c BLAKE2s_Init/Update/Final and
 * graph_batch36.c BLAKE2s_End/Data: only gj_blake2s_256 is exported here.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * string.h. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch292_marker[] = "libcgj-batch292";

/* BLAKE2s block size (bytes). */
#define B292_BLOCK 64u
/* BLAKE2s-256 digest length (bytes). */
#define B292_OUT 32u
/* BLAKE2s round count. */
#define B292_ROUNDS 10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b292_ror32(uint32_t u32X, unsigned uN)
{
	return (u32X >> uN) | (u32X << (32u - uN));
}

static uint32_t
b292_load32le(const uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void
b292_store32le(uint8_t *p, uint32_t u32V)
{
	p[0] = (uint8_t)u32V;
	p[1] = (uint8_t)(u32V >> 8);
	p[2] = (uint8_t)(u32V >> 16);
	p[3] = (uint8_t)(u32V >> 24);
}

static void
b292_memzero(uint8_t *p, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = 0u;
	}
}

static void
b292_memcpy(uint8_t *pDst, const uint8_t *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- BLAKE2s core (RFC 7693, unkeyed, nn=32) --------------------------- */

/* IV = first 8 words of SHA-256 IV (public algorithm constants). */
static const uint32_t s_b292_iv[8] = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
    0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

/*
 * Message word permutation sigma (10 rounds × 16 indices).
 * RFC 7693 Table 2; rounds r reuse sigma[r % 10].
 */
static const uint8_t s_b292_sigma[10][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}};

/* G mixing step (RFC 7693 §3.1). */
static void
b292_g(uint32_t *pV, unsigned uA, unsigned uB, unsigned uC, unsigned uD,
       uint32_t u32X, uint32_t u32Y)
{
	pV[uA] = pV[uA] + pV[uB] + u32X;
	pV[uD] = b292_ror32(pV[uD] ^ pV[uA], 16u);
	pV[uC] = pV[uC] + pV[uD];
	pV[uB] = b292_ror32(pV[uB] ^ pV[uC], 12u);
	pV[uA] = pV[uA] + pV[uB] + u32Y;
	pV[uD] = b292_ror32(pV[uD] ^ pV[uA], 8u);
	pV[uC] = pV[uC] + pV[uD];
	pV[uB] = b292_ror32(pV[uB] ^ pV[uC], 7u);
}

/*
 * Compress one 64-byte block into chaining value aH[8].
 * u32T0/u32T1: 64-bit byte counter (low/high) of bytes hashed so far
 *   including this block.
 * fLast: non-zero → finalization flag (invert v[14]).
 */
static void
b292_compress(uint32_t *aH, const uint8_t *pBlock, uint32_t u32T0,
              uint32_t u32T1, int fLast)
{
	uint32_t aV[16];
	uint32_t aM[16];
	unsigned i;
	unsigned uR;

	for (i = 0u; i < 8u; i++) {
		aV[i] = aH[i];
		aV[i + 8u] = s_b292_iv[i];
	}
	aV[12] ^= u32T0;
	aV[13] ^= u32T1;
	if (fLast != 0) {
		aV[14] = ~aV[14];
	}

	for (i = 0u; i < 16u; i++) {
		aM[i] = b292_load32le(pBlock + i * 4u);
	}

	for (uR = 0u; uR < B292_ROUNDS; uR++) {
		const uint8_t *pS = s_b292_sigma[uR];

		/* Column step */
		b292_g(aV, 0, 4, 8, 12, aM[pS[0]], aM[pS[1]]);
		b292_g(aV, 1, 5, 9, 13, aM[pS[2]], aM[pS[3]]);
		b292_g(aV, 2, 6, 10, 14, aM[pS[4]], aM[pS[5]]);
		b292_g(aV, 3, 7, 11, 15, aM[pS[6]], aM[pS[7]]);
		/* Diagonal step */
		b292_g(aV, 0, 5, 10, 15, aM[pS[8]], aM[pS[9]]);
		b292_g(aV, 1, 6, 11, 12, aM[pS[10]], aM[pS[11]]);
		b292_g(aV, 2, 7, 8, 13, aM[pS[12]], aM[pS[13]]);
		b292_g(aV, 3, 4, 9, 14, aM[pS[14]], aM[pS[15]]);
	}

	for (i = 0u; i < 8u; i++) {
		aH[i] ^= aV[i] ^ aV[i + 8u];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_blake2s_256 — RFC 7693 BLAKE2s-256 unkeyed one-shot.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * out:  32-byte digest destination (must be non-NULL to write).
 *
 * Parameter block (XOR into h[0]): digest_length=32, key_length=0,
 * fanout=1, depth=1 → 0x01010000 ^ 32. No key, no salt, no personal.
 *
 * Intermediate full blocks keep one final block (0..64 bytes) for the
 * last-flag compress, matching the RFC reference streaming rule when
 * n is a multiple of 64.
 */
void
gj_blake2s_256(const void *data, size_t n, uint8_t out[32])
{
	const uint8_t *p;
	uint32_t aH[8];
	uint8_t aBlock[B292_BLOCK];
	uint32_t u32T0;
	uint32_t u32T1;
	size_t cbOff;
	size_t cbRem;
	size_t i;

	if (out == NULL) {
		return;
	}
	if (data == NULL && n != 0u) {
		return;
	}

	p = (const uint8_t *)data;
	u32T0 = 0u;
	u32T1 = 0u;

	/* Init chaining value: IV XOR param block. */
	for (i = 0u; i < 8u; i++) {
		aH[i] = s_b292_iv[i];
	}
	/* Digest length 32, key length 0, fanout 1, depth 1. */
	aH[0] ^= 0x01010000u ^ (uint32_t)B292_OUT;

	/*
	 * Intermediate blocks: leave the final block (even if full 64)
	 * for the last-flag compress. Empty input → single zero block.
	 */
	cbOff = 0u;
	while (n - cbOff > (size_t)B292_BLOCK) {
		u32T0 += (uint32_t)B292_BLOCK;
		if (u32T0 < (uint32_t)B292_BLOCK) {
			u32T1++;
		}
		b292_compress(aH, p + cbOff, u32T0, u32T1, 0);
		cbOff += (size_t)B292_BLOCK;
	}

	/* Final block: remaining 0..64 bytes, zero-padded. */
	cbRem = n - cbOff;
	b292_memzero(aBlock, (size_t)B292_BLOCK);
	if (cbRem != 0u && p != NULL) {
		b292_memcpy(aBlock, p + cbOff, cbRem);
	}
	u32T0 += (uint32_t)cbRem;
	if (u32T0 < (uint32_t)cbRem) {
		u32T1++;
	}
	b292_compress(aH, aBlock, u32T0, u32T1, 1);

	/* Little-endian digest. */
	for (i = 0u; i < 8u; i++) {
		b292_store32le(out + i * 4u, aH[i]);
	}

	/* Wipe stack state (best-effort; no secure zero guarantee). */
	b292_memzero(aBlock, (size_t)B292_BLOCK);
	for (i = 0u; i < 8u; i++) {
		aH[i] = 0u;
	}
}

void __gj_blake2s_256(const void *data, size_t n, uint8_t out[32])
    __attribute__((alias("gj_blake2s_256")));
