/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3172: set one bit in a uint32_t bitset word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitset32_set_u(uint32_t x, unsigned b);
 *     - Return x with bit b set (b in 0..31, LSB = bit 0). If b >= 32,
 *       return x unchanged. Pure integer OR; no builtins.
 *   uint32_t __gj_bitset32_set_u  (alias)
 *   __libcgj_batch3172_marker = "libcgj-batch3172"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_set_u64 (batch651) and array bitset set (batch118) —
 * unique gj_bitset32_set_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3172_marker[] = "libcgj-batch3172";

/* ---- freestanding helpers ---------------------------------------------- */

/* x | (1u << b) for b in 0..31; otherwise x. */
static uint32_t
b3172_set(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X | ((uint32_t)1u << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset32_set_u - set bit b of a 32-bit bitset word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 31 is MSB. b >= 32 → no change.
 *
 * Returns x | (1u << b) for b in 0..31; otherwise x. No parent wires.
 */
uint32_t
gj_bitset32_set_u(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b3172_set(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bitset32_set_u(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bitset32_set_u")));
