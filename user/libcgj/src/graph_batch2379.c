/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2379: saturating uint32_t multiply (u-suffix name).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_mul_u(uint32_t a, uint32_t b);
 *     - a * b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_sat_mul_u  (alias)
 *   __libcgj_batch2379_marker = "libcgj-batch2379"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u32_saturating_mul (batch1373) - different public
 * name (u32_sat_mul_u vs u32_saturating_mul); no multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2379_marker[] = "libcgj-batch2379";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overflow iff a > floor(UINT32_MAX / b) for nonzero b. */
static int
b2379_umul_overflows_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0;
	}
	return (u32A > (UINT32_MAX / u32B)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_mul_u - unsigned 32-bit multiply; clamp to UINT32_MAX.
 *
 * a, b: factors
 *
 * Zero factors are exact (product 0). Overflow when a > floor(UINT32_MAX / b).
 */
uint32_t
gj_u32_sat_mul_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	if (b2379_umul_overflows_u32(u32A, u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_mul_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sat_mul_u")));
