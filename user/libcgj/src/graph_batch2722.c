/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2722: set a single bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_set_u32(uint32_t x, unsigned b);
 *     - Return x with bit b set (b in 0..31, LSB = bit 0). If b >= 32,
 *       return x unchanged (no shift UB). Pure integer OR; no builtins.
 *   uint32_t __gj_bit_set_u32  (alias)
 *   __libcgj_batch2722_marker = "libcgj-batch2722"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_u32_set_bit
 * (batch2017), gj_bit_set_u64 (batch651) — unique gj_bit_set_u32
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2722_marker[] = "libcgj-batch2722";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2722_set(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X | ((uint32_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_set_u32 - set bit b of a 32-bit word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 31 is MSB. b >= 32 -> no change.
 *
 * Returns x | (1u << b) for b in 0..31; otherwise x.
 * Examples: (0, 0) -> 1; (0, 31) -> 0x80000000; (7, 1) -> 7;
 *           (0, 32) -> 0; (0xff, 100) -> 0xff.
 */
uint32_t
gj_bit_set_u32(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2722_set(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_set_u32(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bit_set_u32")));
