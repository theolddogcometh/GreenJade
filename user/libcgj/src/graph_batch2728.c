/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2728: flip a single bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_flip_u64(uint64_t x, unsigned b);
 *     - Flip bit b of x (0 = LSB). If b >= 64, return x unchanged
 *       (no shift UB). Pure integer XOR; no builtins.
 *   uint64_t __gj_bit_flip_u64  (alias)
 *   __libcgj_batch2728_marker = "libcgj-batch2728"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_bit_toggle_u64
 * (batch654), gj_u64_toggle_bit (batch2013) — unique gj_bit_flip_u64
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2728_marker[] = "libcgj-batch2728";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2728_flip(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X ^ ((uint64_t)1ull << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_flip_u64 - flip bit b of a 64-bit word.
 *
 * x: value whose bit is to be toggled
 * b: bit index (0 = least significant bit, 63 = MSB)
 *
 * Returns x with bit b inverted via XOR. If b >= 64, returns x
 * unchanged so the shift never exceeds the word width.
 * Example: gj_bit_flip_u64(0, 0) -> 1; gj_bit_flip_u64(1, 0) -> 0.
 */
uint64_t
gj_bit_flip_u64(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2728_flip(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_flip_u64(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_flip_u64")));
