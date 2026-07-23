/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3079: Chebyshev (Linf) distance uint32 points
 * (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_chebyshev_u32_u(uint32_t x0, uint32_t y0, uint32_t x1,
 *                               uint32_t y1);
 *     - Returns max(|x1-x0|, |y1-y0|). Exact for full uint32 domain.
 *   uint32_t __gj_chebyshev_u32_u  (alias)
 *   __libcgj_batch3079_marker = "libcgj-batch3079"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Unique
 * gj_chebyshev_u32_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3079_marker[] = "libcgj-batch3079";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3079_absdiff_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chebyshev_u32_u - Linf (chessboard) distance between two uint32
 * points.
 *
 * No parent wires.
 */
uint32_t
gj_chebyshev_u32_u(uint32_t u32X0, uint32_t u32Y0, uint32_t u32X1,
    uint32_t u32Y1)
{
	uint32_t u32Dx;
	uint32_t u32Dy;

	(void)NULL;

	u32Dx = b3079_absdiff_u32(u32X0, u32X1);
	u32Dy = b3079_absdiff_u32(u32Y0, u32Y1);
	return (u32Dx > u32Dy) ? u32Dx : u32Dy;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chebyshev_u32_u(uint32_t u32X0, uint32_t u32Y0,
    uint32_t u32X1, uint32_t u32Y1)
    __attribute__((alias("gj_chebyshev_u32_u")));
