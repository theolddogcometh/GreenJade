/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1606: axis-aligned int32 AABB overlap test.
 *
 * Surface (unique symbols):
 *   int gj_aabb_overlap_i32(int32_t ax0, int32_t ay0, int32_t ax1,
 *                           int32_t ay1, int32_t bx0, int32_t by0,
 *                           int32_t bx1, int32_t by1);
 *     — Half-open AABBs [x0,x1)×[y0,y1). Returns 1 if interiors
 *       overlap (non-empty intersection), else 0. Empty inputs → 0.
 *   int __gj_aabb_overlap_i32  (alias)
 *   __libcgj_batch1606_marker = "libcgj-batch1606"
 *
 * Distinct from gj_rect_i32_intersect (batch1602) which also writes the
 * intersection box. Geometry integer exclusive (beyond mat2). Unique
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1606_marker[] = "libcgj-batch1606";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1606_max_i32(int32_t i32A, int32_t i32B)
{
	return (i32A > i32B) ? i32A : i32B;
}

static int32_t
b1606_min_i32(int32_t i32A, int32_t i32B)
{
	return (i32A < i32B) ? i32A : i32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aabb_overlap_i32 — half-open AABB overlap predicate.
 *
 * True iff max(ax0,bx0) < min(ax1,bx1) and same for y.
 */
int
gj_aabb_overlap_i32(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1)
{
	int32_t i32X0;
	int32_t i32Y0;
	int32_t i32X1;
	int32_t i32Y1;

	if (i32Ax0 >= i32Ax1 || i32Ay0 >= i32Ay1) {
		return 0;
	}
	if (i32Bx0 >= i32Bx1 || i32By0 >= i32By1) {
		return 0;
	}

	i32X0 = b1606_max_i32(i32Ax0, i32Bx0);
	i32Y0 = b1606_max_i32(i32Ay0, i32By0);
	i32X1 = b1606_min_i32(i32Ax1, i32Bx1);
	i32Y1 = b1606_min_i32(i32Ay1, i32By1);

	if (i32X0 >= i32X1 || i32Y0 >= i32Y1) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_aabb_overlap_i32(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1)
    __attribute__((alias("gj_aabb_overlap_i32")));
