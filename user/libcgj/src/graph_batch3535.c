/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3535: lower median of a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_median_u(uint32_t *a, size_t n);
 *     - Return the lower median of a[0..n): after in-place partial
 *       selection the element at rank k = (n-1)/2 is the median
 *       (middle for odd n, lower of the two middles for even n).
 *       May reorder a. NULL a or n == 0 → 0.
 *   uint32_t __gj_arr_u32_median_u  (alias)
 *   __libcgj_batch3535_marker = "libcgj-batch3535"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540). Distinct from
 * gj_median_i64 (batch211) and gj_median3_u64 (batch590) — unique
 * arr_u32_median_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3535_marker[] = "libcgj-batch3535";

/* Insertion-select threshold (elements in the active span). */
#define B3535_INS_MAX 16u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3535_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t uT;

	uT = *pA;
	*pA = *pB;
	*pB = uT;
}

static void
b3535_insert_sort(uint32_t *pA, size_t lo, size_t hi)
{
	size_t i;

	for (i = lo + 1u; i <= hi; i++) {
		uint32_t uV = pA[i];
		size_t j = i;

		while (j > lo && pA[j - 1u] > uV) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = uV;
	}
}

static void
b3535_median3_pivot(uint32_t *pA, size_t lo, size_t hi)
{
	size_t mid = lo + ((hi - lo) >> 1);

	if (pA[lo] > pA[mid]) {
		b3535_swap(&pA[lo], &pA[mid]);
	}
	if (pA[lo] > pA[hi]) {
		b3535_swap(&pA[lo], &pA[hi]);
	}
	if (pA[mid] > pA[hi]) {
		b3535_swap(&pA[mid], &pA[hi]);
	}
	b3535_swap(&pA[mid], &pA[hi]);
}

static size_t
b3535_partition(uint32_t *pA, size_t lo, size_t hi)
{
	uint32_t uPivot = pA[hi];
	size_t i = lo;
	size_t j;

	for (j = lo; j < hi; j++) {
		if (pA[j] < uPivot) {
			b3535_swap(&pA[i], &pA[j]);
			i++;
		}
	}
	b3535_swap(&pA[i], &pA[hi]);
	return i;
}

/* Place rank k into a[k]. pA non-NULL, n > 0, k < n. */
static void
b3535_select(uint32_t *pA, size_t cN, size_t k)
{
	size_t lo = 0u;
	size_t hi = cN - 1u;

	for (;;) {
		size_t span = hi - lo + 1u;
		size_t p;

		if (span <= B3535_INS_MAX) {
			b3535_insert_sort(pA, lo, hi);
			return;
		}

		b3535_median3_pivot(pA, lo, hi);
		p = b3535_partition(pA, lo, hi);

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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_median_u - lower median of a[0..n) via in-place select.
 *
 * a: mutable base (NULL → 0); may be reordered
 * n: element count (0 → 0)
 *
 * Rank k = (n-1)/2. Does not call libc.
 */
uint32_t
gj_arr_u32_median_u(uint32_t *pA, size_t cN)
{
	size_t k;

	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	if (cN == 1u) {
		return pA[0];
	}

	k = (cN - 1u) / 2u;
	b3535_select(pA, cN, k);
	return pA[k];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_median_u(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u32_median_u")));
