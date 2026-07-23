/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1424: classic Fletcher-16 (simple one-shot).
 *
 * Surface (unique symbols — gj_fletcher16 already exists in batch183):
 *   uint16_t gj_fletcher16_simple(const void *data, size_t n);
 *     — Classic Fletcher-16 over n bytes (8-bit blocks, modulus 255):
 *         sum1 = (sum1 + byte) mod 255
 *         sum2 = (sum2 + sum1) mod 255
 *       Result = (sum2 << 8) | sum1. Empty/NULL → 0.
 *   uint16_t __gj_fletcher16_simple  (alias)
 *   __libcgj_batch1424_marker = "libcgj-batch1424"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_fletcher16 / __gj_fletcher16  → graph_batch183.c
 *   gj_fletcher32 / __gj_fletcher32  → graph_batch183.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1424_marker[] = "libcgj-batch1424";

/*
 * Max bytes between reductions. sum1 grows by at most 255 per byte;
 * with uint32_t accumulators, 256 is ample and simple.
 */
#define B1424_F16_BLOCK 256u

/* ---- freestanding helpers ---------------------------------------------- */

/* Reduce x mod 255 via fold; final range 0..254. */
static uint32_t
b1424_mod255(uint32_t u32X)
{
	u32X = (u32X & 0xffu) + (u32X >> 8);
	u32X = (u32X & 0xffu) + (u32X >> 8);
	if (u32X >= 255u) {
		u32X -= 255u;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher16_simple — one-shot classic Fletcher-16 (mod 255).
 *
 * Independent of batch183 gj_fletcher16; same public algorithm, unique
 * symbol for the checksum-family graph surface.
 */
uint16_t
gj_fletcher16_simple(const void *pData, size_t cbLen)
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
		if (cbChunk > (size_t)B1424_F16_BLOCK) {
			cbChunk = (size_t)B1424_F16_BLOCK;
		}
		for (i = 0u; i < cbChunk; i++) {
			u32Sum1 += (uint32_t)pBytes[i];
			u32Sum2 += u32Sum1;
		}
		pBytes += cbChunk;
		cbLeft -= cbChunk;
		/* Lazy reduce: x % 255 == (x & 0xff) + (x >> 8). */
		u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
		u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	}

	u32Sum1 = b1424_mod255(u32Sum1);
	u32Sum2 = b1424_mod255(u32Sum2);

	return (uint16_t)((u32Sum2 << 8) | u32Sum1);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_fletcher16_simple(const void *pData, size_t cbLen)
    __attribute__((alias("gj_fletcher16_simple")));
