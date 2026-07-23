/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1098: milli-Tversky index of two sorted unique uint32 sets
 *
 * Surface (unique symbols):
 *   uint32_t gj_tversky_u32(const uint32_t *a, size_t na,
 *                           const uint32_t *b, size_t nb,
 *                           uint32_t alpha_milli, uint32_t beta_milli);
 *     — Tversky index in [0, 1000]:
 *         floor(inter * 1000 /
 *               (inter + (alpha_milli * onlyA + beta_milli * onlyB) / 1000))
 *       alpha_milli / beta_milli are fixed-point weights (1000 = 1.0).
 *       Both empty → 1000. Denominator zero → 1000.
 *   uint32_t __gj_tversky_u32  (alias)
 *   __libcgj_batch1098_marker = "libcgj-batch1098"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1098_marker[] = "libcgj-batch1098";

/* Two-pointer intersection / only-A / only-B counts on sorted unique arrays. */
static void
b1098_set_counts(const uint32_t *pA, size_t cA, const uint32_t *pB, size_t cB,
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
 * gj_tversky_u32 — milli-Tversky index with milli alpha/beta weights.
 *
 * denom = inter + floor((alpha * onlyA + beta * onlyB) / 1000)
 * result = floor(inter * 1000 / denom)  (1000 if denom == 0)
 */
uint32_t
gj_tversky_u32(const uint32_t *pA, size_t cA,
	       const uint32_t *pB, size_t cB,
	       uint32_t u32AlphaMilli, uint32_t u32BetaMilli)
{
	size_t cInter;
	size_t cOnlyA;
	size_t cOnlyB;
	uint64_t u64Weighted;
	uint64_t u64Denom;
	uint64_t u64Inter;

	if ((pA == NULL && cA > 0u) || (pB == NULL && cB > 0u)) {
		return 0u;
	}
	if (cA == 0u && cB == 0u) {
		return 1000u;
	}

	b1098_set_counts(pA, cA, pB, cB, &cInter, &cOnlyA, &cOnlyB);
	u64Inter = (uint64_t)cInter;
	u64Weighted = ((uint64_t)u32AlphaMilli * (uint64_t)cOnlyA +
		       (uint64_t)u32BetaMilli * (uint64_t)cOnlyB) / 1000ull;
	u64Denom = u64Inter + u64Weighted;
	if (u64Denom == 0ull) {
		return 1000u;
	}
	return (uint32_t)((u64Inter * 1000ull) / u64Denom);
}

uint32_t __gj_tversky_u32(const uint32_t *pA, size_t cA,
			  const uint32_t *pB, size_t cB,
			  uint32_t u32AlphaMilli, uint32_t u32BetaMilli)
    __attribute__((alias("gj_tversky_u32")));
