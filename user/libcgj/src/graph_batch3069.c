/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3069: Q15 cosine (exclusive _u).
 *
 * Surface (unique symbols):
 *   int16_t gj_cos_q15_u(int32_t x_q15);
 *     - cos(x) in Q1.15. x is angle in radians, Q15
 *       (1.0 rad == 32768). Result in [-32767, 32767]
 *       (≈ [-1, +1) in Q15; peak clamped to 32767).
 *   int16_t __gj_cos_q15_u  (alias)
 *   __libcgj_batch3069_marker = "libcgj-batch3069"
 *
 * Implemented as sin(x + π/2) with a self-contained Taylor path —
 * does not call gj_sin_q15_u (no parent wires).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3069_marker[] = "libcgj-batch3069";

/* Q15 constants: value * 2^15. */
#define B3069_Q15_ONE    ((int32_t)32768)
#define B3069_PI_Q15     ((int32_t)102944)
#define B3069_HALFPI_Q15 ((int32_t)51472)
#define B3069_TWOPI_Q15  ((int32_t)205887)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b3069_abs_i32(int32_t i32V)
{
	if (i32V >= 0) {
		return i32V;
	}
	if (i32V == (int32_t)0x80000000) {
		return (int32_t)0x7fffffff;
	}
	return -i32V;
}

static int16_t
b3069_sat_q15(int32_t i32V)
{
	if (i32V > 32767) {
		return (int16_t)32767;
	}
	if (i32V < -32767) {
		return (int16_t)(-32767);
	}
	return (int16_t)i32V;
}

static int32_t
b3069_mul_q15(int32_t i32A, int32_t i32B)
{
	int64_t i64P;

	i64P = (int64_t)i32A * (int64_t)i32B;
	i64P += (int64_t)1 << 14;
	return (int32_t)(i64P >> 15);
}

/*
 * sin on [0, π/2] via Taylor: x - x^3/6 + x^5/120 - x^7/5040.
 * (Self-contained copy; cos = sin(x+π/2), no parent wires.)
 */
static int32_t
b3069_sin_quad(int32_t i32X)
{
	int32_t i32X2;
	int32_t i32X3;
	int32_t i32X5;
	int32_t i32X7;
	int32_t i32T;
	const int32_t i32C3 = 5461;
	const int32_t i32C5 = 273;
	const int32_t i32C7 = 7;

	if (i32X <= 0) {
		return 0;
	}
	if (i32X >= B3069_HALFPI_Q15) {
		return B3069_Q15_ONE - 1; /* 32767 */
	}

	i32X2 = b3069_mul_q15(i32X, i32X);
	i32X3 = b3069_mul_q15(i32X2, i32X);
	i32X5 = b3069_mul_q15(i32X2, i32X3);
	i32X7 = b3069_mul_q15(i32X2, i32X5);

	i32T = i32X;
	i32T -= b3069_mul_q15(i32X3, i32C3);
	i32T += b3069_mul_q15(i32X5, i32C5);
	i32T -= b3069_mul_q15(i32X7, i32C7);
	return i32T;
}

static int16_t
b3069_sin(int32_t i32X)
{
	int32_t i32Y;
	int iSign;

	iSign = 1;
	if (i32X < 0) {
		iSign = -1;
		i32X = b3069_abs_i32(i32X);
	}

	if (i32X >= B3069_TWOPI_Q15) {
		i32X = (int32_t)((int64_t)i32X % (int64_t)B3069_TWOPI_Q15);
	}

	if (i32X >= B3069_PI_Q15) {
		i32X -= B3069_PI_Q15;
		iSign = -iSign;
	}

	if (i32X > B3069_HALFPI_Q15) {
		i32X = B3069_PI_Q15 - i32X;
	}

	i32Y = b3069_sin_quad(i32X);
	if (iSign < 0) {
		i32Y = -i32Y;
	}
	return b3069_sat_q15(i32Y);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cos_q15_u — cosine of a Q15 radian angle, result in Q15.
 *
 * Computed as sin(x + π/2) with a local sine implementation (no call to
 * gj_sin_q15_u). Domain: any int32 Q15 radian. Range ≈ [-1, 1] in Q15,
 * saturated to ±32767. No parent wires.
 */
int16_t
gj_cos_q15_u(int32_t i32XQ15)
{
	int64_t i64T;

	(void)NULL;
	/* x + π/2 may exceed int32 near INT32_MAX; reduce via int64. */
	i64T = (int64_t)i32XQ15 + (int64_t)B3069_HALFPI_Q15;
	/* Bring into a range safe for int32 before sin reduction. */
	if (i64T > (int64_t)B3069_TWOPI_Q15 || i64T < -(int64_t)B3069_TWOPI_Q15) {
		i64T %= (int64_t)B3069_TWOPI_Q15;
	}
	return b3069_sin((int32_t)i64T);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_cos_q15_u(int32_t i32XQ15)
    __attribute__((alias("gj_cos_q15_u")));
