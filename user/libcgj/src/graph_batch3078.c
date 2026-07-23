/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3078: Manhattan (L1) distance uint64 points
 * (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_manhattan_u64_u(uint64_t x0, uint64_t y0, uint64_t x1,
 *                               uint64_t y1);
 *     - Returns |x1-x0| + |y1-y0|, saturating at UINT64_MAX when the
 *       true sum overflows 64 bits.
 *   uint64_t __gj_manhattan_u64_u  (alias)
 *   __libcgj_batch3078_marker = "libcgj-batch3078"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Distinct from
 * gj_manhattan_u32_u (batch3077) — unique surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3078_marker[] = "libcgj-batch3078";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3078_absdiff_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manhattan_u64_u - L1 distance between two uint64 points.
 *
 * Saturates at UINT64_MAX on overflow. No parent wires.
 */
uint64_t
gj_manhattan_u64_u(uint64_t u64X0, uint64_t u64Y0, uint64_t u64X1,
    uint64_t u64Y1)
{
	uint64_t u64Dx;
	uint64_t u64Dy;
	uint64_t u64Sum;

	(void)NULL;

	u64Dx = b3078_absdiff_u64(u64X0, u64X1);
	u64Dy = b3078_absdiff_u64(u64Y0, u64Y1);
	u64Sum = u64Dx + u64Dy;
	if (u64Sum < u64Dx) {
		return UINT64_MAX;
	}
	return u64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_manhattan_u64_u(uint64_t u64X0, uint64_t u64Y0,
    uint64_t u64X1, uint64_t u64Y1)
    __attribute__((alias("gj_manhattan_u64_u")));
