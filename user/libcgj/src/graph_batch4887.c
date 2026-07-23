/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4887: half-open AABB emptiness predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_rect_empty_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                       uint32_t y1);
 *     - Returns 1 if half-open [x0,x1)×[y0,y1) is empty (x0>=x1 or
 *       y0>=y1), else 0.
 *   int __gj_rect_empty_u  (alias)
 *   __libcgj_batch4887_marker = "libcgj-batch4887"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_u32_empty_p (batch1904) / gj_rect_i32_empty
 * (batch1605) — unique gj_rect_empty_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4887_marker[] = "libcgj-batch4887";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4887_empty(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_empty_u - half-open AABB emptiness predicate (uint32).
 *
 * x0,y0: inclusive min corner
 * x1,y1: exclusive max corner
 *
 * Returns 1 if empty, 0 if non-empty. No parent wires.
 */
int
gj_rect_empty_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	(void)NULL;
	return b4887_empty(u32X0, u32Y0, u32X1, u32Y1);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rect_empty_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_rect_empty_u")));
