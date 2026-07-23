/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch873: merge two sorted uint32 arrays.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_merge_sorted(const uint32_t *a, size_t na,
 *                              const uint32_t *b, size_t nb,
 *                              uint32_t *out);
 *     — Merge nondecreasing a[0..na) and b[0..nb) into out; return
 *       count written (na+nb). out must hold na+nb elements. NULL out
 *       with na+nb>0 → 0. NULL a/b treated as empty when count 0.
 *   size_t __gj_u32_merge_sorted  (alias)
 *   __libcgj_batch873_marker = "libcgj-batch873"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch873_marker[] = "libcgj-batch873";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_merge_sorted — stable merge of two nondecreasing runs into out.
 *
 * Prefers elements from a on ties (stable with a as left run). Returns
 * number of elements written, or 0 if out is NULL and total > 0.
 */
size_t
gj_u32_merge_sorted(const uint32_t *pA, size_t cNa, const uint32_t *pB,
    size_t cNb, uint32_t *pOut)
{
	size_t iA;
	size_t iB;
	size_t iOut;
	size_t cTot;

	cTot = cNa + cNb;
	if (cTot == 0u) {
		return 0u;
	}
	if (pOut == NULL) {
		return 0u;
	}
	if (cNa > 0u && pA == NULL) {
		return 0u;
	}
	if (cNb > 0u && pB == NULL) {
		return 0u;
	}

	iA = 0u;
	iB = 0u;
	iOut = 0u;
	while (iA < cNa && iB < cNb) {
		if (pA[iA] <= pB[iB]) {
			pOut[iOut++] = pA[iA++];
		} else {
			pOut[iOut++] = pB[iB++];
		}
	}
	while (iA < cNa) {
		pOut[iOut++] = pA[iA++];
	}
	while (iB < cNb) {
		pOut[iOut++] = pB[iB++];
	}
	return iOut;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_merge_sorted(const uint32_t *pA, size_t cNa,
    const uint32_t *pB, size_t cNb, uint32_t *pOut)
    __attribute__((alias("gj_u32_merge_sorted")));
