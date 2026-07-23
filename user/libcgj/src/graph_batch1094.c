/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1094: milli-Jaccard of two sorted unique uint32 sets
 *
 * Surface (unique symbols):
 *   uint32_t gj_jaccard_u32_sets(const uint32_t *a, size_t na,
 *                                const uint32_t *b, size_t nb);
 *     — Millijaccard in [0, 1000]:
 *         floor(|A∩B| * 1000 / |A∪B|)
 *       Inputs must be strictly increasing (sorted unique). Empty vs
 *       empty → 1000. NULL either with positive length → 0.
 *   uint32_t __gj_jaccard_u32_sets  (alias)
 *   __libcgj_batch1094_marker = "libcgj-batch1094"
 *
 * Distinct from gj_jaccard_u64 (batch538 bitset) — avoid multi-def.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1094_marker[] = "libcgj-batch1094";

/* Two-pointer intersection / only-A / only-B counts on sorted unique arrays. */
static void
b1094_set_counts(const uint32_t *pA, size_t cA, const uint32_t *pB, size_t cB,
		size_t *pInter, size_t *pOnlyA, size_t *pOnlyB)
{
	size_t iA;
	size_t iB;
	size_t cInter;
	size_t cOnlyA;
	size_t cOnlyB;

	iA = 0u;
	iB = 0u;
	cInter = 0u;
	cOnlyA = 0u;
	cOnlyB = 0u;

	while (iA < cA && iB < cB) {
		if (pA[iA] == pB[iB]) {
			cInter++;
			iA++;
			iB++;
		} else if (pA[iA] < pB[iB]) {
			cOnlyA++;
			iA++;
		} else {
			cOnlyB++;
			iB++;
		}
	}
	cOnlyA += (cA - iA);
	cOnlyB += (cB - iB);

	if (pInter != NULL) {
		*pInter = cInter;
	}
	if (pOnlyA != NULL) {
		*pOnlyA = cOnlyA;
	}
	if (pOnlyB != NULL) {
		*pOnlyB = cOnlyB;
	}
}

/*
 * gj_jaccard_u32_sets — milli-Jaccard of two sorted unique u32 arrays.
 */
uint32_t
gj_jaccard_u32_sets(const uint32_t *pA, size_t cA,
		    const uint32_t *pB, size_t cB)
{
	size_t cInter;
	size_t cOnlyA;
	size_t cOnlyB;
	size_t cUnion;

	if ((pA == NULL && cA > 0u) || (pB == NULL && cB > 0u)) {
		return 0u;
	}
	if (cA == 0u && cB == 0u) {
		return 1000u;
	}

	b1094_set_counts(pA, cA, pB, cB, &cInter, &cOnlyA, &cOnlyB);
	cUnion = cInter + cOnlyA + cOnlyB;
	if (cUnion == 0u) {
		return 1000u;
	}
	return (uint32_t)((cInter * 1000u) / cUnion);
}

uint32_t __gj_jaccard_u32_sets(const uint32_t *pA, size_t cA,
			       const uint32_t *pB, size_t cB)
    __attribute__((alias("gj_jaccard_u32_sets")));
