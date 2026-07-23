/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3029: saturating uint32_t twice (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_twice_sat_u(uint32_t x);
 *     - Return 2 * x, saturating at UINT32_MAX on overflow (no wrap).
 *       gj_u32_twice_sat_u(0) == 0.
 *       gj_u32_twice_sat_u(1) == 2.
 *       gj_u32_twice_sat_u(UINT32_MAX) == UINT32_MAX.
 *   uint32_t __gj_u32_twice_sat_u  (alias)
 *   __libcgj_batch3029_marker = "libcgj-batch3029"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u32_twice_sat_u surface only; no multi-def. Distinct from
 * gj_u32_sat_mul_u_soft (batch2712). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3029_marker[] = "libcgj-batch3029";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating double: overflow when x > floor(UINT32_MAX / 2), i.e.
 * x > (UINT32_MAX >> 1).
 */
static uint32_t
b3029_twice_sat(uint32_t u32X)
{
	if (u32X > (UINT32_MAX >> 1)) {
		return UINT32_MAX;
	}
	return u32X << 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_twice_sat_u - return 2 * x saturating at UINT32_MAX.
 *
 * No wrap-around. No parent wires.
 */
uint32_t
gj_u32_twice_sat_u(uint32_t u32X)
{
	(void)NULL;
	return b3029_twice_sat(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_twice_sat_u(uint32_t u32X)
    __attribute__((alias("gj_u32_twice_sat_u")));
