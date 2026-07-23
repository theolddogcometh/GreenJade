/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch875: union of two sorted uint32 arrays.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_union_sorted(const uint32_t *a, size_t na,
 *                              const uint32_t *b, size_t nb,
 *                              uint32_t *out);
 *     — Write sorted set-union (duplicates across a/b collapsed once
 *       per equal value pair step) into out; return count. Within each
 *       array, adjacent duplicates are also collapsed (set semantics).
 *   size_t __gj_u32_union_sorted  (alias)
 *   __libcgj_batch875_marker = "libcgj-batch875"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch875_marker[] = "libcgj-batch875";

/* ---- freestanding helpers ---------------------------------------------- */

/* Skip runs of the same value; advance past current *pI. */
static void
b875_skip_dup(const uint32_t *pArr, size_t cN, size_t *pI)
{
	uint32_t u32V;
	size_t i;

	i = *pI;
	if (i >= cN) {
		return;
	}
	u32V = pArr[i];
	i++;
	while (i < cN && pArr[i] == u32V) {
		i++;
	}
	*pI = i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_union_sorted — sorted set-union of two nondecreasing runs.
 *
 * Emits each distinct value once. Caller must ensure out capacity
 * (up to na+nb). NULL out or invalid inputs with work remaining → 0.
 */
size_t
gj_u32_union_sorted(const uint32_t *pA, size_t cNa, const uint32_t *pB,
    size_t cNb, uint32_t *pOut)
{
	size_t iA;
	size_t iB;
	size_t iOut;

	if (cNa == 0u && cNb == 0u) {
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
		if (pA[iA] < pB[iB]) {
			pOut[iOut++] = pA[iA];
			b875_skip_dup(pA, cNa, &iA);
		} else if (pB[iB] < pA[iA]) {
			pOut[iOut++] = pB[iB];
			b875_skip_dup(pB, cNb, &iB);
		} else {
			pOut[iOut++] = pA[iA];
			b875_skip_dup(pA, cNa, &iA);
			b875_skip_dup(pB, cNb, &iB);
		}
	}
	while (iA < cNa) {
		pOut[iOut++] = pA[iA];
		b875_skip_dup(pA, cNa, &iA);
	}
	while (iB < cNb) {
		pOut[iOut++] = pB[iB];
		b875_skip_dup(pB, cNb, &iB);
	}
	return iOut;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_union_sorted(const uint32_t *pA, size_t cNa,
    const uint32_t *pB, size_t cNb, uint32_t *pOut)
    __attribute__((alias("gj_u32_union_sorted")));
