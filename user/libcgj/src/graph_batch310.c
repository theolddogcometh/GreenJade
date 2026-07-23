/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch310: select k-th smallest int32 (quickselect).
 *
 * Surface (unique symbols):
 *   int gj_select_k_i32(int32_t *a, size_t n, size_t k, int32_t *out);
 *     — Place the element of rank k (0-based, k in [0, n)) of a[0..n)
 *       into *out via in-place partial selection (may reorder a). After
 *       success, a[k] == *out is the (k+1)-th smallest; elements left of
 *       k are <= a[k] and right are >= a[k]. Returns 0 on success, -1 if
 *       a or out is NULL, n is 0, or k >= n.
 *   __gj_select_k_i32  (alias)
 *   __libcgj_batch310_marker = "libcgj-batch310"
 *
 * Intended for percentile / order-statistic queries (e.g. k = floor(p*n)
 * for a p-quantile) without a full sort.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Average O(n) quickselect with median-of-3 pivot; insertion select for
 * tiny spans. No third-party source copied. Distinct from batch211
 * (int64 median) and batch120 (constant-time select).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch310_marker[] = "libcgj-batch310";

/* Insertion-select threshold (elements in the active span). */
#define B310_INS_MAX 16u

/* ---- helpers ----------------------------------------------------------- */

static void
b310_swap(int32_t *pA, int32_t *pB)
{
	int32_t t = *pA;

	*pA = *pB;
	*pB = t;
}

/*
 * Insertion-sort the half-open span a[lo..hi] (inclusive hi), then the
 * caller can read a[k] for any k in [lo, hi]. Used only for small spans.
 */
static void
b310_insert_sort(int32_t *a, size_t lo, size_t hi)
{
	size_t i;

	for (i = lo + 1u; i <= hi; i++) {
		int32_t v = a[i];
		size_t j = i;

		while (j > lo && a[j - 1u] > v) {
			a[j] = a[j - 1u];
			j--;
		}
		a[j] = v;
	}
}

/* Median-of-three indices; parks the chosen pivot at hi. */
static void
b310_median3_pivot(int32_t *a, size_t lo, size_t hi)
{
	size_t mid = lo + ((hi - lo) >> 1);

	/* Order a[lo], a[mid], a[hi] so a[mid] is the median of the three. */
	if (a[lo] > a[mid]) {
		b310_swap(&a[lo], &a[mid]);
	}
	if (a[lo] > a[hi]) {
		b310_swap(&a[lo], &a[hi]);
	}
	if (a[mid] > a[hi]) {
		b310_swap(&a[mid], &a[hi]);
	}
	/* a[lo] <= a[mid] <= a[hi]; park median (mid) at hi for partition. */
	b310_swap(&a[mid], &a[hi]);
}

/*
 * Lomuto partition on a[lo..hi] with pivot already at a[hi].
 * Returns final pivot index p such that a[lo..p-1] <= a[p] <= a[p+1..hi].
 */
static size_t
b310_partition(int32_t *a, size_t lo, size_t hi)
{
	int32_t pivot = a[hi];
	size_t i = lo;
	size_t j;

	for (j = lo; j < hi; j++) {
		if (a[j] < pivot) {
			b310_swap(&a[i], &a[j]);
			i++;
		}
	}
	b310_swap(&a[i], &a[hi]);
	return i;
}

/*
 * Place the element of rank k (0-based, k in [0, n)) into a[k] by
 * partial selection. Reorders a. After return, a[k] is the (k+1)-th
 * smallest; elements left of k are <= a[k] and right are >= a[k].
 */
static void
b310_select(int32_t *a, size_t n, size_t k)
{
	size_t lo = 0u;
	size_t hi = n - 1u;

	for (;;) {
		size_t span = hi - lo + 1u;
		size_t p;

		if (span <= B310_INS_MAX) {
			b310_insert_sort(a, lo, hi);
			return;
		}

		b310_median3_pivot(a, lo, hi);
		p = b310_partition(a, lo, hi);

		if (p == k) {
			return;
		}
		if (k < p) {
			if (p == 0u) {
				return;
			}
			hi = p - 1u;
		} else {
			lo = p + 1u;
		}
	}
}

/* ---- gj_select_k_i32 --------------------------------------------------- */

/*
 * gj_select_k_i32 — k-th smallest of a[0..n) via in-place quickselect.
 *
 * a:   mutable array of n int32_t (reordered on success).
 * n:   length; must be > 0.
 * k:   0-based rank in [0, n); k == 0 is minimum, k == n-1 is maximum.
 * out: on success, *out = a[k] after partial selection.
 *
 * Returns 0 on success, -1 on invalid arguments (NULL a/out, n == 0,
 * or k >= n). Does not set errno (freestanding).
 */
int
gj_select_k_i32(int32_t *a, size_t n, size_t k, int32_t *out)
{
	if (a == NULL || out == NULL || n == 0u || k >= n) {
		return -1;
	}

	if (n == 1u) {
		*out = a[0];
		return 0;
	}

	b310_select(a, n, k);
	*out = a[k];
	return 0;
}

int __gj_select_k_i32(int32_t *a, size_t n, size_t k, int32_t *out)
    __attribute__((alias("gj_select_k_i32")));
