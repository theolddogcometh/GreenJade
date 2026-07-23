/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch236: Levenshtein edit distance for short C strings.
 *
 * Surface (unique symbols):
 *   size_t gj_edit_distance(const char *a, const char *b);
 *     — classic Wagner–Fischer edit distance (insert / delete / substitute
 *       cost 1 each; match cost 0). Stack DP only; both strings capped at
 *       B236_MAX_LEN (64) characters (excluding trailing NUL).
 *     Returns distance in 0..64 on success.
 *     Returns (size_t)-1 on NULL args or if either string exceeds max length.
 *     Empty string: distance equals the other string's length.
 *   __gj_edit_distance  (alias)
 *   __libcgj_batch236_marker = "libcgj-batch236"
 *
 * DP layout (two rows of length nLenB+1, stack-allocated):
 *   aPrev[j] = distance for prefix a[0..i-1) vs b[0..j)
 *   aCur[j]  = distance for prefix a[0..i)   vs b[0..j)
 * After processing character a[i-1], aPrev ← aCur.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno, no __uint128_t. No
 * third-party source copied. Distinct from gj_levenshtein (batch170).
 */

#include <stddef.h>

/* Maximum accepted string length (not including trailing NUL). */
#define B236_MAX_LEN 64

const char __libcgj_batch236_marker[] = "libcgj-batch236";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure NUL-terminated length without libc. Caps scan at nMax+1 so a
 * missing terminator on a huge buffer still terminates. Returns length if
 * 0..nMax, or (size_t)-1 if longer than nMax (or if a NUL was not seen).
 */
static size_t
b236_strnlen_cap(const char *sz, size_t nMax)
{
	size_t n;

	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return (size_t)-1;
}

static size_t
b236_min3(size_t nA, size_t nB, size_t nC)
{
	size_t nM = nA;

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
 * gj_edit_distance — stack-DP Levenshtein distance for short C strings.
 *
 * Both inputs must be non-NULL and at most B236_MAX_LEN characters.
 * Empty strings are valid (distance equals the other string's length).
 * Returns (size_t)-1 on NULL or over-length inputs.
 */
size_t
gj_edit_distance(const char *a, const char *b)
{
	size_t nLenA;
	size_t nLenB;
	size_t i;
	size_t j;
	/* Two rolling rows: indices 0..nLenB inclusive (max 65 size_t). */
	size_t aPrev[B236_MAX_LEN + 1];
	size_t aCur[B236_MAX_LEN + 1];

	if (a == NULL || b == NULL) {
		return (size_t)-1;
	}

	nLenA = b236_strnlen_cap(a, B236_MAX_LEN);
	nLenB = b236_strnlen_cap(b, B236_MAX_LEN);
	if (nLenA == (size_t)-1 || nLenB == (size_t)-1) {
		return (size_t)-1;
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
		unsigned char chA = (unsigned char)a[i - 1];

		aCur[0] = i; /* delete all of a[0..i) */
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)b[j - 1];
			size_t nCost = (chA == chB) ? (size_t)0 : (size_t)1;
			size_t nDel = aPrev[j] + 1;         /* delete a[i-1] */
			size_t nIns = aCur[j - 1] + 1;      /* insert b[j-1] */
			size_t nSub = aPrev[j - 1] + nCost; /* substitute/match */

			aCur[j] = b236_min3(nDel, nIns, nSub);
		}
		/* Swap rows for next i (copy aCur → aPrev). */
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	return aPrev[nLenB];
}

size_t __gj_edit_distance(const char *a, const char *b)
    __attribute__((alias("gj_edit_distance")));
