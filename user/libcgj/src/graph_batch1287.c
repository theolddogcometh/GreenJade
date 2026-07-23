/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1287: galloping search on sorted u32.
 *
 * Surface (unique symbols — distinct from gj_u32_binary_search
 * (batch1281) and gj_bsearch_u32 (batch407)):
 *   size_t gj_u32_gallop_search(const uint32_t *a, size_t n, uint32_t v);
 *     — Locate v in sorted nondecreasing a[0..n) by exponential
 *       (galloping) probe to bound a window, then binary search inside.
 *       Return any matching index, or n if absent. NULL a or n == 0 → n.
 *   size_t __gj_u32_gallop_search  (alias)
 *   __libcgj_batch1287_marker = "libcgj-batch1287"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1287_marker[] = "libcgj-batch1287";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1287_bin(const uint32_t *pA, size_t iLo, size_t iHi, uint32_t u32V)
{
	while (iLo < iHi) {
		size_t iMid = iLo + (iHi - iLo) / 2u;

		if (pA[iMid] < u32V) {
			iLo = iMid + 1u;
		} else if (pA[iMid] > u32V) {
			iHi = iMid;
		} else {
			return iMid;
		}
	}
	return (size_t)-1; /* sentinel: not in window (caller maps to n) */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gallop_search — gallop then binary-search for v; n if absent.
 */
size_t
gj_u32_gallop_search(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iPrev;
	size_t iStep;
	size_t iCur;
	size_t iHit;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	/* Quick ends. */
	if (pA[0] == u32V) {
		return 0u;
	}
	if (pA[cN - 1u] == u32V) {
		return cN - 1u;
	}
	if (u32V < pA[0] || u32V > pA[cN - 1u]) {
		return cN;
	}

	/* Gallop: iStep doubles while a[iCur] < v and in range. */
	iPrev = 0u;
	iStep = 1u;
	iCur = 1u;
	while (iCur < cN && pA[iCur] < u32V) {
		iPrev = iCur;
		if (iStep > cN - 1u - iCur) {
			iCur = cN - 1u;
			break;
		}
		iCur += iStep;
		iStep <<= 1;
		if (iStep == 0u) {
			/* Overflow guard: fall back to full remaining. */
			iCur = cN - 1u;
			break;
		}
	}
	if (iCur >= cN) {
		iCur = cN - 1u;
	}

	if (pA[iCur] == u32V) {
		return iCur;
	}

	/* Binary search in (iPrev, iCur] open/closed as [iPrev+1, iCur+1). */
	iHit = b1287_bin(pA, iPrev, iCur + 1u, u32V);
	if (iHit == (size_t)-1) {
		return cN;
	}
	return iHit;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_gallop_search(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_u32_gallop_search")));
