/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3539: in-place k-th smallest uint32_t
 * (nth_element, _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_nth_element_u(uint32_t *a, size_t n, size_t k);
 *     - Rearrange a[0..n) so a[k] is the k-th smallest (0-based) and
 *       return that value. On success every a[i] for i < k is <= a[k]
 *       and every a[j] for j > k is >= a[k]. NULL a, n == 0, or
 *       k >= n → 0.
 *   uint32_t __gj_arr_u32_nth_element_u  (alias)
 *   __libcgj_batch3539_marker = "libcgj-batch3539"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540). Distinct from
 * gj_u32_select_nth (batch712 status return) and gj_select_k_i32
 * (batch310) — unique arr_u32_nth_element_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3539_marker[] = "libcgj-batch3539";

/* Insertion-select threshold (elements in the active span). */
#define B3539_INS_MAX 16u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3539_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t uT;

	uT = *pA;
	*pA = *pB;
	*pB = uT;
}

static void
b3539_insert_sort(uint32_t *pA, size_t lo, size_t hi)
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
b3539_median3_pivot(uint32_t *pA, size_t lo, size_t hi)
{
	size_t mid = lo + ((hi - lo) >> 1);

	if (pA[lo] > pA[mid]) {
		b3539_swap(&pA[lo], &pA[mid]);
	}
	if (pA[lo] > pA[hi]) {
		b3539_swap(&pA[lo], &pA[hi]);
	}
	if (pA[mid] > pA[hi]) {
		b3539_swap(&pA[mid], &pA[hi]);
	}
	b3539_swap(&pA[mid], &pA[hi]);
}

static size_t
b3539_partition(uint32_t *pA, size_t lo, size_t hi)
{
	uint32_t uPivot = pA[hi];
	size_t i = lo;
	size_t j;

	for (j = lo; j < hi; j++) {
		if (pA[j] < uPivot) {
			b3539_swap(&pA[i], &pA[j]);
			i++;
		}
	}
	b3539_swap(&pA[i], &pA[hi]);
	return i;
}

/* Place rank k into a[k]. pA non-NULL, n > 0, k < n. */
static void
b3539_select(uint32_t *pA, size_t cN, size_t k)
{
	size_t lo = 0u;
	size_t hi = cN - 1u;

	for (;;) {
		size_t span = hi - lo + 1u;
		size_t p;

		if (span <= B3539_INS_MAX) {
			b3539_insert_sort(pA, lo, hi);
			return;
		}

		b3539_median3_pivot(pA, lo, hi);
		p = b3539_partition(pA, lo, hi);

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
 * gj_arr_u32_nth_element_u - k-th smallest of a[0..n) via quickselect.
 *
 * a: mutable base (NULL → 0); may be reordered
 * n: element count (0 → 0)
 * k: 0-based rank (k >= n → 0)
 *
 * After success a[k] holds the order statistic and the array is
 * partitioned about k. Does not call libc.
 */
uint32_t
gj_arr_u32_nth_element_u(uint32_t *pA, size_t cN, size_t k)
{
	(void)NULL;
	if (pA == NULL || cN == 0u || k >= cN) {
		return 0u;
	}
	if (cN == 1u) {
		return pA[0];
	}

	b3539_select(pA, cN, k);
	return pA[k];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_nth_element_u(uint32_t *pA, size_t cN, size_t k)
    __attribute__((alias("gj_arr_u32_nth_element_u")));
