/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3071: Q15 tangent via integer CORDIC (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_tan_q15_u(int32_t angle_q15);
 *     - Tangent of angle_q15. Angle unit: 32768 represents pi radians
 *       (binary-angle / Q15-pi mapping). Valid open domain is
 *       (-16384, 16384) i.e. (-pi/2, pi/2). Output is Q15
 *       (32768 represents 1.0). Near the vertical asymptotes the
 *       result saturates to INT32_MAX / INT32_MIN. Outside the open
 *       domain the input is folded into (-pi/2, pi/2) first.
 *   int32_t __gj_tan_q15_u  (alias)
 *   __libcgj_batch3071_marker = "libcgj-batch3071"
 *
 * Exclusive continuum CREATE-ONLY (3071-3080). Unique gj_tan_q15_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No float.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3071_marker[] = "libcgj-batch3071";

/* ---- freestanding helpers ---------------------------------------------- */

/* atan(2^-i) in Q15-pi units (32768 == pi). 16 iterations. */
static const int32_t b3071_atan_tbl[16] = {
	8192, 4836, 2555, 1297, 648, 324, 162, 81,
	41, 20, 10, 5, 3, 1, 1, 0
};

/* pi and pi/2 in Q15-pi units. */
#define B3071_PI      32768
#define B3071_HALF_PI 16384

/*
 * Fold angle into (-pi/2, pi/2) for tan periodicity / asymptotes.
 * Returns 0 on success; 1 if the angle lands on an odd multiple of
 * pi/2 (tan undefined) with *pSign set to +1 or -1 for the side.
 */
static int32_t
b3071_fold_half_pi(int32_t i32A, int *pPole)
{
	int32_t i32X;
	int32_t i32Q;

	*pPole = 0;

	/* Reduce mod 2*pi into [-pi, pi). 2*pi == 65536 wraps in int32. */
	i32X = (int32_t)((int32_t)i32A % (int32_t)(2 * B3071_PI));
	/* C % can be negative; normalize to [0, 2pi). */
	if (i32X < 0) {
		i32X += (int32_t)(2 * B3071_PI);
	}
	/* Map to [-pi, pi). */
	if (i32X >= B3071_PI) {
		i32X -= (int32_t)(2 * B3071_PI);
	}

	/*
	 * Further fold into (-pi/2, pi/2):
	 * tan(x) = tan(x - pi) with sign already in [-pi,pi);
	 * for x in (pi/2, pi] use tan(x - pi) which is negative small;
	 * actually tan has period pi: tan(x+pi) = tan(x).
	 */
	if (i32X > B3071_HALF_PI) {
		i32X -= B3071_PI;
	} else if (i32X < -B3071_HALF_PI) {
		i32X += B3071_PI;
	}

	/* Detect exact poles at ±pi/2 after fold (should not remain). */
	if (i32X >= B3071_HALF_PI) {
		*pPole = 1;
		return B3071_HALF_PI - 1;
	}
	if (i32X <= -B3071_HALF_PI) {
		*pPole = -1;
		return -(B3071_HALF_PI - 1);
	}

	/* Keep a guard band so CORDIC division stays finite. */
	i32Q = B3071_HALF_PI - 8;
	if (i32X >= i32Q) {
		return i32Q;
	}
	if (i32X <= -i32Q) {
		return -i32Q;
	}
	return i32X;
}

/*
 * CORDIC rotation: start at (1, 0) scaled, rotate by z; tan = y/x.
 * Gain cancels in the ratio. Uses 14-bit base scale.
 */
static int32_t
b3071_cordic_tan(int32_t i32Z)
{
	int32_t i32X;
	int32_t i32Y;
	int32_t i32X2;
	int32_t i32Y2;
	int32_t i32Z2;
	int32_t i32T;
	int i;
	int64_t i64Num;

	i32X = (int32_t)1 << 14;
	i32Y = 0;
	i32Z2 = i32Z;

	for (i = 0; i < 16; i++) {
		if (i32Z2 >= 0) {
			i32X2 = i32X - (i32Y >> i);
			i32Y2 = i32Y + (i32X >> i);
			i32Z2 -= b3071_atan_tbl[i];
		} else {
			i32X2 = i32X + (i32Y >> i);
			i32Y2 = i32Y - (i32X >> i);
			i32Z2 += b3071_atan_tbl[i];
		}
		i32X = i32X2;
		i32Y = i32Y2;
	}

	if (i32X == 0) {
		return (i32Y >= 0) ? INT32_MAX : INT32_MIN;
	}

	/* Q15 quotient: (y << 15) / x */
	i64Num = ((int64_t)i32Y) << 15;
	i32T = (int32_t)(i64Num / (int64_t)i32X);
	return i32T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tan_q15_u - Q15 tangent (32768 = 1.0; angle 32768 = pi).
 *
 * angle_q15: angle in Q15-pi units
 *
 * Returns tan(angle) in Q15. Saturates near odd multiples of pi/2.
 * No parent wires.
 */
int32_t
gj_tan_q15_u(int32_t i32AngleQ15)
{
	int nPole;
	int32_t i32A;
	int32_t i32T;
	int nNeg;

	(void)NULL;
	i32A = b3071_fold_half_pi(i32AngleQ15, &nPole);
	if (nPole > 0) {
		return INT32_MAX;
	}
	if (nPole < 0) {
		return INT32_MIN;
	}
	/* Exact zeros; also kills CORDIC z==0 half-step bias. */
	if (i32A == 0) {
		return 0;
	}
	/*
	 * CORDIC with if (z >= 0) has a one-sided residual when z hits 0
	 * mid-loop. Run on |angle| and restore sign for a symmetric result.
	 */
	nNeg = 0;
	if (i32A < 0) {
		nNeg = 1;
		i32A = -i32A;
	}
	i32T = b3071_cordic_tan(i32A);
	if (nNeg) {
		if (i32T == INT32_MIN) {
			return INT32_MAX; /* symmetric clamp */
		}
		i32T = -i32T;
	}
	return i32T;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_tan_q15_u(int32_t i32AngleQ15)
    __attribute__((alias("gj_tan_q15_u")));
