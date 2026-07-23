/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch651: set a single bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_set_u64(uint64_t x, unsigned b);
 *     — Return x with bit b set (b in 0..63, LSB = bit 0). If b >= 64,
 *       return x unchanged. Pure integer OR; no builtins.
 *   uint64_t __gj_bit_set_u64  (alias)
 *   __libcgj_batch651_marker = "libcgj-batch651"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch651_marker[] = "libcgj-batch651";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_set_u64 — set bit b of a 64-bit word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 63 is MSB. b >= 64 → no change.
 *
 * Returns x | (1ull << b) for b in 0..63; otherwise x.
 * Examples: (0, 0) → 1; (0, 63) → 0x8000000000000000; (7, 1) → 7;
 *           (0, 64) → 0; (0xff, 100) → 0xff.
 */
uint64_t
gj_bit_set_u64(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X | ((uint64_t)1 << uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_set_u64(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_set_u64")));
