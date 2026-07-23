/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2707: saturating uint32_t add (add_sat_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_add_sat_u(uint32_t a, uint32_t b);
 *     - a + b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_add_sat_u  (alias)
 *   __libcgj_batch2707_marker = "libcgj-batch2707"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Distinct from gj_u32_sat_add (batch923) and gj_u32_sat_add_u
 * (batch2375) — unique add_sat_u name order; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2707_marker[] = "libcgj-batch2707";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint32_t domain. */
static uint32_t
b2707_add_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_add_sat_u - unsigned 32-bit add; clamp to UINT32_MAX on overflow.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT32_MAX - a.
 */
uint32_t
gj_u32_add_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2707_add_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_add_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_add_sat_u")));
