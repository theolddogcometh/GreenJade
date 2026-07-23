/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1904: axis-aligned uint32 rect emptiness test.
 *
 * Surface (unique symbols):
 *   int gj_rect_u32_empty_p(uint32_t x0, uint32_t y0, uint32_t x1,
 *                           uint32_t y1);
 *     — Half-open rect [x0,x1)×[y0,y1). Returns 1 if empty (x0>=x1 or
 *       y0>=y1), else 0.
 *   int __gj_rect_u32_empty_p  (alias)
 *   __libcgj_batch1904_marker = "libcgj-batch1904"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Distinct from
 * gj_rect_i32_empty (batch1605). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1904_marker[] = "libcgj-batch1904";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if half-open rect has zero width or zero height. */
static int
b1904_empty_p(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1, uint32_t u32Y1)
{
	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_u32_empty_p — half-open AABB emptiness predicate (uint32).
 *
 * Returns 1 if the rect has zero width or zero height, else 0.
 */
int
gj_rect_u32_empty_p(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	(void)NULL;
	return b1904_empty_p(u32X0, u32Y0, u32X1, u32Y1);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_u32_empty_p(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_rect_u32_empty_p")));
