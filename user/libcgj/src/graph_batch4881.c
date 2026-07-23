/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4881: axis-aligned rect width (uint32, _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rect_w_u(uint32_t x0, uint32_t x1);
 *     - Returns half-open width (x1 - x0) when x1 >= x0, else 0.
 *   uint32_t __gj_rect_w_u  (alias)
 *   __libcgj_batch4881_marker = "libcgj-batch4881"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_u32_area (batch1903) — unique gj_rect_w_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4881_marker[] = "libcgj-batch4881";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4881_width(uint32_t u32X0, uint32_t u32X1)
{
	if (u32X1 < u32X0) {
		return 0u;
	}
	return u32X1 - u32X0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_w_u - half-open AABB width along X (uint32).
 *
 * x0: inclusive min X
 * x1: exclusive max X
 *
 * Returns (x1 - x0) when x1 >= x0, else 0 (empty/inverted span).
 * Self-contained; no parent wires.
 */
uint32_t
gj_rect_w_u(uint32_t u32X0, uint32_t u32X1)
{
	(void)NULL;
	return b4881_width(u32X0, u32X1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rect_w_u(uint32_t u32X0, uint32_t u32X1)
    __attribute__((alias("gj_rect_w_u")));
