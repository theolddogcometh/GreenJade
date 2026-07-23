/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3076: squared Euclidean distance (uint64) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_dist2_u64_u(uint64_t x0, uint64_t y0, uint64_t x1,
 *                           uint64_t y1);
 *     - Returns dx*dx + dy*dy where dx = |x1-x0|, dy = |y1-y0|,
 *       saturating at UINT64_MAX when the true sum of squares does not
 *       fit in 64 bits (portable, no __int128).
 *   uint64_t __gj_dist2_u64_u  (alias)
 *   __libcgj_batch3076_marker = "libcgj-batch3076"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Distinct from
 * gj_dist2_u32_u (batch3075) / gj_dist2_i32 (batch1609) — unique
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3076_marker[] = "libcgj-batch3076";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3076_absdiff_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64B - u64A;
}

/* High 64 bits of a*b. */
static uint64_t
b3076_mul_hi(uint64_t u64A, uint64_t u64B)
{
	uint64_t al, ah, bl, bh;
	uint64_t p00, p01, p10, p11;
	uint64_t mid, hi;

	al = u64A & 0xffffffffull;
	ah = u64A >> 32;
	bl = u64B & 0xffffffffull;
	bh = u64B >> 32;

	p00 = al * bl;
	p01 = al * bh;
	p10 = ah * bl;
	p11 = ah * bh;

	mid = (p00 >> 32) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
	hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
	return hi;
}

/*
 * Saturating square-and-add: return min(dx^2 + dy^2, UINT64_MAX).
 * If either square has a non-zero high half, the true value exceeds
 * 64 bits and we saturate.
 */
static uint64_t
b3076_dist2_sat(uint64_t u64Dx, uint64_t u64Dy)
{
	uint64_t u64XxLo;
	uint64_t u64XxHi;
	uint64_t u64YyLo;
	uint64_t u64YyHi;
	uint64_t u64Sum;

	u64XxLo = u64Dx * u64Dx;
	u64XxHi = b3076_mul_hi(u64Dx, u64Dx);
	if (u64XxHi != 0ull) {
		return UINT64_MAX;
	}

	u64YyLo = u64Dy * u64Dy;
	u64YyHi = b3076_mul_hi(u64Dy, u64Dy);
	if (u64YyHi != 0ull) {
		return UINT64_MAX;
	}

	u64Sum = u64XxLo + u64YyLo;
	if (u64Sum < u64XxLo) {
		return UINT64_MAX;
	}
	return u64Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dist2_u64_u - squared Euclidean distance between two uint64 points.
 *
 * Saturates at UINT64_MAX on overflow. No parent wires.
 */
uint64_t
gj_dist2_u64_u(uint64_t u64X0, uint64_t u64Y0, uint64_t u64X1,
    uint64_t u64Y1)
{
	uint64_t u64Dx;
	uint64_t u64Dy;

	(void)NULL;

	u64Dx = b3076_absdiff_u64(u64X0, u64X1);
	u64Dy = b3076_absdiff_u64(u64Y0, u64Y1);
	return b3076_dist2_sat(u64Dx, u64Dy);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dist2_u64_u(uint64_t u64X0, uint64_t u64Y0, uint64_t u64X1,
    uint64_t u64Y1)
    __attribute__((alias("gj_dist2_u64_u")));
