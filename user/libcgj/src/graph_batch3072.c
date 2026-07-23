/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3072: Q15 atan2 via integer CORDIC (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_atan2_q15_u(int32_t y, int32_t x);
 *     - Two-argument arctangent of (y, x). Returns angle in Q15-pi
 *       units where 32768 represents pi radians. Range is
 *       (-32768, 32768] covering (-pi, pi]. (0, 0) returns 0.
 *       Matches the angle convention of gj_tan_q15_u (batch3071).
 *   int32_t __gj_atan2_q15_u  (alias)
 *   __libcgj_batch3072_marker = "libcgj-batch3072"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Unique gj_atan2_q15_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3072_marker[] = "libcgj-batch3072";

/* ---- freestanding helpers ---------------------------------------------- */

/* atan(2^-i) in Q15-pi units (32768 == pi). */
static const int32_t b3072_atan_tbl[16] = {
	8192, 4836, 2555, 1297, 648, 324, 162, 81,
	41, 20, 10, 5, 3, 1, 1, 0
};

#define B3072_PI       32768
#define B3072_HALF_PI  16384

/* Absolute value of int32 as uint32 (maps INT32_MIN -> 0x80000000u). */
static uint32_t
b3072_abs_u32(int32_t i32V)
{
	if (i32V < 0) {
		return (uint32_t)(-(i32V + 1)) + 1u;
	}
	return (uint32_t)i32V;
}

/*
 * CORDIC vectoring on non-negative x and free-sign y after quadrant
 * pre-rotation. Inputs scaled into a safe int32 working range.
 */
static int32_t
b3072_cordic_atan2(int32_t i32X, int32_t i32Y)
{
	int32_t i32Z;
	int32_t i32X2;
	int32_t i32Y2;
	int i;

	i32Z = 0;
	for (i = 0; i < 16; i++) {
		if (i32Y >= 0) {
			i32X2 = i32X + (i32Y >> i);
			i32Y2 = i32Y - (i32X >> i);
			i32Z += b3072_atan_tbl[i];
		} else {
			i32X2 = i32X - (i32Y >> i);
			i32Y2 = i32Y + (i32X >> i);
			i32Z -= b3072_atan_tbl[i];
		}
		i32X = i32X2;
		i32Y = i32Y2;
	}
	return i32Z;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atan2_q15_u - atan2(y, x) in Q15-pi units (32768 = pi).
 *
 * y, x: cartesian components (any int32 scale; only the ratio/sign matter)
 *
 * Returns angle in (-pi, pi]. (0,0) -> 0. No parent wires.
 */
int32_t
gj_atan2_q15_u(int32_t i32Y, int32_t i32X)
{
	uint32_t u32Ax;
	uint32_t u32Ay;
	uint32_t u32M;
	int32_t i32Xs;
	int32_t i32Ys;
	int32_t i32Ang;
	int nNegX;
	int nNegY;

	(void)NULL;

	if (i32X == 0 && i32Y == 0) {
		return 0;
	}

	/* Axis shortcuts (exact). */
	if (i32X == 0) {
		return (i32Y > 0) ? B3072_HALF_PI : -B3072_HALF_PI;
	}
	if (i32Y == 0) {
		return (i32X > 0) ? 0 : B3072_PI;
	}

	nNegX = (i32X < 0) ? 1 : 0;
	nNegY = (i32Y < 0) ? 1 : 0;
	u32Ax = b3072_abs_u32(i32X);
	u32Ay = b3072_abs_u32(i32Y);

	/*
	 * Scale so the larger magnitude sits near 2^14. Prevents CORDIC
	 * shift collapse on tiny inputs and overflow on large ones.
	 */
	u32M = (u32Ax > u32Ay) ? u32Ax : u32Ay;
	while (u32M > 0x4000u) {
		u32Ax >>= 1;
		u32Ay >>= 1;
		u32M >>= 1;
	}
	while (u32M > 0u && u32M < 0x1000u) {
		u32Ax <<= 1;
		u32Ay <<= 1;
		u32M <<= 1;
	}

	/* CORDIC needs x >= 0; fold left half-plane via pi - atan. */
	i32Xs = (int32_t)u32Ax;
	i32Ys = (int32_t)u32Ay;
	if (nNegY) {
		i32Ys = -i32Ys;
	}

	if (!nNegX) {
		i32Ang = b3072_cordic_atan2(i32Xs, i32Ys);
	} else {
		/* atan2(y, -x) = pi - atan2(y, x) with sign of y. */
		i32Ang = b3072_cordic_atan2(i32Xs, i32Ys);
		if (i32Ang >= 0) {
			i32Ang = B3072_PI - i32Ang;
		} else {
			i32Ang = -B3072_PI - i32Ang;
		}
	}

	/* Clamp to (-pi, pi]. */
	if (i32Ang > B3072_PI) {
		i32Ang = B3072_PI;
	}
	if (i32Ang <= -B3072_PI) {
		i32Ang = -B3072_PI + 1;
	}
	return i32Ang;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_atan2_q15_u(int32_t i32Y, int32_t i32X)
    __attribute__((alias("gj_atan2_q15_u")));
