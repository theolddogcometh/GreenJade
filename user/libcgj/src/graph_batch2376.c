/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2376: saturating uint32_t subtract (u-suffix name).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_sub_u(uint32_t a, uint32_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_u32_sat_sub_u  (alias)
 *   __libcgj_batch2376_marker = "libcgj-batch2376"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u32_sat_sub (batch924) - different public name
 * (u32_sat_sub_u vs u32_sat_sub); no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2376_marker[] = "libcgj-batch2376";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b in the uint32_t domain. */
static uint32_t
b2376_sub_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_sub_u - unsigned 32-bit subtract; clamp to 0 on underflow.
 *
 * a: minuend
 * b: subtrahend
 *
 * Underflow when a < b.
 */
uint32_t
gj_u32_sat_sub_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2376_sub_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_sub_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sat_sub_u")));
