/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2378: saturating uint16_t multiply.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_sat_mul(uint16_t a, uint16_t b);
 *     - a * b with saturation at UINT16_MAX on overflow (no wrap).
 *   uint16_t __gj_u16_sat_mul  (alias)
 *   __libcgj_batch2378_marker = "libcgj-batch2378"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u8_sat_mul (batch2377) and gj_u32_saturating_mul
 * (batch1373) - 16-bit surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2378_marker[] = "libcgj-batch2378";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overflow if a*b exceeds UINT16_MAX (product computed in wider type). */
static int
b2378_umul_overflows_u16(uint16_t u16A, uint16_t u16B)
{
	uint32_t u32Prod;

	u32Prod = (uint32_t)u16A * (uint32_t)u16B;
	return (u32Prod > (uint32_t)UINT16_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_sat_mul - unsigned 16-bit multiply; clamp to UINT16_MAX on overflow.
 *
 * a, b: factors
 *
 * Zero factors are exact (product 0). Overflow when a*b > UINT16_MAX.
 */
uint16_t
gj_u16_sat_mul(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	if (b2378_umul_overflows_u16(u16A, u16B)) {
		return UINT16_MAX;
	}
	return (uint16_t)(u16A * u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_sat_mul(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_sat_mul")));
