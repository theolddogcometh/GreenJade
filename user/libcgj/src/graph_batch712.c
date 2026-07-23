/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch712: freestanding in-place k-th smallest
 * selection over a bounded uint32_t array (0-based order statistic).
 *
 * Surface (unique symbols):
 *   int gj_u32_select_nth(uint32_t *a, size_t n, size_t k);
 *     — Rearrange a[0..n) in place so that a[k] is the k-th smallest
 *       element (0-based). On success every a[i] for i < k is <= a[k]
 *       and every a[j] for j > k is >= a[k]. Returns 0 (OK) on success,
 *       -1 (FAIL) if a is NULL, n is 0, or k >= n.
 *   int __gj_u32_select_nth  (alias)
 *   __libcgj_batch712_marker = "libcgj-batch712"
 *
 * Distinct from full sorts (gj_u32_insertion_sort batch669, gj_qsort /
 * gj_heapsort batch100) and from heap surfaces (batch671–674). Unique
 * gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch712_marker[] = "libcgj-batch712";

/* ---- internal helpers -------------------------------------------------- */

static void
b712_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t nTmp;

	nTmp = *pA;
	*pA = *pB;
	*pB = nTmp;
}

/*
 * Lomuto partition of a[lo..hi] inclusive around pivot a[hi].
 * Returns final pivot index p such that a[lo..p) <= a[p] <= a[p+1..hi].
 */
static size_t
b712_partition(uint32_t *a, size_t lo, size_t hi)
{
	uint32_t nPivot;
	size_t i;
	size_t j;

	nPivot = a[hi];
	i = lo;
	for (j = lo; j < hi; j++) {
		if (a[j] <= nPivot) {
			b712_swap(&a[i], &a[j]);
			i++;
		}
	}
	b712_swap(&a[i], &a[hi]);
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_select_nth — in-place select of the k-th smallest uint32_t.
 *
 * a: base of n elements (NULL → -1)
 * n: element count (0 → -1)
 * k: 0-based rank to select (k >= n → -1)
 *
 * Iterative quickselect over a[0..n). Mutates a; after success a[k]
 * holds the order statistic and the array is partitioned about k.
 * Returns 0 (OK) or -1 (FAIL). Does not allocate or call libc.
 */
int
gj_u32_select_nth(uint32_t *a, size_t n, size_t k)
{
	size_t lo;
	size_t hi;

	if (a == NULL || n == 0u || k >= n) {
		return -1;
	}

	lo = 0u;
	hi = n - 1u;

	for (;;) {
		size_t p;

		if (lo == hi) {
			return 0;
		}

		p = b712_partition(a, lo, hi);
		if (p == k) {
			return 0;
		}
		if (k < p) {
			if (p == 0u) {
				return 0;
			}
			hi = p - 1u;
		} else {
			lo = p + 1u;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_select_nth(uint32_t *a, size_t n, size_t k)
    __attribute__((alias("gj_u32_select_nth")));
