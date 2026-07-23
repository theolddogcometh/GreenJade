/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1608: 2D orientation (cross-product sign).
 *
 * Surface (unique symbols):
 *   int gj_orient2d_i32(int32_t ax, int32_t ay, int32_t bx, int32_t by,
 *                       int32_t cx, int32_t cy);
 *     — Sign of (B-A)×(C-A) = (bx-ax)*(cy-ay) - (by-ay)*(cx-ax).
 *       Returns +1 (left / CCW), -1 (right / CW), or 0 (collinear).
 *       Intermediate products computed in int64_t.
 *   int __gj_orient2d_i32  (alias)
 *   __libcgj_batch1608_marker = "libcgj-batch1608"
 *
 * Geometry integer exclusive (beyond mat2). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1608_marker[] = "libcgj-batch1608";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_orient2d_i32 — robust 2D orientation via signed cross product.
 *
 * A,B,C: points. Cross sign tells which side of directed line AB holds C.
 */
int
gj_orient2d_i32(int32_t i32Ax, int32_t i32Ay, int32_t i32Bx, int32_t i32By,
    int32_t i32Cx, int32_t i32Cy)
{
	int64_t i64Abx;
	int64_t i64Aby;
	int64_t i64Acx;
	int64_t i64Acy;
	int64_t i64Cross;

	i64Abx = (int64_t)i32Bx - (int64_t)i32Ax;
	i64Aby = (int64_t)i32By - (int64_t)i32Ay;
	i64Acx = (int64_t)i32Cx - (int64_t)i32Ax;
	i64Acy = (int64_t)i32Cy - (int64_t)i32Ay;
	i64Cross = i64Abx * i64Acy - i64Aby * i64Acx;

	if (i64Cross > 0) {
		return 1;
	}
	if (i64Cross < 0) {
		return -1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_orient2d_i32(int32_t i32Ax, int32_t i32Ay, int32_t i32Bx,
    int32_t i32By, int32_t i32Cx, int32_t i32Cy)
    __attribute__((alias("gj_orient2d_i32")));
