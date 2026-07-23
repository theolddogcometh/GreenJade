/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2017: set one bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_set_bit(uint32_t x, unsigned b);
 *     - Return x with bit b set (b in 0..31, LSB = bit 0). If b >= 32,
 *       return x unchanged (no shift UB). Pure integer OR; no builtins.
 *   uint32_t __gj_u32_set_bit  (alias)
 *   __libcgj_batch2017_marker = "libcgj-batch2017"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_u64_set_bit (batch2011) and gj_bit_set_u64 (batch651).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2017_marker[] = "libcgj-batch2017";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2017_set(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return u32X;
	}
	return u32X | ((uint32_t)1 << uB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_set_bit - set bit b of a 32-bit word.
 *
 * x: value to update
 * b: bit index; 0 is LSB, 31 is MSB. b >= 32 -> no change.
 *
 * Returns x | (1u << b) for b in 0..31; otherwise x.
 */
uint32_t
gj_u32_set_bit(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2017_set(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_set_bit(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_u32_set_bit")));
