/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1423: gj_xor16_fold — XOR bytes, fold to 16 bits.
 *
 * Surface (unique symbols — do NOT collide with gj_xor_buf / keystream):
 *   uint16_t gj_xor16_fold(const void *data, size_t n);
 *     — XOR every input byte into a 32-bit lane (byte i into lane
 *       (i mod 4) at bit offset 8*(i mod 4)), then fold 32→16 by
 *       XORing the high and low halves. data == NULL or n == 0 → 0.
 *   uint16_t __gj_xor16_fold  (alias)
 *   __libcgj_batch1423_marker = "libcgj-batch1423"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1423_marker[] = "libcgj-batch1423";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold a 32-bit XOR accumulator down to 16 bits. */
static uint16_t
b1423_fold32_to16(uint32_t u32X)
{
	u32X ^= (u32X >> 16);
	return (uint16_t)(u32X & 0xffffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor16_fold — XOR all bytes into lanes, fold result to uint16_t.
 */
uint16_t
gj_xor16_fold(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Acc;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u32Acc = 0u;
	for (i = 0u; i < cb; i++) {
		unsigned shift = (unsigned)((i & 3u) * 8u);

		u32Acc ^= (uint32_t)p[i] << shift;
	}
	return b1423_fold32_to16(u32Acc);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_xor16_fold(const void *pData, size_t cb)
    __attribute__((alias("gj_xor16_fold")));
