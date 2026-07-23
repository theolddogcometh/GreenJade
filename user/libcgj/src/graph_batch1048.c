/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1048: partial selection-sort of uint32_t[].
 *
 * Surface (unique symbols):
 *   void gj_u32_partial_sort(uint32_t *a, size_t n, size_t k);
 *     — Place the k smallest elements of a[0..n) into a[0..k) in
 *       ascending order via selection sort on the first k slots
 *       (each step mins over the remaining suffix [i, n)). If k > n,
 *       k is clamped to n (full ascending selection sort). No-op if
 *       a == NULL, n == 0, or k == 0.
 *   void __gj_u32_partial_sort  (alias)
 *   __libcgj_batch1048_marker = "libcgj-batch1048"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1048_marker[] = "libcgj-batch1048";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_partial_sort — selection-sort the first k positions.
 *
 * After return, a[0] <= a[1] <= ... <= a[k-1], and each a[i] (i < k)
 * is among the k smallest original values (ties broken by first-min scan).
 * Elements a[k..n) are a permutation of the remaining values (unordered).
 */
void
gj_u32_partial_sort(uint32_t *a, size_t n, size_t k)
{
	size_t i;
	size_t j;
	size_t iMin;
	uint32_t uT;

	if (a == NULL || n == 0u || k == 0u) {
		return;
	}
	if (k > n) {
		k = n;
	}

	for (i = 0u; i < k; i++) {
		iMin = i;
		for (j = i + 1u; j < n; j++) {
			if (a[j] < a[iMin]) {
				iMin = j;
			}
		}
		if (iMin != i) {
			uT = a[i];
			a[i] = a[iMin];
			a[iMin] = uT;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_partial_sort(uint32_t *a, size_t n, size_t k)
    __attribute__((alias("gj_u32_partial_sort")));
