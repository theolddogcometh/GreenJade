/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2415: case-insensitive first-needle search in
 * a bounded haystack of n octets.
 *
 * Surface (unique symbols):
 *   size_t gj_str_find_ci_n(const char *hay, size_t n, const char *needle);
 *     - Return the first index i in [0, n) where hay[i..] matches needle
 *       under ASCII case fold (A-Z -> a-z), or n if not found. hay is a
 *       fixed n-byte window (binary-safe; embedded NULs are ordinary).
 *       needle is a NUL-terminated C string. Empty needle -> 0 when
 *       hay is non-NULL or n == 0; NULL hay with n > 0 -> n; NULL
 *       needle -> n (not found).
 *   size_t __gj_str_find_ci_n  (alias)
 *   __libcgj_batch2415_marker = "libcgj-batch2415"
 *
 * Distinct from gj_kmp_find / gj_rk_find / gj_str_index_char_n — unique
 * CI bounded surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2415_marker[] = "libcgj-batch2415";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2415_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* Byte length of a NUL-terminated string; 0 if pSz is NULL. */
static size_t
b2415_strlen(const char *pSz)
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
b2415_match_at(const unsigned char *pHay, size_t iHay,
    const unsigned char *pNeedle, size_t cbNeed)
{
	size_t i;

	for (i = 0u; i < cbNeed; i++) {
		if (b2415_fold(pHay[iHay + i]) != b2415_fold(pNeedle[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_find_ci_n - first CI index of needle in hay[0..n), or n.
 *
 * hay:    buffer of n octets when n > 0 (NULL with n > 0 -> return n)
 * n:      haystack byte count
 * needle: NUL-terminated needle (NULL -> n; empty -> 0)
 *
 * Binary-safe over the hay window; does not call libc.
 */
size_t
gj_str_find_ci_n(const char *hay, size_t n, const char *needle)
{
	const unsigned char *pHay;
	const unsigned char *pNeedle;
	size_t cbNeed;
	size_t iHay;

	if (needle == NULL) {
		return n;
	}

	cbNeed = b2415_strlen(needle);
	if (cbNeed == 0u) {
		/* Empty needle matches at index 0 (including n == 0). */
		return 0u;
	}

	if (n == 0u) {
		return 0u;
	}
	if (hay == NULL) {
		return n;
	}
	if (cbNeed > n) {
		return n;
	}

	pHay = (const unsigned char *)hay;
	pNeedle = (const unsigned char *)needle;
	for (iHay = 0u; iHay + cbNeed <= n; iHay++) {
		if (b2415_match_at(pHay, iHay, pNeedle, cbNeed) != 0) {
			return iHay;
		}
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_find_ci_n(const char *hay, size_t n, const char *needle)
    __attribute__((alias("gj_str_find_ci_n")));
