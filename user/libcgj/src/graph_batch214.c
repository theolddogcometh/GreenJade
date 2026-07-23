/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch214: longest common subsequence length (short
 * strings). Clean-room freestanding pure C — integer/pointer only (no libc
 * string). Builds under userspace -msse2; this TU does not require SSE.
 * No third-party LCS source was copied.
 *
 * Surface (unique symbols):
 *   int gj_lcs_len(const char *a, const char *b);
 *     — classic LCS length via Wagner–Fischer-style DP (match → diagonal+1;
 *       mismatch → max of skip-a / skip-b). Stack DP only; both strings
 *       capped at B214_MAX_LEN (64) characters (excluding trailing NUL).
 *     Returns LCS length in 0..64 on success.
 *     Returns -1 on NULL args or if either string exceeds max length.
 *   __gj_lcs_len  (alias)
 *   __libcgj_batch214_marker = "libcgj-batch214"
 *
 * DP layout (two rows of length nLenB+1, stack-allocated):
 *   aPrev[j] = LCS length for prefix a[0..i-1) vs b[0..j)
 *   aCur[j]  = LCS length for prefix a[0..i)   vs b[0..j)
 * After processing character a[i-1], aPrev ↔ aCur.
 *
 * Distinct from Levenshtein edit distance (batch170) and Hamming distance
 * (batch184).
 */

#include <stddef.h>

/* Maximum accepted string length (not including trailing NUL). */
#define B214_MAX_LEN 64

const char __libcgj_batch214_marker[] = "libcgj-batch214";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure NUL-terminated length without libc. Caps scan at nMax+1 so a
 * missing terminator on a huge buffer still terminates. Returns length if
 * 0..nMax, or -1 if longer than nMax (or if a NUL was not seen by then).
 */
static int
b214_strnlen_cap(const char *sz, int nMax)
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
b214_max2(int nA, int nB)
{
	return (nA > nB) ? nA : nB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcs_len — stack-DP longest common subsequence length for short C strings.
 *
 * Both inputs must be non-NULL and at most B214_MAX_LEN characters.
 * Empty strings are valid (LCS length 0). Characters compared as unsigned
 * bytes (NUL terminates; no locale / case folding).
 */
int
gj_lcs_len(const char *a, const char *b)
{
	int nLenA;
	int nLenB;
	int i;
	int j;
	/* Two rolling rows: indices 0..nLenB inclusive (max 65 ints). */
	int aPrev[B214_MAX_LEN + 1];
	int aCur[B214_MAX_LEN + 1];

	if (a == NULL || b == NULL) {
		return -1;
	}

	nLenA = b214_strnlen_cap(a, B214_MAX_LEN);
	nLenB = b214_strnlen_cap(b, B214_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return -1;
	}

	/* Empty prefix → LCS length 0. */
	if (nLenA == 0 || nLenB == 0) {
		return 0;
	}

	/* Row 0: LCS of empty a-prefix with any b-prefix is 0. */
	for (j = 0; j <= nLenB; j++) {
		aPrev[j] = 0;
	}

	for (i = 1; i <= nLenA; i++) {
		unsigned char chA = (unsigned char)a[i - 1];

		aCur[0] = 0; /* empty b-prefix */
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)b[j - 1];

			if (chA == chB) {
				aCur[j] = aPrev[j - 1] + 1;
			} else {
				aCur[j] = b214_max2(aPrev[j], aCur[j - 1]);
			}
		}
		/* Swap rows for next i (copy aCur → aPrev). */
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	return aPrev[nLenB];
}

int __gj_lcs_len(const char *a, const char *b)
    __attribute__((alias("gj_lcs_len")));
