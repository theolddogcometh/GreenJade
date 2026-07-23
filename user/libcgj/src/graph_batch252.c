/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch252: naive complex DFT for small power-of-2
 * lengths (double real/imag, freestanding — no libm).
 *
 * Surface (unique symbols):
 *   int gj_dft_naive(const double *in_re, const double *in_im,
 *                    double *out_re, double *out_im, size_t n);
 *     — Forward DFT X[k] = sum_j x[j] * exp(-2π i k j / n) for k in 0..n-1.
 *       Real/imag parts stored separately. n must be a power of two in
 *       [1, 64]. Returns 0 on success, -1 on bad args (NULL pointer,
 *       n out of range, or n not a power of two). No scaling (1/n).
 *   __gj_dft_naive  (alias)
 *   __libcgj_batch252_marker = "libcgj-batch252"
 *
 * Clean-room freestanding pure C. Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. Double math uses SSE2; sin/cos via manual Taylor
 * (no libm) for freestanding SO safety. No malloc, no errno. No
 * third-party FFT source copied.
 */

#include <stddef.h>

const char __libcgj_batch252_marker[] = "libcgj-batch252";

/* ---- constants --------------------------------------------------------- */

/* pi and 2*pi as double literals (enough for n <= 64 DFT angles). */
static const double b252_pi = 3.14159265358979323846264338327950288;
static const double b252_two_pi = 6.28318530717958647692528676655900576;

#define B252_MAX_N 64u

/* ---- freestanding helpers ---------------------------------------------- */

/* Return 1 if n is a power of two (n >= 1), else 0. */
static int
b252_is_pow2(size_t n)
{
	return (n != 0u) && ((n & (n - 1u)) == 0u);
}

/*
 * b252_mod_two_pi — reduce angle into approximately [-pi, pi].
 *
 * Uses truncating cast (toward zero) of x/(2π); angles for n<=64 stay well
 * within the exact-integer double range, so no libm floor is required.
 */
static double
b252_mod_two_pi(double x)
{
	double rev;
	long long n;

	rev = x / b252_two_pi;
	n = (long long)rev;
	x -= (double)n * b252_two_pi;
	if (x > b252_pi) {
		x -= b252_two_pi;
	} else if (x < -b252_pi) {
		x += b252_two_pi;
	}
	return x;
}

/*
 * b252_sin_series / b252_cos_series — Taylor polynomials about 0.
 *
 * Caller must pass |x| <= pi/2. Enough odd/even powers for double accuracy
 * on that interval (remainder past x^19 / 19! is negligible for DFT use).
 */
static double
b252_sin_series(double x)
{
	double x2 = x * x;
	double t = x;
	double s = t;

	/* x^3/3! */
	t *= x2 / (2.0 * 3.0);
	s -= t;
	/* x^5/5! */
	t *= x2 / (4.0 * 5.0);
	s += t;
	/* x^7/7! */
	t *= x2 / (6.0 * 7.0);
	s -= t;
	/* x^9/9! */
	t *= x2 / (8.0 * 9.0);
	s += t;
	/* x^11/11! */
	t *= x2 / (10.0 * 11.0);
	s -= t;
	/* x^13/13! */
	t *= x2 / (12.0 * 13.0);
	s += t;
	/* x^15/15! */
	t *= x2 / (14.0 * 15.0);
	s -= t;
	/* x^17/17! */
	t *= x2 / (16.0 * 17.0);
	s += t;
	/* x^19/19! */
	t *= x2 / (18.0 * 19.0);
	s -= t;
	return s;
}

static double
b252_cos_series(double x)
{
	double x2 = x * x;
	double t = 1.0;
	double s = t;

	/* x^2/2! */
	t *= x2 / (1.0 * 2.0);
	s -= t;
	/* x^4/4! */
	t *= x2 / (3.0 * 4.0);
	s += t;
	/* x^6/6! */
	t *= x2 / (5.0 * 6.0);
	s -= t;
	/* x^8/8! */
	t *= x2 / (7.0 * 8.0);
	s += t;
	/* x^10/10! */
	t *= x2 / (9.0 * 10.0);
	s -= t;
	/* x^12/12! */
	t *= x2 / (11.0 * 12.0);
	s += t;
	/* x^14/14! */
	t *= x2 / (13.0 * 14.0);
	s -= t;
	/* x^16/16! */
	t *= x2 / (15.0 * 16.0);
	s += t;
	/* x^18/18! */
	t *= x2 / (17.0 * 18.0);
	s -= t;
	return s;
}

/*
 * b252_sincos — freestanding sin/cos via range reduction + Taylor.
 *
 * Folds into [-pi/2, pi/2] with sign/cofunction identities, then evaluates
 * independent sin and cos series (no shared state / no libm).
 */
static void
b252_sincos(double theta, double *pSin, double *pCos)
{
	double x;
	double s;
	double c;
	int fNegCos = 0;

	x = b252_mod_two_pi(theta);

	/* Map to [-pi/2, pi/2]: sin(pi - x) = sin(x), cos(pi - x) = -cos(x).
	 * Same for the negative half: cos(-pi - x) = -cos(x) when folded. */
	if (x > b252_pi * 0.5) {
		x = b252_pi - x;
		fNegCos = 1;
	} else if (x < -b252_pi * 0.5) {
		x = -b252_pi - x;
		fNegCos = 1;
	}

	s = b252_sin_series(x);
	c = b252_cos_series(x);

	if (fNegCos) {
		c = -c;
	}

	*pSin = s;
	*pCos = c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dft_naive — O(n^2) complex DFT, separate real/imag buffers.
 *
 * X[k] = sum_{j=0}^{n-1} x[j] * (cos(θ) - i sin(θ)), θ = 2π k j / n
 *      = sum ( xr cos + xi sin )  + i sum ( xi cos - xr sin )
 *
 * in_im may represent a pure-real signal by passing zeros (pointer still
 * required). out_* must not overlap in_* in a way that races mid-sum;
 * distinct buffers are assumed.
 */
int
gj_dft_naive(const double *in_re, const double *in_im, double *out_re,
    double *out_im, size_t n)
{
	size_t k;
	size_t j;

	if (in_re == NULL || in_im == NULL || out_re == NULL || out_im == NULL) {
		return -1;
	}
	if (n < 1u || n > B252_MAX_N || !b252_is_pow2(n)) {
		return -1;
	}

	for (k = 0u; k < n; k++) {
		double sumRe = 0.0;
		double sumIm = 0.0;

		for (j = 0u; j < n; j++) {
			double theta;
			double s;
			double c;
			double xr;
			double xi;

			/* θ = 2π k j / n  (forward DFT uses e^{-iθ}) */
			theta = b252_two_pi * (double)k * (double)j / (double)n;
			b252_sincos(theta, &s, &c);

			xr = in_re[j];
			xi = in_im[j];
			sumRe += xr * c + xi * s;
			sumIm += xi * c - xr * s;
		}

		out_re[k] = sumRe;
		out_im[k] = sumIm;
	}

	return 0;
}

int __gj_dft_naive(const double *in_re, const double *in_im, double *out_re,
    double *out_im, size_t n) __attribute__((alias("gj_dft_naive")));
