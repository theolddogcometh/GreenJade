/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1049: k-th largest uint32_t in an array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_nth_largest(uint32_t *a, size_t n, size_t k);
 *     — Return the k-th largest element of a[0..n) (k is 0-based:
 *       k == 0 is the maximum). Uses in-place selection of the first
 *       k+1 largest into a[0..k] (descending), then returns a[k].
 *       Mutates a. If a == NULL, n == 0, or k >= n → returns 0.
 *   uint32_t __gj_u32_nth_largest  (alias)
 *   __libcgj_batch1049_marker = "libcgj-batch1049"
 *
 * Distinct from gj_u32_partial_sort (batch1048 ascending k-smallest).
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1049_marker[] = "libcgj-batch1049";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_nth_largest — 0-based k-th largest value in a[0..n).
 *
 * Selection: for i in 0..k, place max of a[i..n) at a[i].
 */
uint32_t
gj_u32_nth_largest(uint32_t *a, size_t n, size_t k)
{
	size_t i;
	size_t j;
	size_t iMax;
	uint32_t uT;

	if (a == NULL || n == 0u || k >= n) {
		return 0u;
	}

	for (i = 0u; i <= k; i++) {
		iMax = i;
		for (j = i + 1u; j < n; j++) {
			if (a[j] > a[iMax]) {
				iMax = j;
			}
		}
		if (iMax != i) {
			uT = a[i];
			a[i] = a[iMax];
			a[iMax] = uT;
		}
	}
	return a[k];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_nth_largest(uint32_t *a, size_t n, size_t k)
    __attribute__((alias("gj_u32_nth_largest")));
