/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1562: bounded Levenshtein edit distance.
 *
 * Surface (unique symbols):
 *   int gj_str_levenshtein_bound(const char *a, const char *b);
 *     — Wagner–Fischer edit distance (ins/del/sub cost 1) via stack DP.
 *       Both strings capped at B1562_MAX_LEN (64) excluding NUL.
 *       Returns distance in 0..64 on success; -1 on NULL or oversize.
 *       Distinct from gj_levenshtein (batch170).
 *   int __gj_str_levenshtein_bound  (alias)
 *   __libcgj_batch1562_marker = "libcgj-batch1562"
 *
 * DP: two rolling rows of length nLenB+1 on the stack (max 65 ints).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

#define B1562_MAX_LEN 64

const char __libcgj_batch1562_marker[] = "libcgj-batch1562";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1562_strnlen_cap(const char *sz, int nMax)
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
b1562_min3(int nA, int nB, int nC)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_levenshtein_bound — stack-DP Levenshtein, max length 64.
 */
int
gj_str_levenshtein_bound(const char *a, const char *b)
{
	int nLenA;
	int nLenB;
	int i;
	int j;
	int aPrev[B1562_MAX_LEN + 1];
	int aCur[B1562_MAX_LEN + 1];

	if (a == NULL || b == NULL) {
		return -1;
	}

	nLenA = b1562_strnlen_cap(a, B1562_MAX_LEN);
	nLenB = b1562_strnlen_cap(b, B1562_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return -1;
	}

	if (nLenA == 0) {
		return nLenB;
	}
	if (nLenB == 0) {
		return nLenA;
	}

	for (j = 0; j <= nLenB; j++) {
		aPrev[j] = j;
	}

	for (i = 1; i <= nLenA; i++) {
		unsigned char chA = (unsigned char)a[i - 1];

		aCur[0] = i;
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)b[j - 1];
			int nCost = (chA == chB) ? 0 : 1;
			int nDel = aPrev[j] + 1;
			int nIns = aCur[j - 1] + 1;
			int nSub = aPrev[j - 1] + nCost;

			aCur[j] = b1562_min3(nDel, nIns, nSub);
		}
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	return aPrev[nLenB];
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_levenshtein_bound(const char *a, const char *b)
    __attribute__((alias("gj_str_levenshtein_bound")));
