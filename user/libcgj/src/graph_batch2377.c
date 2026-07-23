/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2377: saturating uint8_t multiply.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_sat_mul(uint8_t a, uint8_t b);
 *     - a * b with saturation at UINT8_MAX on overflow (no wrap).
 *   uint8_t __gj_u8_sat_mul  (alias)
 *   __libcgj_batch2377_marker = "libcgj-batch2377"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u32_saturating_mul (batch1373) - 8-bit surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2377_marker[] = "libcgj-batch2377";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overflow if a*b exceeds UINT8_MAX (product computed in wider type). */
static int
b2377_umul_overflows_u8(uint8_t u8A, uint8_t u8B)
{
	uint16_t u16Prod;

	u16Prod = (uint16_t)u8A * (uint16_t)u8B;
	return (u16Prod > (uint16_t)UINT8_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sat_mul - unsigned 8-bit multiply; clamp to UINT8_MAX on overflow.
 *
 * a, b: factors
 *
 * Zero factors are exact (product 0). Overflow when a*b > UINT8_MAX.
 */
uint8_t
gj_u8_sat_mul(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	if (b2377_umul_overflows_u8(u8A, u8B)) {
		return UINT8_MAX;
	}
	return (uint8_t)(u8A * u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_sat_mul(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_sat_mul")));
