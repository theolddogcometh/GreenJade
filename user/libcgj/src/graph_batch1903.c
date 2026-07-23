/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1903: axis-aligned uint32 rect area.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rect_u32_area(uint32_t x0, uint32_t y0, uint32_t x1,
 *                             uint32_t y1);
 *     — Half-open rect [x0,x1)×[y0,y1). Returns (x1-x0)*(y1-y0) as
 *       uint64_t when non-empty, else 0. Widens before multiply.
 *   uint64_t __gj_rect_u32_area  (alias)
 *   __libcgj_batch1903_marker = "libcgj-batch1903"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Distinct from
 * gj_rect_i32_area (batch1604). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1903_marker[] = "libcgj-batch1903";

/* ---- freestanding helpers ---------------------------------------------- */

/* Area of half-open rect in unit squares; 0 if empty. */
static uint64_t
b1903_area(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1, uint32_t u32Y1)
{
	uint64_t u64W;
	uint64_t u64H;

	if (u32X0 >= u32X1 || u32Y0 >= u32Y1) {
		return 0ull;
	}
	u64W = (uint64_t)u32X1 - (uint64_t)u32X0;
	u64H = (uint64_t)u32Y1 - (uint64_t)u32Y0;
	return u64W * u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rect_u32_area — half-open AABB area in unit squares (uint32).
 *
 * Empty when x0 >= x1 or y0 >= y1 → 0. Product computed in uint64_t.
 */
uint64_t
gj_rect_u32_area(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	(void)NULL;
	return b1903_area(u32X0, u32Y0, u32X1, u32Y1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rect_u32_area(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_rect_u32_area")));
