/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2416: case-insensitive non-overlapping
 * substring count in a bounded haystack of n octets.
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_ci_n(const char *hay, size_t n, const char *needle);
 *     - Count non-overlapping occurrences of needle in hay[0..n) under
 *       ASCII case fold (A-Z -> a-z). After each match, search resumes
 *       past the matched needle (advance by needle length). Empty
 *       needle, NULL hay (n > 0), or NULL needle yield 0. Overlapping
 *       placements are not counted separately.
 *   size_t __gj_str_count_ci_n  (alias)
 *   __libcgj_batch2416_marker = "libcgj-batch2416"
 *
 * Distinct from gj_str_count_substr (batch732), gj_count_substr
 * (batch359), and gj_str_find_ci_n (batch2415) — unique surface; avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2416_marker[] = "libcgj-batch2416";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2416_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* Byte length of a NUL-terminated string; 0 if pSz is NULL. */
static size_t
b2416_strlen(const char *pSz)
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
 * True if hay[iHay .. iHay+cbNeed) equals needle[0..cbNeed) after fold.
 * Caller ensures iHay + cbNeed <= n and pointers are non-NULL.
 */
static int
b2416_match_at(const unsigned char *pHay, size_t iHay,
    const unsigned char *pNeedle, size_t cbNeed)
{
	size_t i;

	for (i = 0u; i < cbNeed; i++) {
		if (b2416_fold(pHay[iHay + i]) != b2416_fold(pNeedle[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_ci_n - non-overlapping CI count of needle in hay[0..n).
 *
 * hay:    buffer of n octets when n > 0 (NULL with n > 0 -> 0)
 * n:      haystack byte count
 * needle: NUL-terminated needle (NULL or empty -> 0)
 *
 * Advances past each full match by needle length so overlaps are not
 * double-counted.
 */
size_t
gj_str_count_ci_n(const char *hay, size_t n, const char *needle)
{
	const unsigned char *pHay;
	const unsigned char *pNeedle;
	size_t cbNeed;
	size_t cHits;
	size_t iHay;

	if (needle == NULL || n == 0u) {
		return 0u;
	}
	if (hay == NULL) {
		return 0u;
	}

	cbNeed = b2416_strlen(needle);
	if (cbNeed == 0u || cbNeed > n) {
		return 0u;
	}

	pHay = (const unsigned char *)hay;
	pNeedle = (const unsigned char *)needle;
	cHits = 0u;
	iHay = 0u;
	while (iHay + cbNeed <= n) {
		if (b2416_match_at(pHay, iHay, pNeedle, cbNeed) != 0) {
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

size_t __gj_str_count_ci_n(const char *hay, size_t n, const char *needle)
    __attribute__((alias("gj_str_count_ci_n")));
