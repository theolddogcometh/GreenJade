/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3303: floor base-2 logarithm for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_flog2_u(uint32_t x);
 *     - Floor of log2(x) for x >= 1 (MSB position, 0-based):
 *       gj_u32_flog2_u(1)==0, gj_u32_flog2_u(2)==1,
 *       gj_u32_flog2_u(3)==1, gj_u32_flog2_u(0x80000000u)==31.
 *       Returns 0 when x == 0 (defined sentinel; log2(0) undefined).
 *   uint32_t __gj_u32_flog2_u  (alias)
 *   __libcgj_batch3303_marker = "libcgj-batch3303"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_ilog2_u64 (batch372) — unique u32 floor log2; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3303_marker[] = "libcgj-batch3303";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor log2 for x >= 1; caller must not pass 0. */
static uint32_t
b3303_flog2(uint32_t uX)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_flog2_u - floor base-2 logarithm of x.
 *
 * x: unsigned 32-bit value
 *
 * Returns floor(log2(x)) for x >= 1; 0 when x == 0 (defined sentinel).
 * No parent wires.
 */
uint32_t
gj_u32_flog2_u(uint32_t x)
{
	(void)NULL;
	if (x == 0u) {
		return 0u;
	}
	return b3303_flog2(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_flog2_u(uint32_t x)
    __attribute__((alias("gj_u32_flog2_u")));
