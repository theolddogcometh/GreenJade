/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3301: ceiling base-2 logarithm for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clog2_u(uint32_t x);
 *     - Ceiling of log2(x) for x >= 1 (smallest k with 2^k >= x).
 *       gj_u32_clog2_u(0)==0, gj_u32_clog2_u(1)==0,
 *       gj_u32_clog2_u(2)==1, gj_u32_clog2_u(3)==2,
 *       gj_u32_clog2_u(0x80000000u)==31, gj_u32_clog2_u(0xffffffffu)==32.
 *   uint32_t __gj_u32_clog2_u  (alias)
 *   __libcgj_batch3301_marker = "libcgj-batch3301"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_ilog2_u64 (batch372) — unique u32 ceiling log2; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3301_marker[] = "libcgj-batch3301";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor log2 for x >= 1 (MSB position, 0-based). */
static uint32_t
b3301_flog2(uint32_t uX)
{
	uint32_t uR;

	uR = 0u;
	if (uX >= 0x10000u) {
		uX >>= 16;
		uR += 16u;
	}
	if (uX >= 0x100u) {
		uX >>= 8;
		uR += 8u;
	}
	if (uX >= 0x10u) {
		uX >>= 4;
		uR += 4u;
	}
	if (uX >= 0x4u) {
		uX >>= 2;
		uR += 2u;
	}
	if (uX >= 0x2u) {
		uR += 1u;
	}
	return uR;
}

/* Ceiling log2; 0 for x == 0. */
static uint32_t
b3301_clog2(uint32_t uX)
{
	uint32_t uF;

	if (uX <= 1u) {
		return 0u;
	}
	/* clog2(x) = flog2(x-1) + 1 for x > 1. */
	uF = b3301_flog2(uX - 1u);
	return uF + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clog2_u - ceiling base-2 logarithm of x.
 *
 * x: unsigned 32-bit value
 *
 * Returns ceil(log2(x)) for x >= 1; 0 when x == 0 (defined sentinel).
 * No parent wires.
 */
uint32_t
gj_u32_clog2_u(uint32_t x)
{
	(void)NULL;
	return b3301_clog2(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clog2_u(uint32_t x)
    __attribute__((alias("gj_u32_clog2_u")));
