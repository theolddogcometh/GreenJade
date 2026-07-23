/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4187: one-shot Fletcher-16 checksum.
 *
 * Surface (unique symbols):
 *   uint16_t gj_fletcher16_u(const uint8_t *p, size_t n);
 *     - Fletcher-16 over n bytes (modulus 255).
 *         sum1 = (sum1 + byte) mod 255
 *         sum2 = (sum2 + sum1) mod 255
 *       Result = (sum2 << 8) | sum1. NULL or n == 0 → 0.
 *   uint16_t __gj_fletcher16_u  (alias)
 *   __libcgj_batch4187_marker = "libcgj-batch4187"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_fletcher16_u surface only; no multi-def.
 * Distinct from gj_fletcher16 / gj_fletcher16_update_u (streaming).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4187_marker[] = "libcgj-batch4187";

/* Bytes between partial reductions (sums stay in uint32_t). */
#define B4187_BLOCK 256u

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold x toward 0..254 via x % 255 == (x & 0xff) + (x >> 8). */
static uint32_t
b4187_mod255(uint32_t u32X)
{
	u32X = (u32X & 0xffu) + (u32X >> 8);
	u32X = (u32X & 0xffu) + (u32X >> 8);
	if (u32X >= 255u) {
		u32X -= 255u;
	}
	return u32X;
}

static uint16_t
b4187_fletcher16(const uint8_t *pData, size_t cbN)
{
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	size_t cbLeft;
	size_t cbChunk;
	size_t iByte;

	u32Sum1 = 0u;
	u32Sum2 = 0u;
	cbLeft = cbN;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B4187_BLOCK) {
			cbChunk = (size_t)B4187_BLOCK;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			u32Sum1 += (uint32_t)pData[iByte];
			u32Sum2 += u32Sum1;
		}
		pData += cbChunk;
		cbLeft -= cbChunk;
		u32Sum1 = (u32Sum1 & 0xffu) + (u32Sum1 >> 8);
		u32Sum2 = (u32Sum2 & 0xffu) + (u32Sum2 >> 8);
	}

	u32Sum1 = b4187_mod255(u32Sum1);
	u32Sum2 = b4187_mod255(u32Sum2);
	return (uint16_t)((u32Sum2 << 8) | u32Sum1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher16_u - one-shot Fletcher-16 over n bytes.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns (sum2 << 8) | sum1 with sums in 0..254. NULL or empty → 0.
 * Self-contained; no parent wires.
 */
uint16_t
gj_fletcher16_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4187_fletcher16(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_fletcher16_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_fletcher16_u")));
