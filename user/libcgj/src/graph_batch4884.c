/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4884: half-open AABB point containment (_u).
 *
 * Surface (unique symbols):
 *   int gj_rect_contains_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                          uint32_t y1, uint32_t x, uint32_t y);
 *     - Returns 1 if (x,y) lies in half-open [x0,x1)×[y0,y1), else 0.
 *       Empty/inverted rects never contain.
 *   int __gj_rect_contains_u  (alias)
 *   __libcgj_batch4884_marker = "libcgj-batch4884"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_u32_contains (batch1901) /
 * gj_rect_i32_contains_pt (batch1601) — unique gj_rect_contains_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4884_marker[] = "libcgj-batch4884";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4884_contains(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32X, uint32_t u32Y)
{
	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 0;
	}
	if (u32X < u32X0 || u32X >= u32X1) {
		return 0;
	}
	if (u32Y < u32Y0 || u32Y >= u32Y1) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_contains_u - half-open AABB contains point (uint32).
 *
 * x0,y0: inclusive min corner
 * x1,y1: exclusive max corner
 * x,y: query point
 *
 * Returns 1 if contained, 0 otherwise. No parent wires.
 */
int
gj_rect_contains_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32X, uint32_t u32Y)
{
	(void)NULL;
	return b4884_contains(u32X0, u32Y0, u32X1, u32Y1, u32X, u32Y);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_contains_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1, uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_rect_contains_u")));
