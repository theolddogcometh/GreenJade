/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch309: online mean and variance (Welford) for
 * streaming samples (int64 count, double accumulators).
 *
 * Surface (unique symbols):
 *   void gj_welford_init(int64_t *n, double *mean, double *m2);
 *     — Zero sample count and clear mean / M2 accumulators.
 *   void gj_welford_update(int64_t *n, double *mean, double *m2, double x);
 *     — One Welford step: n++, mean, M2 for sample x.
 *   double gj_welford_var(int64_t n, double m2);
 *     — Unbiased sample variance: m2 / (n - 1) when n > 1, else 0.0.
 *   __gj_welford_init / __gj_welford_update / __gj_welford_var  (aliases)
 *   __libcgj_batch309_marker = "libcgj-batch309"
 *
 * Welford (1962) one-pass algorithm — numerically stable online mean and
 * sum of squared deviations from the mean (M2). Population variance is
 * m2/n; this helper exposes the sample (Bessel-corrected) form m2/(n-1).
 *
 * Clean-room freestanding pure C. Double arithmetic is intentional
 * (soft/SSE2 scalar FP under -msse2). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libm. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch309_marker[] = "libcgj-batch309";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_welford_init — reset Welford state to empty.
 *
 * n, mean, m2: caller-owned accumulators. Any NULL argument is ignored
 * for that field (remaining non-NULL fields are still cleared).
 */
void
gj_welford_init(int64_t *n, double *mean, double *m2)
{
	if (n != NULL) {
		*n = 0;
	}
	if (mean != NULL) {
		*mean = 0.0;
	}
	if (m2 != NULL) {
		*m2 = 0.0;
	}
}

void __gj_welford_init(int64_t *n, double *mean, double *m2)
    __attribute__((alias("gj_welford_init")));

/*
 * gj_welford_update — fold one sample into running mean / M2.
 *
 * Classic Welford step:
 *   n     ← n + 1
 *   delta ← x - mean
 *   mean  ← mean + delta / n
 *   M2    ← M2 + delta * (x - mean)   // post-update mean
 *
 * NULL n, mean, or m2 is a no-op (avoids deref; freestanding, no errno).
 * n is treated as a non-negative count; overflow of int64_t is undefined
 * for the caller to avoid (same as any unbounded streaming counter).
 */
void
gj_welford_update(int64_t *n, double *mean, double *m2, double x)
{
	int64_t nNew;
	double dDelta;
	double dMean;
	double dDelta2;

	if (n == NULL || mean == NULL || m2 == NULL) {
		return;
	}

	nNew = *n + (int64_t)1;
	*n = nNew;

	dMean = *mean;
	dDelta = x - dMean;
	/* nNew >= 1 after the increment; cast to double for the divide. */
	dMean = dMean + dDelta / (double)nNew;
	dDelta2 = x - dMean;
	*mean = dMean;
	*m2 = *m2 + dDelta * dDelta2;
}

void __gj_welford_update(int64_t *n, double *mean, double *m2, double x)
    __attribute__((alias("gj_welford_update")));

/*
 * gj_welford_var — unbiased sample variance from Welford M2.
 *
 * Returns m2 / (n - 1) when n > 1. For n <= 1 (empty or single sample)
 * there is no sample variance; returns 0.0.
 */
double
gj_welford_var(int64_t n, double m2)
{
	if (n <= (int64_t)1) {
		return 0.0;
	}
	return m2 / (double)(n - (int64_t)1);
}

double __gj_welford_var(int64_t n, double m2)
    __attribute__((alias("gj_welford_var")));
