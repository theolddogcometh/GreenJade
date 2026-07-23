/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch871: uint32 argsort indices (selection sort).
 *
 * Surface (unique symbols):
 *   void gj_u32_argsort_indices(const uint32_t *a, size_t n, size_t *idx);
 *     — Fill idx[0..n) with 0..n-1 ordered so a[idx[i]] is nondecreasing.
 *       Stable on ties (lower original index first via selection of min
 *       among remaining). NULL a or idx with n>0 → no-op. n==0 → no-op.
 *   void __gj_u32_argsort_indices  (alias)
 *   __libcgj_batch871_marker = "libcgj-batch871"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch871_marker[] = "libcgj-batch871";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_argsort_indices — permutation of 0..n-1 sorted by a values.
 *
 * Simple selection sort of indices: O(n^2). Does not mutate a.
 * On equal values, earlier original indices stay earlier (stable).
 */
void
gj_u32_argsort_indices(const uint32_t *pA, size_t cN, size_t *pIdx)
{
	size_t i;
	size_t j;
	size_t iMin;
	size_t uTmp;

	if (pA == NULL || pIdx == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		pIdx[i] = i;
	}

	for (i = 0u; i + 1u < cN; i++) {
		iMin = i;
		for (j = i + 1u; j < cN; j++) {
			uint32_t u32J = pA[pIdx[j]];
			uint32_t u32M = pA[pIdx[iMin]];

			if (u32J < u32M ||
			    (u32J == u32M && pIdx[j] < pIdx[iMin])) {
				iMin = j;
			}
		}
		if (iMin != i) {
			uTmp = pIdx[i];
			pIdx[i] = pIdx[iMin];
			pIdx[iMin] = uTmp;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_argsort_indices(const uint32_t *pA, size_t cN, size_t *pIdx)
    __attribute__((alias("gj_u32_argsort_indices")));
