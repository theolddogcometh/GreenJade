/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3075: squared Euclidean distance (uint32) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_dist2_u32_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                           uint32_t y1);
 *     - Returns dx*dx + dy*dy where dx = |x1-x0|, dy = |y1-y0|.
 *       Each square fits in uint64_t; the sum saturates at UINT64_MAX
 *       if it would overflow (only when both |d| are near UINT32_MAX).
 *   uint64_t __gj_dist2_u32_u  (alias)
 *   __libcgj_batch3075_marker = "libcgj-batch3075"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Distinct from
 * gj_dist2_i32 (batch1609) — unique gj_dist2_u32_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3075_marker[] = "libcgj-batch3075";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3075_absdiff_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dist2_u32_u - squared Euclidean distance between two uint32 points.
 *
 * Prefer over hypot when only comparisons matter. No parent wires.
 */
uint64_t
gj_dist2_u32_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	uint32_t u32Dx;
	uint32_t u32Dy;
	uint64_t u64Xx;
	uint64_t u64Yy;
	uint64_t u64Sum;

	(void)NULL;

	u32Dx = b3075_absdiff_u32(u32X0, u32X1);
	u32Dy = b3075_absdiff_u32(u32Y0, u32Y1);
	u64Xx = (uint64_t)u32Dx * (uint64_t)u32Dx;
	u64Yy = (uint64_t)u32Dy * (uint64_t)u32Dy;
	u64Sum = u64Xx + u64Yy;
	if (u64Sum < u64Xx) {
		return UINT64_MAX;
	}
	return u64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dist2_u32_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_dist2_u32_u")));
