/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3068: Q15 sine (exclusive _u).
 *
 * Surface (unique symbols):
 *   int16_t gj_sin_q15_u(int32_t x_q15);
 *     - sin(x) in Q1.15. x is angle in radians, Q15
 *       (1.0 rad == 32768). Result in [-32767, 32767]
 *       (≈ [-1, +1) in Q15; peak clamped to 32767).
 *   int16_t __gj_sin_q15_u  (alias)
 *   __libcgj_batch3068_marker = "libcgj-batch3068"
 *
 * Clean-room Taylor polynomial on [0, π/2] after quadrant reduction.
 * Integer only; no float; no parent wires; no __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3068_marker[] = "libcgj-batch3068";

/* Q15 constants: value * 2^15. */
#define B3068_Q15_ONE    ((int32_t)32768)
#define B3068_PI_Q15     ((int32_t)102944)   /* π   * 32768 ≈ 102943.7 */
#define B3068_HALFPI_Q15 ((int32_t)51472)    /* π/2 * 32768 ≈ 51471.9 */
#define B3068_TWOPI_Q15  ((int32_t)205887)   /* 2π  * 32768 ≈ 205887.4 */

/* ---- freestanding helpers ---------------------------------------------- */

/* Absolute value for int32; INT32_MIN → INT32_MAX (saturate). */
static int32_t
b3068_abs_i32(int32_t i32V)
{
	if (i32V >= 0) {
		return i32V;
	}
	if (i32V == (int32_t)0x80000000) {
		return (int32_t)0x7fffffff;
	}
	return -i32V;
}

/* Clamp to int16 Q15 range [-32767, 32767]. */
static int16_t
b3068_sat_q15(int32_t i32V)
{
	if (i32V > 32767) {
		return (int16_t)32767;
	}
	if (i32V < -32767) {
		return (int16_t)(-32767);
	}
	return (int16_t)i32V;
}

/*
 * Q15 * Q15 → Q15 (with rounding via +0.5 ulp before shift).
 */
static int32_t
b3068_mul_q15(int32_t i32A, int32_t i32B)
{
	int64_t i64P;

	i64P = (int64_t)i32A * (int64_t)i32B;
	i64P += (int64_t)1 << 14; /* round */
	return (int32_t)(i64P >> 15);
}

/*
 * sin on [0, π/2] via Taylor: x - x^3/6 + x^5/120 - x^7/5040.
 * Coefficients in Q15: 1/6, 1/120, 1/5040.
 */
static int32_t
b3068_sin_quad(int32_t i32X)
{
	int32_t i32X2;
	int32_t i32X3;
	int32_t i32X5;
	int32_t i32X7;
	int32_t i32T;
	/* 1/6, 1/120, 1/5040 in Q15 */
	const int32_t i32C3 = 5461;  /* 32768/6    ≈ 5461.33 */
	const int32_t i32C5 = 273;   /* 32768/120  ≈ 273.07  */
	const int32_t i32C7 = 7;     /* 32768/5040 ≈ 6.50 → 7 */

	if (i32X <= 0) {
		return 0;
	}
	if (i32X >= B3068_HALFPI_Q15) {
		return B3068_Q15_ONE - 1; /* 32767 */
	}

	i32X2 = b3068_mul_q15(i32X, i32X);
	i32X3 = b3068_mul_q15(i32X2, i32X);
	i32X5 = b3068_mul_q15(i32X2, i32X3);
	i32X7 = b3068_mul_q15(i32X2, i32X5);

	i32T = i32X;
	i32T -= b3068_mul_q15(i32X3, i32C3);
	i32T += b3068_mul_q15(i32X5, i32C5);
	i32T -= b3068_mul_q15(i32X7, i32C7);
	return i32T;
}

/*
 * Reduce radians Q15 into [0, π/2] with sign, then Taylor.
 *   sin is odd; reduce |x| mod 2π
 *   [π, 2π) → flip sign and subtract π
 *   (π/2, π] → mirror: π - x
 */
static int16_t
b3068_sin(int32_t i32X)
{
	int32_t i32Y;
	int iSign;

	iSign = 1;
	if (i32X < 0) {
		iSign = -1;
		i32X = b3068_abs_i32(i32X);
	}

	if (i32X >= B3068_TWOPI_Q15) {
		i32X = (int32_t)((int64_t)i32X % (int64_t)B3068_TWOPI_Q15);
	}

	if (i32X >= B3068_PI_Q15) {
		i32X -= B3068_PI_Q15;
		iSign = -iSign;
	}

	/* (π/2, π] → [0, π/2); leave π/2 itself so sin(π/2) = 1 */
	if (i32X > B3068_HALFPI_Q15) {
		i32X = B3068_PI_Q15 - i32X;
	}

	i32Y = b3068_sin_quad(i32X);
	if (iSign < 0) {
		i32Y = -i32Y;
	}
	return b3068_sat_q15(i32Y);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sin_q15_u — sine of a Q15 radian angle, result in Q15.
 *
 * Domain: any int32 Q15 radian (reduced mod 2π). Range ≈ [-1, 1] in
 * Q15, saturated to ±32767. No parent wires.
 */
int16_t
gj_sin_q15_u(int32_t i32XQ15)
{
	(void)NULL;
	return b3068_sin(i32XQ15);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_sin_q15_u(int32_t i32XQ15)
    __attribute__((alias("gj_sin_q15_u")));
