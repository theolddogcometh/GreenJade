/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch211: in-place int64 median via partial select.
 *
 * Surface (unique symbols):
 *   int64_t gj_median_i64(int64_t *a, size_t n);
 *     — return the lower median of a[0..n): after partial selection the
 *       element at index k = (n-1)/2 is the median (middle for odd n,
 *       lower of the two middles for even n). May reorder a. Returns 0
 *       if a is NULL or n is 0.
 *   __gj_median_i64  (alias)
 *   __libcgj_batch211_marker = "libcgj-batch211"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. Average O(n) quickselect
 * with median-of-3 pivot; insertion select for tiny spans.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch211_marker[] = "libcgj-batch211";

/* Insertion-select threshold (elements in the active span). */
#define B211_INS_MAX 16u

/* ---- helpers ----------------------------------------------------------- */

static void
b211_swap(int64_t *pA, int64_t *pB)
{
	int64_t t = *pA;

	*pA = *pB;
	*pB = t;
}

/*
 * Insertion-sort the half-open span a[lo..hi] (inclusive hi), then the
 * caller can read a[k] for any k in [lo, hi]. Used only for small spans.
 */
static void
b211_insert_sort(int64_t *a, size_t lo, size_t hi)
{
	size_t i;

	for (i = lo + 1u; i <= hi; i++) {
		int64_t v = a[i];
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
b211_median3_pivot(int64_t *a, size_t lo, size_t hi)
{
	size_t mid = lo + ((hi - lo) >> 1);

	/* Order a[lo], a[mid], a[hi] so a[mid] is the median of the three. */
	if (a[lo] > a[mid]) {
		b211_swap(&a[lo], &a[mid]);
	}
	if (a[lo] > a[hi]) {
		b211_swap(&a[lo], &a[hi]);
	}
	if (a[mid] > a[hi]) {
		b211_swap(&a[mid], &a[hi]);
	}
	/* a[lo] <= a[mid] <= a[hi]; park median (mid) at hi for partition. */
	b211_swap(&a[mid], &a[hi]);
}

/*
 * Lomuto partition on a[lo..hi] with pivot already at a[hi].
 * Returns final pivot index p such that a[lo..p-1] <= a[p] <= a[p+1..hi].
 */
static size_t
b211_partition(int64_t *a, size_t lo, size_t hi)
{
	int64_t pivot = a[hi];
	size_t i = lo;
	size_t j;

	for (j = lo; j < hi; j++) {
		if (a[j] < pivot) {
			b211_swap(&a[i], &a[j]);
			i++;
		}
	}
	b211_swap(&a[i], &a[hi]);
	return i;
}

/*
 * Place the element of rank k (0-based, k in [0, n)) into a[k] by
 * partial selection. Reorders a. After return, a[k] is the (k+1)-th
 * smallest; elements left of k are <= a[k] and right are >= a[k].
 */
static void
b211_select(int64_t *a, size_t n, size_t k)
{
	size_t lo = 0u;
	size_t hi = n - 1u;

	for (;;) {
		size_t span = hi - lo + 1u;
		size_t p;

		if (span <= B211_INS_MAX) {
			b211_insert_sort(a, lo, hi);
			return;
		}

		b211_median3_pivot(a, lo, hi);
		p = b211_partition(a, lo, hi);

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

/* ---- gj_median_i64 ----------------------------------------------------- */

/*
 * Lower median of a[0..n) via in-place partial select (may reorder a).
 * Rank k = (n-1)/2: middle element when n is odd; lower of the two
 * central elements when n is even. Returns 0 if a is NULL or n is 0.
 */
int64_t
gj_median_i64(int64_t *a, size_t n)
{
	size_t k;

	if (a == NULL || n == 0u) {
		return 0;
	}
	if (n == 1u) {
		return a[0];
	}

	k = (n - 1u) / 2u;
	b211_select(a, n, k);
	return a[k];
}

int64_t __gj_median_i64(int64_t *a, size_t n)
    __attribute__((alias("gj_median_i64")));
