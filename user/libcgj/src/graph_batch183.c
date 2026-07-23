/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch183: Fletcher-16 and Fletcher-32 checksums.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party checksum source was copied.
 *
 * Surface (unique symbols):
 *   uint16_t gj_fletcher16(const void *data, size_t len);
 *     — Fletcher-16 over len bytes (8-bit blocks, modulus 255).
 *         sum1 = (sum1 + byte) mod 255
 *         sum2 = (sum2 + sum1) mod 255
 *       Result = (sum2 << 8) | sum1. Empty/NULL → 0.
 *   uint32_t gj_fletcher32(const void *data, size_t len);
 *     — Fletcher-32 over len bytes as little-endian 16-bit words
 *       (modulus 65535). Odd trailing byte is zero-padded as a high byte
 *       of zero (i.e. the last word is just the low byte).
 *         sum1 = (sum1 + word) mod 65535
 *         sum2 = (sum2 + sum1) mod 65535
 *       Result = (sum2 << 16) | sum1. Empty/NULL → 0.
 *   __gj_fletcher16 / __gj_fletcher32  (aliases)
 *   __libcgj_batch183_marker = "libcgj-batch183"
 *
 * No malloc, no errno (freestanding-safe). Lazy modular reduction is used
 * so intermediate sums fit in uint32_t / uint64_t without per-byte % cost.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch183_marker[] = "libcgj-batch183";

/*
 * Max bytes between reductions for Fletcher-16:
 * sum1 grows by at most 255 per byte; sum2 by at most sum1.
 * With uint32_t accumulators, 5600+ bytes is safe; 256 is ample and simple.
 */
#define B183_F16_BLOCK 256u

/*
 * Max 16-bit words between reductions for Fletcher-32:
 * sum1 grows by at most 0xffff per word. uint64_t holds many; 360 is classic
 * (Wikipedia / Adler-style bound for 32-bit sums). We use uint64_t so larger
 * blocks are fine; keep 360 for well-known correctness margins.
 */
#define B183_F32_BLOCK 360u

/* ---- gj_fletcher16 ----------------------------------------------------- */

/*
 * One-shot Fletcher-16. Processes each byte; returns (sum2<<8)|sum1 with
 * both sums reduced mod 255. sum1/sum2 never take the value 255 (mod 255
 * range is 0..254), matching the standard definition.
 */
uint16_t
gj_fletcher16(const void *pData, size_t cbLen)
{
	const unsigned char *pBytes;
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	size_t cbLeft;
	size_t cbChunk;
	size_t i;

	if (pData == NULL || cbLen == 0u) {
		return 0u;
	}

	pBytes = (const unsigned char *)pData;
	u32Sum1 = 0u;
	u32Sum2 = 0u;
	cbLeft = cbLen;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B183_F16_BLOCK) {
			cbChunk = (size_t)B183_F16_BLOCK;
		}
		for (i = 0u; i < cbChunk; i++) {
			u32Sum1 += (uint32_t)pBytes[i];
			u32Sum2 += u32Sum1;
		}
		pBytes += cbChunk;
		cbLeft -= cbChunk;
		/* Reduce: x % 255 == (x & 0xff) + (x >> 8), then once more. */
		u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
		u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	}

	/* Final fold into 0..254. */
	u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
	u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	if (u32Sum1 >= 255u) {
		u32Sum1 -= 255u;
	}
	if (u32Sum2 >= 255u) {
		u32Sum2 -= 255u;
	}

	return (uint16_t)((u32Sum2 << 8) | u32Sum1);
}

uint16_t __gj_fletcher16(const void *pData, size_t cbLen)
    __attribute__((alias("gj_fletcher16")));

/* ---- gj_fletcher32 ----------------------------------------------------- */

/*
 * One-shot Fletcher-32 over little-endian 16-bit words assembled from the
 * byte stream. An odd final byte becomes a word with the high byte zero.
 */
uint32_t
gj_fletcher32(const void *pData, size_t cbLen)
{
	const unsigned char *pBytes;
	uint64_t u64Sum1;
	uint64_t u64Sum2;
	size_t cWords;
	size_t cChunk;
	size_t i;
	uint32_t u32Word;

	if (pData == NULL || cbLen == 0u) {
		return 0u;
	}

	pBytes = (const unsigned char *)pData;
	u64Sum1 = 0u;
	u64Sum2 = 0u;
	cWords = cbLen / 2u;

	while (cWords > 0u) {
		cChunk = cWords;
		if (cChunk > (size_t)B183_F32_BLOCK) {
			cChunk = (size_t)B183_F32_BLOCK;
		}
		for (i = 0u; i < cChunk; i++) {
			u32Word = (uint32_t)pBytes[0] |
			    ((uint32_t)pBytes[1] << 8);
			pBytes += 2u;
			u64Sum1 += (uint64_t)u32Word;
			u64Sum2 += u64Sum1;
		}
		cWords -= cChunk;
		/* Reduce mod 65535: x % 65535 == (x & 0xffff) + (x >> 16). */
		u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
		u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	}

	/* Trailing odd byte (zero-padded high half). */
	if ((cbLen & 1u) != 0u) {
		u64Sum1 += (uint64_t)pBytes[0];
		u64Sum2 += u64Sum1;
	}

	/* Final fold into 0..65534. */
	u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
	u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	u64Sum1 = (u64Sum1 & 0xffffu) + (u64Sum1 >> 16);
	u64Sum2 = (u64Sum2 & 0xffffu) + (u64Sum2 >> 16);
	if (u64Sum1 >= 65535u) {
		u64Sum1 -= 65535u;
	}
	if (u64Sum2 >= 65535u) {
		u64Sum2 -= 65535u;
	}

	return (uint32_t)((u64Sum2 << 16) | u64Sum1);
}

uint32_t __gj_fletcher32(const void *pData, size_t cbLen)
    __attribute__((alias("gj_fletcher32")));
