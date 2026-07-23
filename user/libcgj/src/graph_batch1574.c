/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1574: extract one row from an 8x8 bit matrix.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bitmat8_row(uint64_t m, unsigned r);
 *     — Return the 8 bits of row r as a uint8_t (col 0 = LSB). Layout
 *       matches gj_bitmat8_get: row r occupies bits [r*8, r*8+7].
 *       Out-of-range r → 0.
 *   uint8_t __gj_bitmat8_row  (alias)
 *   __libcgj_batch1574_marker = "libcgj-batch1574"
 *
 * Distinct from gj_bitmat8_get (batch1571). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1574_marker[] = "libcgj-batch1574";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte mask for one matrix row. */
static uint64_t
b1574_row_mask(void)
{
	return 0xFFull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_row — extract row uR of an 8x8 matrix packed in u64M.
 *
 * uR in [0,7]; otherwise returns 0. Col 0 is the LSB of the result byte.
 */
uint8_t
gj_bitmat8_row(uint64_t u64M, unsigned uR)
{
	unsigned uShift;

	if (uR >= 8u) {
		return 0u;
	}
	uShift = uR << 3;
	return (uint8_t)((u64M >> uShift) & b1574_row_mask());
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bitmat8_row(uint64_t u64M, unsigned uR)
    __attribute__((alias("gj_bitmat8_row")));
