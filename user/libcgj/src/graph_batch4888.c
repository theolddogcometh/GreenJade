/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4888: Manhattan (L1) distance uint32 points.
 *
 * Surface (unique symbols):
 *   uint64_t gj_manhattan_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                           uint32_t y1);
 *     - Returns |x1-x0| + |y1-y0| as uint64_t (exact for full uint32
 *       domain; max sum is 2*(2^32-1) which fits in 33 bits).
 *   uint64_t __gj_manhattan_u  (alias)
 *   __libcgj_batch4888_marker = "libcgj-batch4888"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_manhattan_u32_u (batch3077) / gj_manhattan_u64_u
 * (batch3078) — unique gj_manhattan_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4888_marker[] = "libcgj-batch4888";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4888_absdiff(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manhattan_u - L1 distance between two uint32 points.
 *
 * x0,y0: first point
 * x1,y1: second point
 *
 * Returns |x1-x0| + |y1-y0| as uint64_t. No parent wires.
 */
uint64_t
gj_manhattan_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	uint32_t u32Dx;
	uint32_t u32Dy;

	(void)NULL;

	u32Dx = b4888_absdiff(u32X0, u32X1);
	u32Dy = b4888_absdiff(u32Y0, u32Y1);
	return (uint64_t)u32Dx + (uint64_t)u32Dy;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_manhattan_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_manhattan_u")));
