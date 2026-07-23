/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4403: saturating uint32_t multiply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_sat_u(uint32_t a, uint32_t b);
 *     - a * b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_mul_sat_u  (alias)
 *   __libcgj_batch4403_marker = "libcgj-batch4403"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410). Distinct from
 * gj_u64_mul_sat_soft (batch2033), gj_i32_mul_sat_u (batch3626), and
 * wrap mul surfaces — unique gj_u32_mul_sat_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4403_marker[] = "libcgj-batch4403";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating multiply via widening product: overflow when the high
 * 32 bits of the 64-bit product are non-zero.
 */
static uint32_t
b4403_mul_sat(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64P;

	u64P = (uint64_t)u32A * (uint64_t)u32B;
	if (u64P > (uint64_t)UINT32_MAX) {
		return UINT32_MAX;
	}
	return (uint32_t)u64P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_sat_u - unsigned 32-bit multiply; clamp to UINT32_MAX.
 *
 * a, b: factors
 * Zero factor never overflows. No parent wires. No __int128.
 */
uint32_t
gj_u32_mul_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4403_mul_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mul_sat_u")));
