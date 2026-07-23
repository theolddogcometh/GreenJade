/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch216: Knuth–Morris–Pratt first-match string search.
 *
 * Surface (unique symbols):
 *   ssize_t gj_kmp_find(const char *hay, const char *needle);
 *     — First byte index of needle in hay, or -1 if not found / NULL args /
 *       needle longer than B216_MAX_NEEDLE. Empty needle → 0 (like strstr).
 *   __gj_kmp_find  (alias)
 *   __libcgj_batch216_marker = "libcgj-batch216"
 *
 * Classic KMP: build the longest proper prefix-suffix (LPS / π) table for
 * needle, then scan hay once with the automaton. O(|hay| + |needle|) time,
 * O(|needle|) stack for LPS (capped).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno, no libc string. No third-party
 * KMP source was copied.
 *
 * Distinct from naive strstr wrappers (batch27) and from LCS / edit-distance
 * DP (batch214 / batch170).
 */

#include <stddef.h>
#include <sys/types.h>

/* Maximum accepted needle length (excluding trailing NUL); LPS is stack. */
#define B216_MAX_NEEDLE 4096

const char __libcgj_batch216_marker[] = "libcgj-batch216";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure NUL-terminated length without libc. Caps scan at nMax+1 so a
 * missing terminator still terminates. Returns length if 0..nMax, or -1 if
 * longer than nMax.
 */
static int
b216_strnlen_cap(const char *sz, int nMax)
{
	int n;

	if (sz == NULL) {
		return -1;
	}
	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return -1;
}

/*
 * Build LPS (π) table for needle[0..nLen).
 * lps[i] = longest proper prefix of needle[0..i] that is also a suffix of
 * needle[0..i]. Stack array of length nLen provided by caller.
 */
static void
b216_build_lps(const char *needle, int nLen, int *lps)
{
	int len = 0; /* length of previous longest prefix-suffix */
	int i;

	if (nLen <= 0) {
		return;
	}

	lps[0] = 0;
	i = 1;
	while (i < nLen) {
		if (needle[i] == needle[len]) {
			len++;
			lps[i] = len;
			i++;
		} else if (len != 0) {
			len = lps[len - 1];
		} else {
			lps[i] = 0;
			i++;
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmp_find — first index of needle in hay via KMP, or -1.
 *
 * hay / needle: NUL-terminated C strings (bytes; no locale / case folding).
 * Empty needle is found at index 0 (including when hay is empty).
 * NULL either argument → -1. Needle longer than B216_MAX_NEEDLE → -1.
 * Hay may be arbitrarily long (scanned until NUL or match).
 */
ssize_t
gj_kmp_find(const char *hay, const char *needle)
{
	int nNeedle;
	int i; /* index in hay */
	int j; /* index in needle */
	int lps[B216_MAX_NEEDLE];

	if (hay == NULL || needle == NULL) {
		return (ssize_t)-1;
	}

	nNeedle = b216_strnlen_cap(needle, B216_MAX_NEEDLE);
	if (nNeedle < 0) {
		return (ssize_t)-1;
	}
	if (nNeedle == 0) {
		return (ssize_t)0;
	}

	b216_build_lps(needle, nNeedle, lps);

	i = 0;
	j = 0;
	while (hay[i] != '\0') {
		if (hay[i] == needle[j]) {
			i++;
			j++;
			if (j == nNeedle) {
				/* Match ends at i-1; start is i - nNeedle. */
				return (ssize_t)(i - nNeedle);
			}
		} else if (j != 0) {
			j = lps[j - 1];
		} else {
			i++;
		}
	}

	return (ssize_t)-1;
}

ssize_t __gj_kmp_find(const char *hay, const char *needle)
    __attribute__((alias("gj_kmp_find")));
