/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1901: axis-aligned uint32 rect point containment.
 *
 * Surface (unique symbols):
 *   int gj_rect_u32_contains(uint32_t x0, uint32_t y0, uint32_t x1,
 *                            uint32_t y1, uint32_t px, uint32_t py);
 *     — Half-open rect [x0,x1)×[y0,y1). Returns 1 if (px,py) is inside,
 *       else 0. Empty rects (x0>=x1 or y0>=y1) never contain.
 *   int __gj_rect_u32_contains  (alias)
 *   __libcgj_batch1901_marker = "libcgj-batch1901"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Distinct from
 * gj_rect_i32_contains_pt (batch1601). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1901_marker[] = "libcgj-batch1901";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if half-open rect contains point. */
static int
b1901_contains(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32Px, uint32_t u32Py)
{
	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 0;
	}
	if (u32Px < u32X0 || u32Px >= u32X1) {
		return 0;
	}
	if (u32Py < u32Y0 || u32Py >= u32Y1) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_u32_contains — half-open AABB point-in-rect test (uint32).
 *
 * x0,y0: inclusive min corner
 * x1,y1: exclusive max corner
 * px,py: query point
 *
 * Returns 1 if contained, 0 otherwise.
 */
int
gj_rect_u32_contains(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32Px, uint32_t u32Py)
{
	(void)NULL;
	return b1901_contains(u32X0, u32Y0, u32X1, u32Y1, u32Px, u32Py);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_u32_contains(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32Px, uint32_t u32Py)
    __attribute__((alias("gj_rect_u32_contains")));
