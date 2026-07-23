/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch498: freestanding integer online mean (floor).
 *
 * Surface (unique symbols):
 *   void gj_online_mean_init(uint64_t *count, int64_t *mean);
 *     — Reset sample count and mean to zero (empty stream).
 *   void gj_online_mean_add(uint64_t *count, int64_t *mean, int64_t x);
 *     — Fold one sample; count++, mean ← floor integer online mean.
 *   void gj_mean_i64_init(int64_t *n, int64_t *mean);
 *     — Same as init with signed count (n cast/clamped as non-negative).
 *   void gj_mean_i64_update(int64_t *n, int64_t *mean, int64_t x);
 *     — Same as add with signed count.
 *   __gj_online_mean_init / __gj_online_mean_add  (aliases)
 *   __gj_mean_i64_init / __gj_mean_i64_update      (aliases)
 *   __libcgj_batch498_marker = "libcgj-batch498"
 *
 * Integer online mean (Welford-style step, floor division):
 *   count ← count + 1
 *   mean  ← mean + floor((x - mean) / count)
 *
 * No separate sum state — only (count, mean). Truncation uses mathematical
 * floor (toward −∞), not C truncating division. Distinct from double
 * Welford in graph_batch309.c.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no libm. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch498_marker[] = "libcgj-batch498";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b498_floor_div — floor(a / b) for b > 0 (mathematical floor, not trunc).
 *
 * C99 signed division truncates toward zero. When a < 0 and a is not
 * divisible by b, subtract one from the quotient to move toward −∞.
 */
static int64_t
b498_floor_div(int64_t a, int64_t b)
{
	int64_t q;
	int64_t r;

	/* b > 0 is guaranteed by callers. */
	q = a / b;
	r = a % b;
	if (r != (int64_t)0 && a < (int64_t)0) {
		q -= (int64_t)1;
	}
	return q;
}

/*
 * b498_online_step — count++, mean += floor((x - mean) / count).
 *
 * When count exceeds INT64_MAX, |x - mean| / count is always < 1 for any
 * representable int64 delta, so the mean is left unchanged after the
 * count increment (avoids a narrowing cast).
 */
static void
b498_online_step(uint64_t *count, int64_t *mean, int64_t x)
{
	uint64_t n;
	int64_t m;
	int64_t delta;

	n = *count + (uint64_t)1;
	*count = n;

	if (n > (uint64_t)INT64_MAX) {
		return;
	}

	m = *mean;
	delta = x - m;
	*mean = m + b498_floor_div(delta, (int64_t)n);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_online_mean_init — reset online mean state to empty.
 *
 * count, mean: caller-owned. NULL arguments are ignored for that field
 * (remaining non-NULL fields are still cleared). Freestanding: no errno.
 */
void
gj_online_mean_init(uint64_t *count, int64_t *mean)
{
	if (count != NULL) {
		*count = (uint64_t)0;
	}
	if (mean != NULL) {
		*mean = (int64_t)0;
	}
}

void __gj_online_mean_init(uint64_t *count, int64_t *mean)
    __attribute__((alias("gj_online_mean_init")));

/*
 * gj_online_mean_add — fold one int64 sample into the running floor mean.
 *
 *   count ← count + 1
 *   mean  ← mean + floor((x - mean) / count)
 *
 * NULL count or mean is a no-op. count overflow of uint64_t is undefined
 * for the caller to avoid (same as any unbounded streaming counter).
 * Signed subtract overflow of (x - mean) is also on the caller.
 */
void
gj_online_mean_add(uint64_t *count, int64_t *mean, int64_t x)
{
	if (count == NULL || mean == NULL) {
		return;
	}
	b498_online_step(count, mean, x);
}

void __gj_online_mean_add(uint64_t *count, int64_t *mean, int64_t x)
    __attribute__((alias("gj_online_mean_add")));

/*
 * gj_mean_i64_init — int64 count variant of gj_online_mean_init.
 *
 * n is treated as a non-negative sample count (same role as uint64 count).
 */
void
gj_mean_i64_init(int64_t *n, int64_t *mean)
{
	if (n != NULL) {
		*n = (int64_t)0;
	}
	if (mean != NULL) {
		*mean = (int64_t)0;
	}
}

void __gj_mean_i64_init(int64_t *n, int64_t *mean)
    __attribute__((alias("gj_mean_i64_init")));

/*
 * gj_mean_i64_update — int64 count variant of gj_online_mean_add.
 *
 * Negative *n is treated as empty (reset path via nNew = 1 after
 * saturating at zero before increment is avoided — instead we force
 * a positive step from max(n, 0) + 1 so a corrupt negative count does
 * not feed a non-positive divisor into floor_div).
 */
void
gj_mean_i64_update(int64_t *n, int64_t *mean, int64_t x)
{
	int64_t n0;
	uint64_t count;
	int64_t m;

	if (n == NULL || mean == NULL) {
		return;
	}

	n0 = *n;
	if (n0 < (int64_t)0) {
		n0 = (int64_t)0;
	}

	count = (uint64_t)n0;
	m = *mean;
	b498_online_step(&count, &m, x);

	/* count is at most INT64_MAX + 1 after one step from a non-neg n0. */
	if (count > (uint64_t)INT64_MAX) {
		*n = INT64_MAX;
	} else {
		*n = (int64_t)count;
	}
	*mean = m;
}

void __gj_mean_i64_update(int64_t *n, int64_t *mean, int64_t x)
    __attribute__((alias("gj_mean_i64_update")));
