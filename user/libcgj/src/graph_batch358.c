/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch358: in-place single-character replace on a
 * NUL-terminated C string.
 *
 * Surface (unique symbols):
 *   size_t gj_replace_char(char *s, char from, char to);
 *     — Walk s until '\0'; replace every octet equal to from with to.
 *       Return the number of replacements performed. NULL s → 0.
 *       Safe when from == to (counts matches; string bytes unchanged).
 *   size_t __gj_replace_char  (alias)
 *   __libcgj_batch358_marker = "libcgj-batch358"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch358_marker[] = "libcgj-batch358";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_replace_char — replace all occurrences of from with to in s.
 *
 * s:    mutable NUL-terminated string (NULL → return 0)
 * from: character value to match
 * to:   replacement character value
 *
 * Returns the count of positions where from was found (and written as to).
 */
size_t
gj_replace_char(char *s, char from, char to)
{
	size_t cRepl;
	char *pCh;

	cRepl = 0u;
	if (s == NULL) {
		return 0u;
	}

	pCh = s;
	while (*pCh != '\0') {
		if (*pCh == from) {
			*pCh = to;
			cRepl++;
		}
		pCh++;
	}
	return cRepl;
}

size_t __gj_replace_char(char *s, char from, char to)
    __attribute__((alias("gj_replace_char")));
