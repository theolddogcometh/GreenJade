/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch874: intersection of two sorted uint32 arrays.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_intersect_sorted(const uint32_t *a, size_t na,
 *                                  const uint32_t *b, size_t nb,
 *                                  uint32_t *out);
 *     — Write sorted intersection of nondecreasing a and b into out;
 *       return count. Multiplicity is min of counts in a and b (multiset).
 *       NULL out with positive result possible still walks; out NULL → 0.
 *   size_t __gj_u32_intersect_sorted  (alias)
 *   __libcgj_batch874_marker = "libcgj-batch874"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch874_marker[] = "libcgj-batch874";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_intersect_sorted — multiset intersection of two sorted runs.
 *
 * Advances both pointers; on equal, emits once and advances both.
 * Caller must ensure out capacity (up to min(na, nb)).
 */
size_t
gj_u32_intersect_sorted(const uint32_t *pA, size_t cNa, const uint32_t *pB,
    size_t cNb, uint32_t *pOut)
{
	size_t iA;
	size_t iB;
	size_t iOut;

	if (cNa == 0u || cNb == 0u) {
		return 0u;
	}
	if (pA == NULL || pB == NULL || pOut == NULL) {
		return 0u;
	}

	iA = 0u;
	iB = 0u;
	iOut = 0u;
	while (iA < cNa && iB < cNb) {
		if (pA[iA] < pB[iB]) {
			iA++;
		} else if (pB[iB] < pA[iA]) {
			iB++;
		} else {
			pOut[iOut++] = pA[iA];
			iA++;
			iB++;
		}
	}
	return iOut;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_intersect_sorted(const uint32_t *pA, size_t cNa,
    const uint32_t *pB, size_t cNb, uint32_t *pOut)
    __attribute__((alias("gj_u32_intersect_sorted")));
