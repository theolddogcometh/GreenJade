/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch150: simple shell-style glob match (* and ?).
 * Clean-room freestanding pure C (integer/pointer; builds with -msse2).
 * No third-party glob / fnmatch source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   fnmatch / FNM_*  → fnmatch.c + include/fnmatch.h
 *   glob / globfree  → glob.c
 *   gj_re_match / gj_re_fullmatch / gj_re_search → graph_batch80.c
 *
 * This TU adds only unique symbols:
 *   int gj_glob_match(const char *pat, const char *str);
 *     — whole-string match; supports '*' (any run) and '?' (one char)
 *       only. Literals are byte-exact (no casefold, no FNM_PATHNAME,
 *       no escapes, no bracket classes). Returns 1 on match, 0 otherwise
 *       (including NULL args).
 *   __gj_glob_match  (alias)
 *   __libcgj_batch150_marker = "libcgj-batch150"
 */

#include <stddef.h>

const char __libcgj_batch150_marker[] = "libcgj-batch150";

/*
 * Recursive * / ? matcher. '*' may span any run of bytes (including
 * empty); consecutive '*' collapse. '?' matches exactly one byte.
 * Both ends of the string must be consumed for success.
 */
static int
b150_match(const char *szPat, const char *szStr)
{
	while (*szPat != '\0') {
		if (*szPat == '*') {
			while (*szPat == '*') {
				szPat++;
			}
			if (*szPat == '\0') {
				return 1;
			}
			while (*szStr != '\0') {
				if (b150_match(szPat, szStr) != 0) {
					return 1;
				}
				szStr++;
			}
			return 0;
		}
		if (*szPat == '?') {
			if (*szStr == '\0') {
				return 0;
			}
			szPat++;
			szStr++;
			continue;
		}
		if (*szStr == '\0' || *szPat != *szStr) {
			return 0;
		}
		szPat++;
		szStr++;
	}
	return (*szStr == '\0') ? 1 : 0;
}

/*
 * gj_glob_match — simple whole-string glob (* and ? only).
 * Returns 1 on match, 0 on no match or NULL argument.
 */
int
gj_glob_match(const char *szPat, const char *szStr)
{
	if (szPat == NULL || szStr == NULL) {
		return 0;
	}
	return b150_match(szPat, szStr);
}

int __gj_glob_match(const char *szPat, const char *szStr)
    __attribute__((alias("gj_glob_match")));
