/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4885: half-open AABB overlap predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_rect_intersect_u(uint32_t ax0, uint32_t ay0, uint32_t ax1,
 *                           uint32_t ay1, uint32_t bx0, uint32_t by0,
 *                           uint32_t bx1, uint32_t by1);
 *     - Returns 1 if half-open rects A and B have non-empty overlap,
 *       else 0. Empty inputs never intersect.
 *   int __gj_rect_intersect_u  (alias)
 *   __libcgj_batch4885_marker = "libcgj-batch4885"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_i32_intersect (batch1602) — unique
 * gj_rect_intersect_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4885_marker[] = "libcgj-batch4885";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4885_empty(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	return (u32X0 >= u32X1 || u32Y0 >= u32Y1) ? 1 : 0;
}

/* Half-open intervals [a0,a1) and [b0,b1) overlap iff a0 < b1 && b0 < a1. */
static int
b4885_overlap(uint32_t u32Ax0, uint32_t u32Ay0, uint32_t u32Ax1,
    uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0, uint32_t u32Bx1,
    uint32_t u32By1)
{
	if (b4885_empty(u32Ax0, u32Ay0, u32Ax1, u32Ay1)) {
		return 0;
	}
	if (b4885_empty(u32Bx0, u32By0, u32Bx1, u32By1)) {
		return 0;
	}
	if (u32Ax0 >= u32Bx1 || u32Bx0 >= u32Ax1) {
		return 0;
	}
	if (u32Ay0 >= u32By1 || u32By0 >= u32Ay1) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_intersect_u - half-open AABB overlap test (uint32).
 *
 * ax0,ay0,ax1,ay1: rect A [ax0,ax1)×[ay0,ay1)
 * bx0,by0,bx1,by1: rect B [bx0,bx1)×[by0,by1)
 *
 * Returns 1 if the intersection is non-empty, else 0.
 * No parent wires.
 */
int
gj_rect_intersect_u(uint32_t u32Ax0, uint32_t u32Ay0, uint32_t u32Ax1,
    uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0, uint32_t u32Bx1,
    uint32_t u32By1)
{
	(void)NULL;
	return b4885_overlap(u32Ax0, u32Ay0, u32Ax1, u32Ay1, u32Bx0,
	    u32By0, u32Bx1, u32By1);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_intersect_u(uint32_t u32Ax0, uint32_t u32Ay0,
    uint32_t u32Ax1, uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0,
    uint32_t u32Bx1, uint32_t u32By1)
    __attribute__((alias("gj_rect_intersect_u")));
