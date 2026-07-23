/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch360: reverse space-separated word order
 * in-place on a mutable NUL-terminated C string.
 *
 * Surface (unique symbols):
 *   int gj_reverse_words(char *s);
 *     — Reverse the order of words in s. Words are maximal runs of
 *       non-space characters; the only separator is ASCII space (' ').
 *       Leading, trailing, and repeated spaces are preserved in place
 *       relative to the reversed word sequence (the classic reverse-
 *       whole-string then reverse-each-word algorithm). Returns the
 *       number of words on success (0 for empty / all-space strings);
 *       returns -1 if s is NULL.
 *   int __gj_reverse_words  (alias)
 *   __libcgj_batch360_marker = "libcgj-batch360"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch360_marker[] = "libcgj-batch360";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse the closed byte range sz[iLo .. iHi] inclusive.
 * Half-count loop avoids size_t underflow when iLo/iHi cross.
 */
static void
b360_rev_range(char *sz, size_t iLo, size_t iHi)
{
	size_t i;
	size_t cHalf;

	if (iLo >= iHi) {
		return;
	}
	cHalf = (iHi - iLo + 1u) / 2u;
	for (i = 0u; i < cHalf; i++) {
		char chTmp = sz[iLo + i];

		sz[iLo + i] = sz[iHi - i];
		sz[iHi - i] = chTmp;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reverse_words — reverse space-separated word order in place.
 *
 * s: mutable NUL-terminated string (NULL → -1).
 *
 * Algorithm (O(n) time, O(1) extra):
 *   1. Reverse the entire character range [0, len).
 *   2. Reverse each word (maximal non-space run) back to readable order.
 *
 * Returns the word count (>= 0), or -1 on NULL s.
 */
int
gj_reverse_words(char *sz)
{
	size_t cbLen;
	size_t i;
	size_t iWord;
	int cWords;

	if (sz == NULL) {
		return -1;
	}

	/* Measure length without libc strlen. */
	cbLen = 0u;
	while (sz[cbLen] != '\0') {
		cbLen++;
	}

	if (cbLen == 0u) {
		return 0;
	}

	/* Step 1: reverse the whole buffer. */
	b360_rev_range(sz, 0u, cbLen - 1u);

	/* Step 2: reverse each word; count words. */
	cWords = 0;
	i = 0u;
	while (i < cbLen) {
		/* Skip spaces (preserved). */
		while (i < cbLen && sz[i] == ' ') {
			i++;
		}
		if (i >= cbLen) {
			break;
		}

		/* Word: [iWord, i) will be non-space after the scan. */
		iWord = i;
		while (i < cbLen && sz[i] != ' ') {
			i++;
		}
		/* Reverse the closed range [iWord, i-1]. */
		if (i > iWord) {
			b360_rev_range(sz, iWord, i - 1u);
			cWords++;
		}
	}

	return cWords;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_reverse_words(char *sz)
    __attribute__((alias("gj_reverse_words")));
