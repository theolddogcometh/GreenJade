/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4882: axis-aligned rect height (uint32, _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rect_h_u(uint32_t y0, uint32_t y1);
 *     - Returns half-open height (y1 - y0) when y1 >= y0, else 0.
 *   uint32_t __gj_rect_h_u  (alias)
 *   __libcgj_batch4882_marker = "libcgj-batch4882"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_w_u (batch4881) — unique gj_rect_h_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4882_marker[] = "libcgj-batch4882";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4882_height(uint32_t u32Y0, uint32_t u32Y1)
{
	if (u32Y1 < u32Y0) {
		return 0u;
	}
	return u32Y1 - u32Y0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_h_u - half-open AABB height along Y (uint32).
 *
 * y0: inclusive min Y
 * y1: exclusive max Y
 *
 * Returns (y1 - y0) when y1 >= y0, else 0 (empty/inverted span).
 * Self-contained; no parent wires.
 */
uint32_t
gj_rect_h_u(uint32_t u32Y0, uint32_t u32Y1)
{
	(void)NULL;
	return b4882_height(u32Y0, u32Y1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rect_h_u(uint32_t u32Y0, uint32_t u32Y1)
    __attribute__((alias("gj_rect_h_u")));
