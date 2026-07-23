/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4883: rect area from width and height (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rect_area_u(uint32_t w, uint32_t h);
 *     - Returns (uint64_t)w * (uint64_t)h without overflow wrap of the
 *       product (exact for full uint32 domain).
 *   uint64_t __gj_rect_area_u  (alias)
 *   __libcgj_batch4883_marker = "libcgj-batch4883"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_rect_u32_area (batch1903) / gj_rect_i32_area
 * (batch1604) — unique gj_rect_area_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4883_marker[] = "libcgj-batch4883";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4883_area(uint32_t u32W, uint32_t u32H)
{
	return (uint64_t)u32W * (uint64_t)u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_area_u - area of a rect given width and height (uint32).
 *
 * w: width in unit squares
 * h: height in unit squares
 *
 * Returns the exact product as uint64_t. Zero if either edge is zero.
 * Self-contained; no parent wires.
 */
uint64_t
gj_rect_area_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b4883_area(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rect_area_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_rect_area_u")));
