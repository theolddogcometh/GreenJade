/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2882: qsort-style Lomuto partition on u32 (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_qsort_u32_partition_u(uint32_t *a, size_t n);
 *     - Lomuto-partition a[0..n) about pivot a[n-1]. On return, every
 *       element left of the pivot is < pivot and every element right is
 *       >= pivot; pivot sits at the returned index. n < 2 or NULL a → 0.
 *   size_t __gj_qsort_u32_partition_u  (alias)
 *   __libcgj_batch2882_marker = "libcgj-batch2882"
 *
 * Exclusive sort/search helpers wave (2881-2890). Distinct from
 * gj_u32_partition (batch711 external pivot value) and from internal
 * Lomuto in gj_qsort (batch100) — unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2882_marker[] = "libcgj-batch2882";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2882_swap(uint32_t *pA, size_t i, size_t j)
{
	uint32_t u32T;

	u32T = pA[i];
	pA[i] = pA[j];
	pA[j] = u32T;
}

/*
 * Lomuto: pivot is a[n-1]. Scan a[0..n-1); elements < pivot pack left.
 * Returns final pivot index in [0, n). Requires n >= 2 and non-NULL a.
 */
static size_t
b2882_part(uint32_t *pA, size_t cN)
{
	size_t iHi;
	size_t iStore;
	size_t i;
	uint32_t u32Pivot;

	iHi = cN - 1u;
	u32Pivot = pA[iHi];
	iStore = 0u;
	for (i = 0u; i < iHi; i++) {
		if (pA[i] < u32Pivot) {
			if (i != iStore) {
				b2882_swap(pA, i, iStore);
			}
			iStore++;
		}
	}
	if (iStore != iHi) {
		b2882_swap(pA, iStore, iHi);
	}
	return iStore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_u32_partition_u - Lomuto partition of a[0..n) about a[n-1].
 *
 * a: base of n uint32_t elements (NULL → return 0)
 * n: element count (n < 2 → return 0, no mutation when n < 2)
 *
 * After return, a[0..ret) are all < a[ret], a[ret] holds the original
 * last element (pivot), and a[ret+1..n) are all >= pivot. Relative order
 * among equals is not guaranteed. Mutates a[0..n) only.
 */
size_t
gj_qsort_u32_partition_u(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return 0u;
	}
	return b2882_part(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_qsort_u32_partition_u(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_qsort_u32_partition_u")));
