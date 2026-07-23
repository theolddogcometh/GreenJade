/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1093: Levenshtein distance with max-distance cutoff
 *
 * Surface (unique symbols):
 *   int gj_edit_distance_bounded(const char *a, const char *b,
 *                                int max_dist);
 *     — Wagner–Fischer Levenshtein distance with early cutoff. Both
 *       strings capped at B1093_MAX_LEN (64). If the true distance is
 *       <= max_dist, return it; if it exceeds max_dist, return
 *       max_dist + 1 (or max_dist + 1 after early-exit). max_dist < 0
 *       is treated as 0 for the cutoff test. NULL a/b or over-length
 *       → -1. Distinct from gj_edit_distance / gj_levenshtein.
 *   int __gj_edit_distance_bounded  (alias)

 *   __libcgj_batch1093_marker = "libcgj-batch1093"
 *
 * Does NOT redefine gj_edit_distance or gj_levenshtein — avoid multi-def.
 * * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

#define B1093_MAX_LEN 64

const char __libcgj_batch1093_marker[] = "libcgj-batch1093";

static int
b1093_strnlen_cap(const char *sz, int nMax)
{
	int n;

	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return -1;
}

static int
b1093_min3(int nA, int nB, int nC)
{
	int nM = nA;

	if (nB < nM) {
		nM = nB;
	}
	if (nC < nM) {
		nM = nC;
	}
	return nM;
}

/*
 * gj_edit_distance_bounded — Levenshtein with max_dist cutoff.
 *
 * After each DP row, if every cell > max_dist, return max_dist + 1.
 */
int
gj_edit_distance_bounded(const char *szA, const char *szB, int nMaxDist)
{
	int nLenA;
	int nLenB;
	int i;
	int j;
	int nCap;
	int aPrev[B1093_MAX_LEN + 1];
	int aCur[B1093_MAX_LEN + 1];
	int fAny;

	if (szA == NULL || szB == NULL) {
		return -1;
	}

	if (nMaxDist < 0) {
		nMaxDist = 0;
	}

	nLenA = b1093_strnlen_cap(szA, B1093_MAX_LEN);
	nLenB = b1093_strnlen_cap(szB, B1093_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return -1;
	}

	/* Length lower bound: |na - nb| is a lower bound on distance. */
	if (nLenA > nLenB) {
		if (nLenA - nLenB > nMaxDist) {
			return nMaxDist + 1;
		}
	} else {
		if (nLenB - nLenA > nMaxDist) {
			return nMaxDist + 1;
		}
	}

	if (nLenA == 0) {
		return (nLenB <= nMaxDist) ? nLenB : (nMaxDist + 1);
	}
	if (nLenB == 0) {
		return (nLenA <= nMaxDist) ? nLenA : (nMaxDist + 1);
	}

	for (j = 0; j <= nLenB; j++) {
		aPrev[j] = j;
	}

	for (i = 1; i <= nLenA; i++) {
		unsigned char chA = (unsigned char)szA[i - 1];

		aCur[0] = i;
		fAny = 0;
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)szB[j - 1];
			int nCost = (chA == chB) ? 0 : 1;
			int nDel = aPrev[j] + 1;
			int nIns = aCur[j - 1] + 1;
			int nSub = aPrev[j - 1] + nCost;

			aCur[j] = b1093_min3(nDel, nIns, nSub);
			if (aCur[j] <= nMaxDist) {
				fAny = 1;
			}
		}
		/* Also consider ends of the band via aCur[0]. */
		if (aCur[0] <= nMaxDist) {
			fAny = 1;
		}
		if (!fAny) {
			return nMaxDist + 1;
		}
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	nCap = aPrev[nLenB];
	if (nCap > nMaxDist) {
		return nMaxDist + 1;
	}
	return nCap;
}

int __gj_edit_distance_bounded(const char *szA, const char *szB, int nMaxDist)
    __attribute__((alias("gj_edit_distance_bounded")));
