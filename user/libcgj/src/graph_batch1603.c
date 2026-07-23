/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1603: axis-aligned int32 rect union (AABB hull).
 *
 * Surface (unique symbols):
 *   int gj_rect_i32_union(int32_t ax0, int32_t ay0, int32_t ax1,
 *                         int32_t ay1, int32_t bx0, int32_t by0,
 *                         int32_t bx1, int32_t by1, int32_t *ox0,
 *                         int32_t *oy0, int32_t *ox1, int32_t *oy1);
 *     — Half-open rects. Writes the axis-aligned bounding union of two
 *       non-empty rects. If one side is empty, copies the other. If both
 *       empty, returns 0. NULL out-pointers skipped. Returns 1 on
 *       non-empty result.
 *   int __gj_rect_i32_union  (alias)
 *   __libcgj_batch1603_marker = "libcgj-batch1603"
 *
 * Distinct from gj_rect_i32_intersect (batch1602). Geometry integer
 * exclusive (beyond mat2). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1603_marker[] = "libcgj-batch1603";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1603_max_i32(int32_t i32A, int32_t i32B)
{
	return (i32A > i32B) ? i32A : i32B;
}

static int32_t
b1603_min_i32(int32_t i32A, int32_t i32B)
{
	return (i32A < i32B) ? i32A : i32B;
}

static int
b1603_empty(int32_t i32X0, int32_t i32Y0, int32_t i32X1, int32_t i32Y1)
{
	return (i32X0 >= i32X1 || i32Y0 >= i32Y1) ? 1 : 0;
}

static void
b1603_write(int32_t i32X0, int32_t i32Y0, int32_t i32X1, int32_t i32Y1,
    int32_t *pOx0, int32_t *pOy0, int32_t *pOx1, int32_t *pOy1)
{
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
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_i32_union — half-open AABB hull of two rects.
 *
 * Union AABB is [min(x0), max(x1)) × [min(y0), max(y1)) for non-empty
 * inputs. Empty inputs are ignored (identity for the non-empty peer).
 */
int
gj_rect_i32_union(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1, int32_t *pOx0, int32_t *pOy0, int32_t *pOx1,
    int32_t *pOy1)
{
	int fAEmpty;
	int fBEmpty;
	int32_t i32X0;
	int32_t i32Y0;
	int32_t i32X1;
	int32_t i32Y1;

	fAEmpty = b1603_empty(i32Ax0, i32Ay0, i32Ax1, i32Ay1);
	fBEmpty = b1603_empty(i32Bx0, i32By0, i32Bx1, i32By1);

	if (fAEmpty != 0 && fBEmpty != 0) {
		return 0;
	}
	if (fAEmpty != 0) {
		b1603_write(i32Bx0, i32By0, i32Bx1, i32By1, pOx0, pOy0, pOx1,
		    pOy1);
		return 1;
	}
	if (fBEmpty != 0) {
		b1603_write(i32Ax0, i32Ay0, i32Ax1, i32Ay1, pOx0, pOy0, pOx1,
		    pOy1);
		return 1;
	}

	i32X0 = b1603_min_i32(i32Ax0, i32Bx0);
	i32Y0 = b1603_min_i32(i32Ay0, i32By0);
	i32X1 = b1603_max_i32(i32Ax1, i32Bx1);
	i32Y1 = b1603_max_i32(i32Ay1, i32By1);
	b1603_write(i32X0, i32Y0, i32X1, i32Y1, pOx0, pOy0, pOx1, pOy1);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_i32_union(int32_t i32Ax0, int32_t i32Ay0, int32_t i32Ax1,
    int32_t i32Ay1, int32_t i32Bx0, int32_t i32By0, int32_t i32Bx1,
    int32_t i32By1, int32_t *pOx0, int32_t *pOy0, int32_t *pOx1,
    int32_t *pOy1)
    __attribute__((alias("gj_rect_i32_union")));
