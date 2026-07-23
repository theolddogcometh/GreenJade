/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1607: integer barycentric point-in-triangle.
 *
 * Surface (unique symbols):
 *   int gj_point_in_triangle_i32(int32_t ax, int32_t ay, int32_t bx,
 *                                int32_t by, int32_t cx, int32_t cy,
 *                                int32_t px, int32_t py);
 *     — Returns 1 if P is inside or on the boundary of triangle ABC
 *       using integer barycentric coordinates (no division; compare
 *       against signed area). Degenerate (zero area) → 0.
 *   int __gj_point_in_triangle_i32  (alias)
 *   __libcgj_batch1607_marker = "libcgj-batch1607"
 *
 * Geometry integer exclusive (beyond mat2). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1607_marker[] = "libcgj-batch1607";

/* ---- freestanding helpers ---------------------------------------------- */

/* Cross (B-A) × (P-A) in int64, expanded to avoid mid-overflow. */
static int64_t
b1607_cross(int32_t i32Ax, int32_t i32Ay, int32_t i32Bx, int32_t i32By,
    int32_t i32Px, int32_t i32Py)
{
	int64_t i64Abx;
	int64_t i64Aby;
	int64_t i64Apx;
	int64_t i64Apy;

	i64Abx = (int64_t)i32Bx - (int64_t)i32Ax;
	i64Aby = (int64_t)i32By - (int64_t)i32Ay;
	i64Apx = (int64_t)i32Px - (int64_t)i32Ax;
	i64Apy = (int64_t)i32Py - (int64_t)i32Ay;
	return i64Abx * i64Apy - i64Aby * i64Apx;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_point_in_triangle_i32 — barycentric / same-orientation PIP test.
 *
 * Computes oriented edges AB, BC, CA relative to P. P is inside (or on
 * boundary) iff all three cross products share the same sign (or zero),
 * matching the triangle's winding. Zero total area → outside.
 *
 * Algebraically equivalent to non-negative barycentric weights with
 * sum 1, evaluated without division via signed-area comparisons.
 */
int
gj_point_in_triangle_i32(int32_t i32Ax, int32_t i32Ay, int32_t i32Bx,
    int32_t i32By, int32_t i32Cx, int32_t i32Cy, int32_t i32Px,
    int32_t i32Py)
{
	int64_t i64C0;
	int64_t i64C1;
	int64_t i64C2;
	int64_t i64Area;

	i64C0 = b1607_cross(i32Ax, i32Ay, i32Bx, i32By, i32Px, i32Py);
	i64C1 = b1607_cross(i32Bx, i32By, i32Cx, i32Cy, i32Px, i32Py);
	i64C2 = b1607_cross(i32Cx, i32Cy, i32Ax, i32Ay, i32Px, i32Py);

	/* Triangle signed area * 2 = cross(A,B,C). */
	i64Area = b1607_cross(i32Ax, i32Ay, i32Bx, i32By, i32Cx, i32Cy);
	if (i64Area == 0) {
		return 0;
	}

	if (i64Area > 0) {
		if (i64C0 < 0 || i64C1 < 0 || i64C2 < 0) {
			return 0;
		}
		return 1;
	}
	/* area < 0: clockwise winding */
	if (i64C0 > 0 || i64C1 > 0 || i64C2 > 0) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_point_in_triangle_i32(int32_t i32Ax, int32_t i32Ay, int32_t i32Bx,
    int32_t i32By, int32_t i32Cx, int32_t i32Cy, int32_t i32Px,
    int32_t i32Py)
    __attribute__((alias("gj_point_in_triangle_i32")));
