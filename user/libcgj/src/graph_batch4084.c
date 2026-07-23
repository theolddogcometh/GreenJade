/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4084: ceil unsigned Q16.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_ceil_q16_u(uint32_t q);
 *     - Ceiling of unsigned Q16.16 q as a plain integer.
 *       floor(q) if fraction is zero, else floor(q)+1.
 *       Max result 0x10000 when q is in (0xFFFF0000, 0xFFFFFFFF].
 *   uint32_t __gj_u32_ceil_q16_u  (alias)
 *   __libcgj_batch4084_marker = "libcgj-batch4084"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u32_floor_q16_u (batch4082) / gj_u32_ceil_div_u (batch3851) —
 * unique gj_u32_ceil_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4084_marker[] = "libcgj-batch4084";

/* Low 16 bits = fractional field of unsigned Q16.16. */
#define B4084_FRAC_MASK  0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(q) = floor(q) + (frac != 0). Result always fits in uint32_t
 * (max floor is 0xFFFF; +1 yields 0x10000).
 */
static uint32_t
b4084_ceil(uint32_t u32Q)
{
	uint32_t u32I;

	u32I = u32Q >> 16;
	if ((u32Q & B4084_FRAC_MASK) != 0u) {
		u32I += 1u;
	}
	return u32I;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_ceil_q16_u - ceiling of unsigned Q16.16 q to integer.
 *
 * q: unsigned Q16.16 (bits 31..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns ceil(q) in [0, 0x10000]. No parent wires.
 */
uint32_t
gj_u32_ceil_q16_u(uint32_t u32Q)
{
	(void)NULL;
	return b4084_ceil(u32Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_ceil_q16_u(uint32_t u32Q)
    __attribute__((alias("gj_u32_ceil_q16_u")));
