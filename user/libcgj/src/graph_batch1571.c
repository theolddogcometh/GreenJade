/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1571: 8x8 bit-matrix get (packed in uint64_t).
 *
 * Surface (unique symbols):
 *   int gj_bitmat8_get(uint64_t m, unsigned r, unsigned c);
 *     — Return bit (r,c) of an 8x8 bit matrix packed in u64. Layout is
 *       row-major LSB-first: bit index = r*8 + c (row 0 = bits 0..7).
 *       Out-of-range r/c → 0.
 *   int __gj_bitmat8_get  (alias)
 *   __libcgj_batch1571_marker = "libcgj-batch1571"
 *
 * Distinct from gj_bitset_test (batch118). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1571_marker[] = "libcgj-batch1571";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bit index for cell (r,c); caller guarantees r,c < 8. */
static unsigned
b1571_bit(unsigned uR, unsigned uC)
{
	return (uR << 3) + uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_get — read one bit from an 8x8 matrix packed in u64M.
 *
 * uR, uC in [0,7]; otherwise returns 0. Result is 0 or 1.
 */
int
gj_bitmat8_get(uint64_t u64M, unsigned uR, unsigned uC)
{
	unsigned uBit;

	if (uR >= 8u || uC >= 8u) {
		return 0;
	}
	uBit = b1571_bit(uR, uC);
	return (int)((u64M >> uBit) & 1ull);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmat8_get(uint64_t u64M, unsigned uR, unsigned uC)
    __attribute__((alias("gj_bitmat8_get")));
