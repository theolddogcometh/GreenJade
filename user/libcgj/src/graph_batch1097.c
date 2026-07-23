/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1097: milli-overlap coefficient of two sorted unique uint32 sets
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlap_coeff_u32(const uint32_t *a, size_t na,
 *                                 const uint32_t *b, size_t nb);
 *     — Overlap (Szymkiewicz–Simpson) milli-coefficient in [0, 1000]:
 *         floor(|A∩B| * 1000 / min(|A|, |B|))
 *       Either empty (and not both) → 0; both empty → 1000.
 *   uint32_t __gj_overlap_coeff_u32  (alias)
 *   __libcgj_batch1097_marker = "libcgj-batch1097"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1097_marker[] = "libcgj-batch1097";

/* Two-pointer intersection / only-A / only-B counts on sorted unique arrays. */
static void
b1097_set_counts(const uint32_t *pA, size_t cA, const uint32_t *pB, size_t cB,
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
 * gj_overlap_coeff_u32 — milli-overlap coefficient of two sorted sets.
 */
uint32_t
gj_overlap_coeff_u32(const uint32_t *pA, size_t cA,
		     const uint32_t *pB, size_t cB)
{
	size_t cInter;
	size_t cMin;

	if ((pA == NULL && cA > 0u) || (pB == NULL && cB > 0u)) {
		return 0u;
	}
	if (cA == 0u && cB == 0u) {
		return 1000u;
	}
	if (cA == 0u || cB == 0u) {
		return 0u;
	}

	b1097_set_counts(pA, cA, pB, cB, &cInter, NULL, NULL);
	cMin = (cA < cB) ? cA : cB;
	return (uint32_t)((cInter * 1000u) / cMin);
}

uint32_t __gj_overlap_coeff_u32(const uint32_t *pA, size_t cA,
				const uint32_t *pB, size_t cB)
    __attribute__((alias("gj_overlap_coeff_u32")));
