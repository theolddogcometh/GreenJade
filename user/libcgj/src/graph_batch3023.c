/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3023: saturating uint32_t increment (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_inc_sat_u(uint32_t x);
 *     - Return x + 1, saturating at UINT32_MAX (no wrap).
 *       gj_u32_inc_sat_u(UINT32_MAX) == UINT32_MAX.
 *       gj_u32_inc_sat_u(0) == 1.
 *   uint32_t __gj_u32_inc_sat_u  (alias)
 *   __libcgj_batch3023_marker = "libcgj-batch3023"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u32_inc_sat_u surface only; no multi-def. Distinct from
 * gj_u64_inc_sat (batch649) and gj_u64_clamped_inc (batch927). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3023_marker[] = "libcgj-batch3023";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating increment at UINT32_MAX. */
static uint32_t
b3023_inc_sat(uint32_t u32X)
{
	if (u32X == UINT32_MAX) {
		return UINT32_MAX;
	}
	return u32X + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_inc_sat_u - increment x with saturation at UINT32_MAX.
 *
 * If x is already UINT32_MAX, return UINT32_MAX (no wrap-around).
 * Otherwise return x + 1. No parent wires.
 */
uint32_t
gj_u32_inc_sat_u(uint32_t u32X)
{
	(void)NULL;
	return b3023_inc_sat(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_inc_sat_u(uint32_t u32X)
    __attribute__((alias("gj_u32_inc_sat_u")));
