/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3173: clear one bit in a uint32_t bitset word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitset32_clr_u(uint32_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..31 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint32_t __gj_bitset32_clr_u  (alias)
 *   __libcgj_batch3173_marker = "libcgj-batch3173"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_clear_u64 (batch652) and array bitset clear (batch118) —
 * unique gj_bitset32_clr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3173_marker[] = "libcgj-batch3173";

/* ---- freestanding helpers ---------------------------------------------- */

/* x & ~(1u << b) for b in 0..31; otherwise x. */
static uint32_t
b3173_clr(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X & ~((uint32_t)1u << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset32_clr_u - clear bit b of a 32-bit bitset word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 32, return x unchanged.
 *
 * Example: gj_bitset32_clr_u(0xFFu, 0) → 0xFEu;
 *          gj_bitset32_clr_u(0x80000000u, 31) → 0.
 * No parent wires.
 */
uint32_t
gj_bitset32_clr_u(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b3173_clr(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bitset32_clr_u(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bitset32_clr_u")));
