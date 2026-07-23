/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1575: extract one column from an 8x8 bit matrix.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bitmat8_col(uint64_t m, unsigned c);
 *     — Return the 8 bits of column c as a uint8_t (row 0 = LSB). Layout
 *       matches gj_bitmat8_get: bit (r,c) at index r*8 + c.
 *       Out-of-range c → 0.
 *   uint8_t __gj_bitmat8_col  (alias)
 *   __libcgj_batch1575_marker = "libcgj-batch1575"
 *
 * Distinct from gj_bitmat8_row (batch1574). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1575_marker[] = "libcgj-batch1575";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Gather bit (iR, uC) into result bit iR (row 0 = LSB of returned byte).
 */
static uint8_t
b1575_gather_col(uint64_t u64M, unsigned uC)
{
	unsigned iR;
	uint8_t u8Out;

	u8Out = 0u;
	for (iR = 0u; iR < 8u; iR++) {
		if (((u64M >> ((iR << 3) + uC)) & 1ull) != 0ull) {
			u8Out = (uint8_t)(u8Out | (uint8_t)(1u << iR));
		}
	}
	return u8Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_col — extract column uC of an 8x8 matrix packed in u64M.
 *
 * uC in [0,7]; otherwise returns 0. Row 0 is the LSB of the result byte.
 */
uint8_t
gj_bitmat8_col(uint64_t u64M, unsigned uC)
{
	if (uC >= 8u) {
		return 0u;
	}
	return b1575_gather_col(u64M, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bitmat8_col(uint64_t u64M, unsigned uC)
    __attribute__((alias("gj_bitmat8_col")));
