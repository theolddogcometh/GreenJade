/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2018: clear one bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clear_bit(uint32_t x, unsigned b);
 *     - Return x with bit b cleared (LSB is bit 0). Bits outside
 *       0..31 leave x unchanged (no shift UB). Pure integer; no
 *       builtins.
 *   uint32_t __gj_u32_clear_bit  (alias)
 *   __libcgj_batch2018_marker = "libcgj-batch2018"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_u64_clear_bit (batch2012) and gj_bit_clear_u64 (batch652).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2018_marker[] = "libcgj-batch2018";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2018_clear(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X & ~((uint32_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clear_bit - clear bit b of a 32-bit word.
 *
 * x: value
 * b: bit index (0 = LSB). If b >= 32, return x unchanged.
 *
 * Example: gj_u32_clear_bit(0xFFu, 0) -> 0xFEu;
 *          gj_u32_clear_bit(0x80000000u, 31) -> 0.
 */
uint32_t
gj_u32_clear_bit(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2018_clear(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clear_bit(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_u32_clear_bit")));
