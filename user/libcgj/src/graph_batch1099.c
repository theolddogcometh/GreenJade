/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1099: milli-Kulczynski similarity of two sorted unique uint32 sets
 *
 * Surface (unique symbols):
 *   uint32_t gj_kulczynski_u32(const uint32_t *a, size_t na,
 *                              const uint32_t *b, size_t nb);
 *     — Kulczynski II milli-similarity in [0, 1000]:
 *         floor(500 * |A∩B| / |A| + 500 * |A∩B| / |B|)
 *       (average of the two coverage ratios). Either empty (not both)
 *       → 0; both empty → 1000.
 *   uint32_t __gj_kulczynski_u32  (alias)
 *   __libcgj_batch1099_marker = "libcgj-batch1099"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1099_marker[] = "libcgj-batch1099";

/* Two-pointer intersection / only-A / only-B counts on sorted unique arrays. */
static void
b1099_set_counts(const uint32_t *pA, size_t cA, const uint32_t *pB, size_t cB,
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
 * gj_kulczynski_u32 — Kulczynski II milli-similarity.
 *
 * K = 500 * inter / na + 500 * inter / nb   (integer floors, then sum)
 */
uint32_t
gj_kulczynski_u32(const uint32_t *pA, size_t cA,
		  const uint32_t *pB, size_t cB)
{
	size_t cInter;
	uint32_t u32Left;
	uint32_t u32Right;
	uint32_t u32Sum;

	if ((pA == NULL && cA > 0u) || (pB == NULL && cB > 0u)) {
		return 0u;
	}
	if (cA == 0u && cB == 0u) {
		return 1000u;
	}
	if (cA == 0u || cB == 0u) {
		return 0u;
	}

	b1099_set_counts(pA, cA, pB, cB, &cInter, NULL, NULL);
	u32Left = (uint32_t)((cInter * 500u) / cA);
	u32Right = (uint32_t)((cInter * 500u) / cB);
	u32Sum = u32Left + u32Right;
	if (u32Sum > 1000u) {
		u32Sum = 1000u;
	}
	return u32Sum;
}

uint32_t __gj_kulczynski_u32(const uint32_t *pA, size_t cA,
			     const uint32_t *pB, size_t cB)
    __attribute__((alias("gj_kulczynski_u32")));
