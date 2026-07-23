/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2013: toggle one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_toggle_bit(uint64_t x, unsigned b);
 *     - Flip bit b of x (0 = LSB). If b >= 64, return x unchanged
 *       (no shift UB). Pure integer XOR; no builtins.
 *   uint64_t __gj_u64_toggle_bit  (alias)
 *   __libcgj_batch2013_marker = "libcgj-batch2013"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_bit_toggle_u64 (batch654) - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2013_marker[] = "libcgj-batch2013";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2013_toggle(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X ^ ((uint64_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_toggle_bit - flip bit b of a 64-bit word.
 *
 * x: value whose bit is to be toggled
 * b: bit index (0 = LSB, 63 = MSB)
 *
 * Returns x with bit b inverted via XOR. If b >= 64, returns x
 * unchanged so the shift never exceeds the word width.
 * Example: gj_u64_toggle_bit(0, 0) -> 1; gj_u64_toggle_bit(1, 0) -> 0.
 */
uint64_t
gj_u64_toggle_bit(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2013_toggle(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_toggle_bit(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_u64_toggle_bit")));
