/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3862: saturating uint32_t multiply-add (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_madd_sat_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Return sat(a * b + c): multiply-add with saturation at
 *       UINT32_MAX on overflow (no wrap). Implemented as
 *       sat_add(sat_mul(a,b), c).
 *   uint32_t __gj_u32_madd_sat_u  (alias)
 *   __libcgj_batch3862_marker = "libcgj-batch3862"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_u32_madd_sat_u surface only; no multi-def. Distinct from
 * wrap mul/add surfaces. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3862_marker[] = "libcgj-batch3862";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a * b in the uint32_t domain (via wide product). */
static uint32_t
b3862_mul_sat(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64P;

	u64P = (uint64_t)u32A * (uint64_t)u32B;
	if (u64P > (uint64_t)UINT32_MAX) {
		return UINT32_MAX;
	}
	return (uint32_t)u64P;
}

/* Saturating a + b in the uint32_t domain. */
static uint32_t
b3862_add_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_madd_sat_u - saturating multiply-add: sat(sat(a*b) + c).
 *
 * a, b: factors
 * c:    addend
 *
 * Product overflow or sum overflow clamps to UINT32_MAX.
 * No parent wires.
 */
uint32_t
gj_u32_madd_sat_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b3862_add_sat(b3862_mul_sat(u32A, u32B), u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_madd_sat_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_madd_sat_u")));
