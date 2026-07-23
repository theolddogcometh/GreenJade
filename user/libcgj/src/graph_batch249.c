/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch249: longest increasing subsequence LENGTH
 * for an int64 array via patience sorting (O(n log n)).
 *
 * Surface (unique symbols):
 *   size_t gj_lis_len(const int64_t *a, size_t n);
 *     — Length of a strictly increasing subsequence of a[0..n).
 *       Patience-sorting tails array with binary search: O(n log n).
 *       n <= 1024; NULL or n == 0 (or n > 1024) → 0.
 *   __gj_lis_len  (alias)
 *   __libcgj_batch249_marker = "libcgj-batch249"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. Stack tails[1024].
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch249_marker[] = "libcgj-batch249";

#define B249_MAX_N 1024u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b249_lower_bound — first index i in [0, len) with tails[i] >= x.
 * Returns len if all tails[i] < x. tails is strictly increasing.
 */
static size_t
b249_lower_bound(const int64_t *tails, size_t len, int64_t x)
{
	size_t lo = 0u;
	size_t hi = len;

	while (lo < hi) {
		size_t mid = lo + ((hi - lo) >> 1);

		if (tails[mid] < x) {
			lo = mid + 1u;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lis_len — length of the longest strictly increasing subsequence.
 *
 * Patience sorting: tails[i] = smallest tail value of any increasing
 * subsequence of length i+1 found so far. For each a[j], binary-search
 * the first tails entry >= a[j] and replace it (or append if larger than
 * every tail). Final length of tails is the LIS length.
 *
 * Edge cases:
 *   a == NULL       → 0
 *   n == 0          → 0
 *   n > B249_MAX_N  → 0  (freestanding stack bound)
 * Empty sequence length is 0; a single element yields 1.
 */
size_t
gj_lis_len(const int64_t *a, size_t n)
{
	int64_t tails[B249_MAX_N];
	size_t len = 0u;
	size_t i;

	if (a == NULL || n == 0u || n > (size_t)B249_MAX_N) {
		return 0u;
	}

	for (i = 0u; i < n; i++) {
		int64_t x = a[i];
		size_t pos = b249_lower_bound(tails, len, x);

		if (pos == len) {
			tails[len] = x;
			len++;
		} else {
			/* Strictly increasing: equal values replace, not extend. */
			tails[pos] = x;
		}
	}

	return len;
}

size_t __gj_lis_len(const int64_t *a, size_t n)
    __attribute__((alias("gj_lis_len")));
