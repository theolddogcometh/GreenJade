/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch904: in-place character replacement.
 *
 * Surface (unique symbols):
 *   char *gj_str_replace_char(char *s, int old_ch, int new_ch);
 *     — Replace every occurrence of (unsigned char)old_ch with
 *       (unsigned char)new_ch in the NUL-terminated string s, in place.
 *       Returns s. NULL s → NULL (no write).
 *   char *__gj_str_replace_char  (alias)
 *   __libcgj_batch904_marker = "libcgj-batch904"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch904_marker[] = "libcgj-batch904";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_replace_char — replace all old_ch with new_ch in place.
 *
 * s:      mutable NUL-terminated C string; NULL → NULL
 * old_ch: character to find (compared as unsigned char)
 * new_ch: replacement character (stored as unsigned char)
 *
 * Walks s until NUL. Does not stop if old_ch is 0 (NUL is not replaced
 * as a payload character because the scan ends at the terminator).
 */
char *
gj_str_replace_char(char *s, int old_ch, int new_ch)
{
	unsigned char uOld;
	unsigned char uNew;
	size_t i;

	if (s == NULL) {
		return NULL;
	}

	uOld = (unsigned char)old_ch;
	uNew = (unsigned char)new_ch;
	for (i = 0u; s[i] != '\0'; i++) {
		if ((unsigned char)s[i] == uOld) {
			s[i] = (char)uNew;
		}
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_replace_char(char *s, int old_ch, int new_ch)
    __attribute__((alias("gj_str_replace_char")));
