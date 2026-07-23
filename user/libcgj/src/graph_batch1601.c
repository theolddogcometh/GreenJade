/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1601: axis-aligned int32 rect point containment.
 *
 * Surface (unique symbols):
 *   int gj_rect_i32_contains_pt(int32_t x0, int32_t y0, int32_t x1,
 *                               int32_t y1, int32_t px, int32_t py);
 *     — Half-open rect [x0,x1) × [y0,y1). Returns 1 if (px,py) is
 *       inside, else 0. Empty rects (x0>=x1 or y0>=y1) never contain.
 *   int __gj_rect_i32_contains_pt  (alias)
 *   __libcgj_batch1601_marker = "libcgj-batch1601"
 *
 * Geometry integer exclusive (beyond mat2). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1601_marker[] = "libcgj-batch1601";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_i32_contains_pt — half-open AABB point-in-rect test.
 *
 * x0,y0: inclusive min corner
 * x1,y1: exclusive max corner
 * px,py: query point
 *
 * Returns 1 if contained, 0 otherwise.
 */
int
gj_rect_i32_contains_pt(int32_t i32X0, int32_t i32Y0, int32_t i32X1,
    int32_t i32Y1, int32_t i32Px, int32_t i32Py)
{
	if (i32X0 >= i32X1 || i32Y0 >= i32Y1) {
		return 0;
	}
	if (i32Px < i32X0 || i32Px >= i32X1) {
		return 0;
	}
	if (i32Py < i32Y0 || i32Py >= i32Y1) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_i32_contains_pt(int32_t i32X0, int32_t i32Y0, int32_t i32X1,
    int32_t i32Y1, int32_t i32Px, int32_t i32Py)
    __attribute__((alias("gj_rect_i32_contains_pt")));
