/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4889: Chebyshev (Linf) distance uint32 points.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chebyshev_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                           uint32_t y1);
 *     - Returns max(|x1-x0|, |y1-y0|) (chessboard / L-infinity distance).
 *   uint32_t __gj_chebyshev_u  (alias)
 *   __libcgj_batch4889_marker = "libcgj-batch4889"
 *
 * Exclusive continuum CREATE-ONLY (4881-4890: rect_w_u, rect_h_u,
 * rect_area_u, rect_contains_u, rect_intersect_u, point_in_rect_u,
 * rect_empty_u, manhattan_u, chebyshev_u, batch_id_4890).
 * Distinct from gj_chebyshev_u32_u (batch3079) / gj_chebyshev_u64_u
 * (batch3081) — unique gj_chebyshev_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4889_marker[] = "libcgj-batch4889";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4889_absdiff(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

static uint32_t
b4889_cheby(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	uint32_t u32Dx;
	uint32_t u32Dy;

	u32Dx = b4889_absdiff(u32X0, u32X1);
	u32Dy = b4889_absdiff(u32Y0, u32Y1);
	if (u32Dx >= u32Dy) {
		return u32Dx;
	}
	return u32Dy;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chebyshev_u - Linf (chessboard) distance between two uint32 points.
 *
 * x0,y0: first point
 * x1,y1: second point
 *
 * Returns max(|x1-x0|, |y1-y0|). No parent wires.
 */
uint32_t
gj_chebyshev_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	(void)NULL;
	return b4889_cheby(u32X0, u32Y0, u32X1, u32Y1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chebyshev_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
    __attribute__((alias("gj_chebyshev_u")));
