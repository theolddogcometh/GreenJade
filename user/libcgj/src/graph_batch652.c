/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch652: clear one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_clear_u64(uint64_t x, unsigned b);
 *     — Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..63 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint64_t __gj_bit_clear_u64  (alias)
 *   __libcgj_batch652_marker = "libcgj-batch652"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch652_marker[] = "libcgj-batch652";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_clear_u64 — clear bit b of a 64-bit word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 64, return x unchanged.
 *
 * Example: gj_bit_clear_u64(0xFFull, 0) → 0xFEull;
 *          gj_bit_clear_u64(0x8000000000000000ull, 63) → 0.
 */
uint64_t
gj_bit_clear_u64(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X & ~((uint64_t)1u << uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_clear_u64(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_clear_u64")));
