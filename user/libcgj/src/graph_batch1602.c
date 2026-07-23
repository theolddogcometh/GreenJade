/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1602: axis-aligned int32 rect intersection.
 *
 * Surface (unique symbols):
 *   int gj_rect_i32_intersect(int32_t ax0, int32_t ay0, int32_t ax1,
 *                             int32_t ay1, int32_t bx0, int32_t by0,
 *                             int32_t bx1, int32_t by1, int32_t *ox0,
 *                             int32_t *oy0, int32_t *ox1, int32_t *oy1);
 *     — Half-open rects [x0,x1)×[y0,y1). Writes intersection to out
 *       pointers (NULL out-pointers skipped). Returns 1 if non-empty
 *       intersection, else 0 (outs left unset on empty when written).
 *   int __gj_rect_i32_intersect  (alias)
 *   __libcgj_batch1602_marker = "libcgj-batch1602"
 *
 * Distinct from gj_rect_i32_union (batch1603) / contains (batch1601).
 * Geometry integer exclusive (beyond mat2). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1602_marker[] = "libcgj-batch1602";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1602_max_i32(int32_t i32A, int32_t i32B)
{
	return (i32A > i32B) ? i32A : i32B;
}

static int32_t
b1602_min_i32(int32_t i32A, int32_t i32B)
{
	return (i32A < i32B) ? i32A : i32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_i32_intersect — half-open AABB intersection.
 *
 * Intersection is [max(ax0,bx0), min(ax1,bx1)) × same for y.
 * Empty when ox0 >= ox1 or oy0 >= oy1.
 */
int
gj_rect_i32_intersect(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1, int32_t *pOx0, int32_t *pOy0, int32_t *pOx1,
    int32_t *pOy1)
{
	int32_t i32X0;
	int32_t i32Y0;
	int32_t i32X1;
	int32_t i32Y1;

	i32X0 = b1602_max_i32(i32Ax0, i32Bx0);
	i32Y0 = b1602_max_i32(i32Ay0, i32By0);
	i32X1 = b1602_min_i32(i32Ax1, i32Bx1);
	i32Y1 = b1602_min_i32(i32Ay1, i32By1);

	if (i32X0 >= i32X1 || i32Y0 >= i32Y1) {
		return 0;
	}

	if (pOx0 != NULL) {
		*pOx0 = i32X0;
	}
	if (pOy0 != NULL) {
		*pOy0 = i32Y0;
	}
	if (pOx1 != NULL) {
		*pOx1 = i32X1;
	}
	if (pOy1 != NULL) {
		*pOy1 = i32Y1;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_i32_intersect(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1, int32_t *pOx0, int32_t *pOy0, int32_t *pOx1,
    int32_t *pOy1)
    __attribute__((alias("gj_rect_i32_intersect")));
