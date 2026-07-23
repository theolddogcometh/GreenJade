/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2723: clear one bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_clr_u32(uint32_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..31 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint32_t __gj_bit_clr_u32  (alias)
 *   __libcgj_batch2723_marker = "libcgj-batch2723"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_u32_clear_bit
 * (batch2018), gj_bit_clear_u64 (batch652) — unique gj_bit_clr_u32
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2723_marker[] = "libcgj-batch2723";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2723_clr(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X & ~((uint32_t)1u << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_clr_u32 - clear bit b of a 32-bit word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 32, return x unchanged.
 *
 * Example: gj_bit_clr_u32(0xFFu, 0) -> 0xFEu;
 *          gj_bit_clr_u32(0x80000000u, 31) -> 0.
 */
uint32_t
gj_bit_clr_u32(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2723_clr(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_clr_u32(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bit_clr_u32")));
