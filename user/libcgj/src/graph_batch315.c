/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch315: sparse table for range minimum on int64.
 *
 * Surface (unique symbols):
 *   int gj_st_build(const int64_t *a, size_t n, int64_t *st, size_t st_cap,
 *                   size_t *logn);
 *     — Build a min sparse table over a[0..n) into st, and floor-log2 table
 *       into logn. Layout: st[k * n + i] = min of a[i .. i+2^k - 1] (the
 *       2^k-length window starting at i). Requires st_cap >= n * ceil_log
 *       where ceil_log = floor(log2(n)) + 1 (levels k = 0 .. floor(log2(n))).
 *       logn[i] = floor(log2(i)) for i in 1..n (logn[0] set to 0). Caller
 *       supplies logn with at least n+1 size_t slots. n must be in 1..256.
 *       Returns 0 on success, -1 on invalid args or insufficient st_cap.
 *   int64_t gj_st_rmq(const int64_t *st, size_t n, const size_t *logn,
 *                     size_t l, size_t r);
 *     — Minimum of a[l..r] inclusive after a prior gj_st_build with the
 *       same n. Empty / invalid ranges return INT64_MAX (min identity).
 *       r is clamped to n-1 when r >= n.
 *   __gj_st_build / __gj_st_rmq  (aliases)
 *   __libcgj_batch315_marker = "libcgj-batch315"
 *
 * Distinct from batch221 (segment tree RMQ): O(1) query, O(n log n) space,
 * static (no point updates).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch315_marker[] = "libcgj-batch315";

/* Design limit: tables sized for small static arrays (n <= 256). */
#define B315_N_MAX 256u

/* Min identity: larger than any int64_t value. */
#define B315_INF ((int64_t)0x7fffffffffffffffLL)

/* ---- helpers ----------------------------------------------------------- */

static int64_t
b315_min2(int64_t i64A, int64_t i64B)
{
	return (i64A < i64B) ? i64A : i64B;
}

/*
 * Number of sparse-table levels for length n (>= 1):
 *   k runs from 0 to floor(log2(n)), inclusive → floor(log2(n))+1 rows.
 * st_cap must be at least n * that value.
 */
static size_t
b315_ceil_log(size_t n)
{
	size_t cLevels = 1u;
	size_t uPow = 1u;

	/* Count how many times we can double while staying < n. */
	while ((uPow << 1) <= n) {
		uPow <<= 1;
		cLevels++;
	}
	return cLevels;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_st_build — fill st and logn for min sparse-table RMQ over a[0..n).
 *
 * a:      source array of n int64_t values.
 * n:      length in 1..B315_N_MAX.
 * st:     flat table buffer; st[k*n+i] = min of length-2^k window at i.
 * st_cap: capacity of st in elements; need >= n * ceil_log(n).
 * logn:   floor-log2 table; logn[i] = floor(log2(i)) for i in 1..n.
 *
 * Returns 0 on success, -1 if any pointer is NULL, n is 0 or > 256, or
 * st_cap is too small. Does not set errno (freestanding).
 */
int
gj_st_build(const int64_t *a, size_t n, int64_t *st, size_t st_cap,
            size_t *logn)
{
	size_t cLevels;
	size_t i;
	size_t k;
	size_t uHalf;

	if (a == NULL || st == NULL || logn == NULL) {
		return -1;
	}
	if (n == 0u || n > B315_N_MAX) {
		return -1;
	}

	cLevels = b315_ceil_log(n);
	if (st_cap < n * cLevels) {
		return -1;
	}

	/* logn[i] = floor(log2(i)); logn[0] unused but set for determinism. */
	logn[0] = 0u;
	if (n >= 1u) {
		logn[1] = 0u;
	}
	for (i = 2u; i <= n; i++) {
		logn[i] = logn[i >> 1] + 1u;
	}

	/* Level 0: windows of length 1 are the elements themselves. */
	for (i = 0u; i < n; i++) {
		st[i] = a[i];
	}

	/*
	 * st[k][i] = min(st[k-1][i], st[k-1][i + 2^(k-1)])
	 * for every start i where the full 2^k window fits in [0, n).
	 */
	for (k = 1u; k < cLevels; k++) {
		uHalf = (size_t)1u << (k - 1u);
		for (i = 0u; i + (uHalf << 1) <= n; i++) {
			st[k * n + i] = b315_min2(st[(k - 1u) * n + i],
			                          st[(k - 1u) * n + i + uHalf]);
		}
	}

	return 0;
}

int __gj_st_build(const int64_t *a, size_t n, int64_t *st, size_t st_cap,
                  size_t *logn)
    __attribute__((alias("gj_st_build")));

/*
 * gj_st_rmq — range minimum on inclusive indices [l, r].
 *
 * st/n/logn must match a prior successful gj_st_build. Uses two
 * overlapping power-of-two windows of length 2^k where
 * k = floor(log2(r - l + 1)).
 *
 * Returns B315_INF (INT64_MAX) when the effective query is empty or
 * arguments are invalid (NULL st/logn, n == 0, l >= n, or l > r).
 */
int64_t
gj_st_rmq(const int64_t *st, size_t n, const size_t *logn, size_t l, size_t r)
{
	size_t cLen;
	size_t k;
	size_t uSpan;
	size_t iRight;

	if (st == NULL || logn == NULL || n == 0u || l >= n || l > r) {
		return B315_INF;
	}

	/* Clamp r to last valid index. */
	if (r >= n) {
		r = n - 1u;
	}

	cLen = r - l + 1u;
	k = logn[cLen];
	uSpan = (size_t)1u << k;
	iRight = r - uSpan + 1u;

	return b315_min2(st[k * n + l], st[k * n + iRight]);
}

int64_t __gj_st_rmq(const int64_t *st, size_t n, const size_t *logn, size_t l,
                    size_t r)
    __attribute__((alias("gj_st_rmq")));
