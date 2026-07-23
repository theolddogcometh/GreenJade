/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1605: axis-aligned int32 rect emptiness test.
 *
 * Surface (unique symbols):
 *   int gj_rect_i32_empty(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
 *     — Half-open rect [x0,x1)×[y0,y1). Returns 1 if empty (x0>=x1 or
 *       y0>=y1), else 0.
 *   int __gj_rect_i32_empty  (alias)
 *   __libcgj_batch1605_marker = "libcgj-batch1605"
 *
 * Distinct from area/contains siblings. Geometry integer exclusive
 * (beyond mat2). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1605_marker[] = "libcgj-batch1605";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_i32_empty — half-open AABB emptiness predicate.
 *
 * Returns 1 if the rect has zero width or zero height, else 0.
 */
int
gj_rect_i32_empty(int32_t i32X0, int32_t i32Y0, int32_t i32X1, int32_t i32Y1)
{
	if (i32X0 >= i32X1 || i32Y0 >= i32Y1) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_i32_empty(int32_t i32X0, int32_t i32Y0, int32_t i32X1,
    int32_t i32Y1)
    __attribute__((alias("gj_rect_i32_empty")));
