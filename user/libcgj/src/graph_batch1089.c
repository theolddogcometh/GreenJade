/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1089: ordinal ranks 0..n-1 for a uint32 array (stable)
 *
 * Surface (unique symbols):
 *   void gj_u32_zscore_rank(const uint32_t *a, size_t n, uint32_t *out);
 *     — For each index i, write a unique rank in 0..n-1 into out[i]
 *       such that lower values get lower ranks; ties break by lower
 *       original index (stable total order). Equivalent to the inverse
 *       of a stable argsort. NULL a/out or n == 0 → no-op. O(n^2).
 *   void __gj_u32_zscore_rank  (alias)

 *   __libcgj_batch1089_marker = "libcgj-batch1089"
 *
 * Distinct from gj_u32_argsort_indices (batch871) — avoid multi-def.
 * * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1089_marker[] = "libcgj-batch1089";

/*
 * gj_u32_zscore_rank — stable ordinal rank of each element (0 = smallest).
 *
 * rank(i) = |{ j : a[j] < a[i] }| + |{ j < i : a[j] == a[i] }|
 * so ranks are a permutation of 0..n-1 even with ties.
 */
void
gj_u32_zscore_rank(const uint32_t *pA, size_t cN, uint32_t *pOut)
{
	size_t i;
	size_t j;
	uint32_t u32Rank;

	if (pA == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		u32Rank = 0u;
		for (j = 0u; j < cN; j++) {
			if (pA[j] < pA[i]) {
				u32Rank++;
			} else if (pA[j] == pA[i] && j < i) {
				u32Rank++;
			}
		}
		pOut[i] = u32Rank;
	}
}

void __gj_u32_zscore_rank(const uint32_t *pA, size_t cN, uint32_t *pOut)
    __attribute__((alias("gj_u32_zscore_rank")));
