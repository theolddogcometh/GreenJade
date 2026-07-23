/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3175: set one bit in a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitset64_set_u(uint64_t x, unsigned b);
 *     - Return x with bit b set (b in 0..63, LSB = bit 0). If b >= 64,
 *       return x unchanged. Pure integer OR; no builtins.
 *   uint64_t __gj_bitset64_set_u  (alias)
 *   __libcgj_batch3175_marker = "libcgj-batch3175"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_set_u64 (batch651) and gj_bitset32_set_u (batch3172) —
 * unique gj_bitset64_set_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3175_marker[] = "libcgj-batch3175";

/* ---- freestanding helpers ---------------------------------------------- */

/* x | (1ull << b) for b in 0..63; otherwise x. */
static uint64_t
b3175_set(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X | ((uint64_t)1ull << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_set_u - set bit b of a 64-bit bitset word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 63 is MSB. b >= 64 → no change.
 *
 * Returns x | (1ull << b) for b in 0..63; otherwise x. No parent wires.
 */
uint64_t
gj_bitset64_set_u(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b3175_set(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitset64_set_u(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bitset64_set_u")));
