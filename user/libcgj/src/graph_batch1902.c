/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1902: axis-aligned uint32 rect intersect predicate.
 *
 * Surface (unique symbols):
 *   int gj_rect_u32_intersect_p(uint32_t ax0, uint32_t ay0, uint32_t ax1,
 *                               uint32_t ay1, uint32_t bx0, uint32_t by0,
 *                               uint32_t bx1, uint32_t by1);
 *     — Half-open rects [x0,x1)×[y0,y1). Returns 1 if interiors overlap
 *       (non-empty intersection), else 0. Empty inputs → 0. Predicate
 *       only; does not write intersection corners.
 *   int __gj_rect_u32_intersect_p  (alias)
 *   __libcgj_batch1902_marker = "libcgj-batch1902"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Distinct from
 * gj_rect_i32_intersect (batch1602) and gj_aabb_overlap_i32 (batch1606).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1902_marker[] = "libcgj-batch1902";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1902_max_u32(uint32_t u32A, uint32_t u32B)
{
	return (u32A > u32B) ? u32A : u32B;
}

static uint32_t
b1902_min_u32(uint32_t u32A, uint32_t u32B)
{
	return (u32A < u32B) ? u32A : u32B;
}

/* 1 if half-open AABBs have non-empty intersection. */
static int
b1902_intersect_p(uint32_t u32Ax0, uint32_t u32Ay0, uint32_t u32Ax1,
    uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0, uint32_t u32Bx1,
    uint32_t u32By1)
{
	uint32_t u32X0;
	uint32_t u32Y0;
	uint32_t u32X1;
	uint32_t u32Y1;

	if (u32Ax0 >= u32Ax1 || u32Ay0 >= u32Ay1) {
		return 0;
	}
	if (u32Bx0 >= u32Bx1 || u32By0 >= u32By1) {
		return 0;
	}

	u32X0 = b1902_max_u32(u32Ax0, u32Bx0);
	u32Y0 = b1902_max_u32(u32Ay0, u32By0);
	u32X1 = b1902_min_u32(u32Ax1, u32Bx1);
	u32Y1 = b1902_min_u32(u32Ay1, u32By1);

	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_u32_intersect_p — half-open AABB overlap predicate (uint32).
 *
 * True iff max(ax0,bx0) < min(ax1,bx1) and same for y.
 */
int
gj_rect_u32_intersect_p(uint32_t u32Ax0, uint32_t u32Ay0, uint32_t u32Ax1,
    uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0, uint32_t u32Bx1,
    uint32_t u32By1)
{
	(void)NULL;
	return b1902_intersect_p(u32Ax0, u32Ay0, u32Ax1, u32Ay1, u32Bx0,
	    u32By0, u32Bx1, u32By1);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_u32_intersect_p(uint32_t u32Ax0, uint32_t u32Ay0,
    uint32_t u32Ax1, uint32_t u32Ay1, uint32_t u32Bx0, uint32_t u32By0,
    uint32_t u32Bx1, uint32_t u32By1)
    __attribute__((alias("gj_rect_u32_intersect_p")));
