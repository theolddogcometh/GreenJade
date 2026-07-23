/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2959: in-place character replacement.
 *
 * Surface (unique symbols):
 *   char *gj_str_replace_char_u(char *s, int old_ch, int new_ch);
 *     - Replace every occurrence of (unsigned char)old_ch with
 *       (unsigned char)new_ch in the NUL-terminated string s, in place.
 *       Returns s. NULL s → NULL (no write).
 *   char *__gj_str_replace_char_u  (alias)
 *   __libcgj_batch2959_marker = "libcgj-batch2959"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_str_replace_char (batch904) — unique gj_str_replace_char_u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2959_marker[] = "libcgj-batch2959";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2959_replace_char(char *s, int nOld, int nNew)
{
	unsigned char uOld;
	unsigned char uNew;
	size_t i;

	if (s == NULL) {
		return NULL;
	}

	uOld = (unsigned char)nOld;
	uNew = (unsigned char)nNew;
	for (i = 0u; s[i] != '\0'; i++) {
		if ((unsigned char)s[i] == uOld) {
			s[i] = (char)uNew;
		}
	}
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_replace_char_u - replace all old_ch with new_ch in place.
 *
 * s:      mutable NUL-terminated C string; NULL → NULL
 * old_ch: character to find (compared as unsigned char)
 * new_ch: replacement character (stored as unsigned char)
 *
 * Walks s until NUL. Does not stop if old_ch is 0 (NUL is not replaced
 * as a payload character because the scan ends at the terminator).
 */
char *
gj_str_replace_char_u(char *s, int old_ch, int new_ch)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2959_replace_char(s, old_ch, new_ch);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_replace_char_u(char *s, int old_ch, int new_ch)
    __attribute__((alias("gj_str_replace_char_u")));
