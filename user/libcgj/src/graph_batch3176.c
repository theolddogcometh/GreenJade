/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3176: clear one bit in a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitset64_clr_u(uint64_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..63 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint64_t __gj_bitset64_clr_u  (alias)
 *   __libcgj_batch3176_marker = "libcgj-batch3176"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_clear_u64 (batch652) and gj_bitset32_clr_u (batch3173) —
 * unique gj_bitset64_clr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3176_marker[] = "libcgj-batch3176";

/* ---- freestanding helpers ---------------------------------------------- */

/* x & ~(1ull << b) for b in 0..63; otherwise x. */
static uint64_t
b3176_clr(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return u64X;
	}
	return u64X & ~((uint64_t)1ull << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_clr_u - clear bit b of a 64-bit bitset word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 64, return x unchanged.
 *
 * Example: gj_bitset64_clr_u(0xFFull, 0) → 0xFEull;
 *          gj_bitset64_clr_u(0x8000000000000000ull, 63) → 0.
 * No parent wires.
 */
uint64_t
gj_bitset64_clr_u(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b3176_clr(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitset64_clr_u(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bitset64_clr_u")));
