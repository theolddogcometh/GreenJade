/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch509: simple shell-style glob match (* and ?).
 * Clean-room freestanding pure C (integer/pointer; builds with -msse2).
 * No third-party glob / fnmatch source was copied.
 *
 * Surface (unique names — batch150 already owns gj_glob_match, so this TU
 * uses gj_fnmatch_simple):
 *   int gj_fnmatch_simple(const char *pat, const char *s);
 *     — whole-string match; supports '*' (any run) and '?' (one char)
 *       only. Literals are byte-exact (no casefold, no FNM_PATHNAME,
 *       no escapes, no bracket classes). Returns 1 on match, 0 otherwise
 *       (including NULL args).
 *   __gj_fnmatch_simple  (alias)
 *   __libcgj_batch509_marker = "libcgj-batch509"
 *
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc calls.
 */

#include <stddef.h>

const char __libcgj_batch509_marker[] = "libcgj-batch509";

/*
 * Recursive * / ? matcher. '*' may span any run of bytes (including
 * empty); consecutive '*' collapse. '?' matches exactly one byte.
 * Both ends of the string must be consumed for success.
 */
static int
b509_match(const char *szPat, const char *szS)
{
	while (*szPat != '\0') {
		if (*szPat == '*') {
			while (*szPat == '*') {
				szPat++;
			}
			if (*szPat == '\0') {
				return 1;
			}
			while (*szS != '\0') {
				if (b509_match(szPat, szS) != 0) {
					return 1;
				}
				szS++;
			}
			return 0;
		}
		if (*szPat == '?') {
			if (*szS == '\0') {
				return 0;
			}
			szPat++;
			szS++;
			continue;
		}
		if (*szS == '\0' || *szPat != *szS) {
			return 0;
		}
		szPat++;
		szS++;
	}
	return (*szS == '\0') ? 1 : 0;
}

/*
 * gj_fnmatch_simple — simple whole-string glob (* and ? only).
 * Returns 1 on match, 0 on no match or NULL argument.
 */
int
gj_fnmatch_simple(const char *szPat, const char *szS)
{
	if (szPat == NULL || szS == NULL) {
		return 0;
	}
	return b509_match(szPat, szS);
}

int __gj_fnmatch_simple(const char *szPat, const char *szS)
    __attribute__((alias("gj_fnmatch_simple")));
