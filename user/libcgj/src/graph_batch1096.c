/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1096: milli-Dice coefficient of two sorted unique uint32 sets
 *
 * Surface (unique symbols):
 *   uint32_t gj_dice_u32_sets(const uint32_t *a, size_t na,
 *                             const uint32_t *b, size_t nb);
 *     — Milli-Dice in [0, 1000]:
 *         floor(2 * |A∩B| * 1000 / (|A| + |B|))
 *       Empty vs empty → 1000. Sorted unique inputs assumed.
 *   uint32_t __gj_dice_u32_sets  (alias)
 *   __libcgj_batch1096_marker = "libcgj-batch1096"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1096_marker[] = "libcgj-batch1096";

/* Two-pointer intersection / only-A / only-B counts on sorted unique arrays. */
static void
b1096_set_counts(const uint32_t *pA, size_t cA, const uint32_t *pB, size_t cB,
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
 * gj_dice_u32_sets — Sørensen–Dice milli-coefficient.
 */
uint32_t
gj_dice_u32_sets(const uint32_t *pA, size_t cA,
		 const uint32_t *pB, size_t cB)
{
	size_t cInter;
	size_t cSum;

	if ((pA == NULL && cA > 0u) || (pB == NULL && cB > 0u)) {
		return 0u;
	}
	if (cA == 0u && cB == 0u) {
		return 1000u;
	}

	b1096_set_counts(pA, cA, pB, cB, &cInter, NULL, NULL);
	cSum = cA + cB;
	if (cSum == 0u) {
		return 1000u;
	}
	return (uint32_t)((2u * cInter * 1000u) / cSum);
}

uint32_t __gj_dice_u32_sets(const uint32_t *pA, size_t cA,
			    const uint32_t *pB, size_t cB)
    __attribute__((alias("gj_dice_u32_sets")));
