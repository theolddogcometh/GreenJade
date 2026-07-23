/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1604: axis-aligned int32 rect area.
 *
 * Surface (unique symbols):
 *   int64_t gj_rect_i32_area(int32_t x0, int32_t y0, int32_t x1,
 *                            int32_t y1);
 *     — Half-open rect [x0,x1)×[y0,y1). Returns (x1-x0)*(y1-y0) as
 *       int64_t when non-empty, else 0. Widens before multiply.
 *   int64_t __gj_rect_i32_area  (alias)
 *   __libcgj_batch1604_marker = "libcgj-batch1604"
 *
 * Distinct from empty/contains/intersect/union siblings. Geometry
 * integer exclusive (beyond mat2). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1604_marker[] = "libcgj-batch1604";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_i32_area — half-open AABB area in unit squares.
 *
 * Empty when x0 >= x1 or y0 >= y1 → 0. Product computed in int64_t.
 */
int64_t
gj_rect_i32_area(int32_t i32X0, int32_t i32Y0, int32_t i32X1, int32_t i32Y1)
{
	int64_t i64W;
	int64_t i64H;

	if (i32X0 >= i32X1 || i32Y0 >= i32Y1) {
		return 0;
	}
	i64W = (int64_t)i32X1 - (int64_t)i32X0;
	i64H = (int64_t)i32Y1 - (int64_t)i32Y0;
	return i64W * i64H;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_rect_i32_area(int32_t i32X0, int32_t i32Y0, int32_t i32X1,
    int32_t i32Y1)
    __attribute__((alias("gj_rect_i32_area")));
