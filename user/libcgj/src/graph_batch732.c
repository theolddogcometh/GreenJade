/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch732: non-overlapping substring occurrence
 * count (gj_str_count_substr surface).
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_substr(const char *hay, const char *needle);
 *     — Count non-overlapping occurrences of needle in hay.
 *       After each match, search resumes past the matched needle
 *       (advance by needle length). Empty needle, NULL hay, or NULL
 *       needle yield 0. Overlapping placements are not counted
 *       separately (e.g. hay="aaa", needle="aa" → 1).
 *   size_t __gj_str_count_substr  (alias)
 *   __libcgj_batch732_marker = "libcgj-batch732"
 *
 * Distinct from gj_count_substr / __gj_count_substr (batch359). This TU
 * only defines the gj_str_count_substr surface — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch732_marker[] = "libcgj-batch732";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte length of a NUL-terminated string; 0 if pSz is NULL. */
static size_t
b732_strlen(const char *pSz)
{
	size_t cb;

	if (pSz == NULL) {
		return 0u;
	}
	cb = 0u;
	while (pSz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/*
 * True if hay[iHay .. iHay+cbNeed) equals needle[0..cbNeed).
 * Assumes hay has at least cbNeed remaining bytes (caller checks).
 */
static int
b732_match_at(const char *pHay, size_t iHay, const char *pNeedle,
    size_t cbNeed)
{
	size_t i;

	for (i = 0u; i < cbNeed; i++) {
		if (pHay[iHay + i] != pNeedle[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_substr — non-overlapping count of needle in hay.
 *
 * hay:    NUL-terminated haystack; NULL → 0
 * needle: NUL-terminated needle; NULL or empty → 0
 *
 * Advances past each full match by needle length so overlaps are not
 * double-counted.
 */
size_t
gj_str_count_substr(const char *hay, const char *needle)
{
	size_t cbHay;
	size_t cbNeed;
	size_t cHits;
	size_t iHay;

	if (hay == NULL || needle == NULL) {
		return 0u;
	}

	cbNeed = b732_strlen(needle);
	if (cbNeed == 0u) {
		return 0u;
	}

	cbHay = b732_strlen(hay);
	if (cbHay < cbNeed) {
		return 0u;
	}

	cHits = 0u;
	iHay = 0u;
	while (iHay + cbNeed <= cbHay) {
		if (b732_match_at(hay, iHay, needle, cbNeed) != 0) {
			cHits++;
			/* Non-overlapping: skip the whole match. */
			iHay += cbNeed;
		} else {
			iHay++;
		}
	}
	return cHits;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_substr(const char *hay, const char *needle)
    __attribute__((alias("gj_str_count_substr")));
