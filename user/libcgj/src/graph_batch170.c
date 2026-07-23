/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch170: Levenshtein edit distance (short strings).
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; this TU does not require SSE.
 * No third-party edit-distance source was copied.
 *
 * Surface (unique symbols):
 *   int gj_levenshtein(const char *a, const char *b);
 *     — classic Wagner–Fischer edit distance (insert / delete / substitute
 *       cost 1 each; match cost 0). Stack DP only; both strings capped at
 *       B170_MAX_LEN (64) characters (excluding trailing NUL).
 *     Returns distance in 0..64 on success.
 *     Returns -1 on NULL args or if either string exceeds max length.
 *   __gj_levenshtein  (alias)
 *   __libcgj_batch170_marker = "libcgj-batch170"
 *
 * DP layout (two rows of length nLenB+1, stack-allocated):
 *   aPrev[j] = distance for prefix a[0..i-1) vs b[0..j)
 *   aCur[j]  = distance for prefix a[0..i)   vs b[0..j)
 * After processing character a[i-1], aPrev ↔ aCur.
 */

#include <stddef.h>

/* Maximum accepted string length (not including trailing NUL). */
#define B170_MAX_LEN 64

const char __libcgj_batch170_marker[] = "libcgj-batch170";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure NUL-terminated length without libc. Caps scan at nMax+1 so a
 * missing terminator on a huge buffer still terminates. Returns length if
 * 0..nMax, or -1 if longer than nMax (or if a NUL was not seen by then).
 */
static int
b170_strnlen_cap(const char *sz, int nMax)
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
b170_min3(int nA, int nB, int nC)
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
 * gj_levenshtein — stack-DP Levenshtein distance for short C strings.
 *
 * Both inputs must be non-NULL and at most B170_MAX_LEN characters.
 * Empty strings are valid (distance equals the other string's length).
 */
int
gj_levenshtein(const char *szA, const char *szB)
{
	int nLenA;
	int nLenB;
	int i;
	int j;
	/* Two rolling rows: indices 0..nLenB inclusive (max 65 ints). */
	int aPrev[B170_MAX_LEN + 1];
	int aCur[B170_MAX_LEN + 1];

	if (szA == NULL || szB == NULL) {
		return -1;
	}

	nLenA = b170_strnlen_cap(szA, B170_MAX_LEN);
	nLenB = b170_strnlen_cap(szB, B170_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return -1;
	}

	/* Trivial cases: empty string → insert/delete all characters. */
	if (nLenA == 0) {
		return nLenB;
	}
	if (nLenB == 0) {
		return nLenA;
	}

	/* Row 0: distance from empty a-prefix to b[0..j). */
	for (j = 0; j <= nLenB; j++) {
		aPrev[j] = j;
	}

	for (i = 1; i <= nLenA; i++) {
		unsigned char chA = (unsigned char)szA[i - 1];

		aCur[0] = i; /* delete all of a[0..i) */
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)szB[j - 1];
			int nCost = (chA == chB) ? 0 : 1;
			int nDel = aPrev[j] + 1;       /* delete a[i-1] */
			int nIns = aCur[j - 1] + 1;    /* insert b[j-1] */
			int nSub = aPrev[j - 1] + nCost; /* substitute/match */

			aCur[j] = b170_min3(nDel, nIns, nSub);
		}
		/* Swap rows for next i (copy aCur → aPrev). */
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	return aPrev[nLenB];
}

int __gj_levenshtein(const char *szA, const char *szB)
    __attribute__((alias("gj_levenshtein")));
