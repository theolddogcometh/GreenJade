/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1217: Fletcher-16 one-shot over n bytes.
 *
 * Surface (unique symbols):
 *   uint16_t gj_fletcher16_n(const void *data, size_t n);
 *     — Fletcher-16 over n bytes (8-bit blocks, modulus 255).
 *         sum1 = (sum1 + byte) mod 255
 *         sum2 = (sum2 + sum1) mod 255
 *       Result = (sum2 << 8) | sum1. Empty/NULL → 0.
 *   uint16_t __gj_fletcher16_n  (alias)
 *   __libcgj_batch1217_marker = "libcgj-batch1217"
 *
 * Distinct from gj_fletcher16 (batch183) — unique _n name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1217_marker[] = "libcgj-batch1217";

#define B1217_F16_BLOCK 256u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher16_n — one-shot Fletcher-16 over n bytes.
 */
uint16_t
gj_fletcher16_n(const void *pData, size_t cbN)
{
	const unsigned char *pBytes;
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	size_t cbLeft;
	size_t cbChunk;
	size_t i;

	if (pData == NULL || cbN == 0u) {
		return 0u;
	}

	pBytes = (const unsigned char *)pData;
	u32Sum1 = 0u;
	u32Sum2 = 0u;
	cbLeft = cbN;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B1217_F16_BLOCK) {
			cbChunk = (size_t)B1217_F16_BLOCK;
		}
		for (i = 0u; i < cbChunk; i++) {
			u32Sum1 += (uint32_t)pBytes[i];
			u32Sum2 += u32Sum1;
		}
		pBytes += cbChunk;
		cbLeft -= cbChunk;
		u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
		u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	}

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

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_fletcher16_n(const void *pData, size_t cbN)
    __attribute__((alias("gj_fletcher16_n")));
