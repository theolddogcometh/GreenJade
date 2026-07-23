/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3631: count-limited in-place character replace.
 *
 * Surface (unique symbols):
 *   char *gj_str_replace_n_u(char *s, int old_ch, int new_ch, size_t n);
 *     - Replace at most n occurrences of (unsigned char)old_ch with
 *       (unsigned char)new_ch in the NUL-terminated string s, in place,
 *       left-to-right. Returns s. NULL s → NULL (no write). n == 0 →
 *       no-op (returns s).
 *   char *__gj_str_replace_n_u  (alias)
 *   __libcgj_batch3631_marker = "libcgj-batch3631"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_replace_char (batch904) and gj_str_replace_char_u (batch2959)
 * which replace all occurrences — unique count-limited surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3631_marker[] = "libcgj-batch3631";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b3631_replace_n(char *s, int nOld, int nNew, size_t cMax)
{
	unsigned char uOld;
	unsigned char uNew;
	size_t i;
	size_t cDone;

	if (s == NULL) {
		return NULL;
	}
	if (cMax == 0u) {
		return s;
	}

	uOld = (unsigned char)nOld;
	uNew = (unsigned char)nNew;
	cDone = 0u;
	for (i = 0u; s[i] != '\0'; i++) {
		if ((unsigned char)s[i] == uOld) {
			s[i] = (char)uNew;
			cDone++;
			if (cDone >= cMax) {
				break;
			}
		}
	}
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_replace_n_u - replace at most n occurrences of old_ch in place.
 *
 * s:      mutable NUL-terminated C string; NULL → NULL
 * old_ch: character to find (compared as unsigned char)
 * new_ch: replacement character (stored as unsigned char)
 * n:      maximum number of replacements (0 → no-op)
 *
 * Walks s until NUL or n replacements. Does not replace the terminator.
 * No parent wires.
 */
char *
gj_str_replace_n_u(char *s, int old_ch, int new_ch, size_t n)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3631_replace_n(s, old_ch, new_ch, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_replace_n_u(char *s, int old_ch, int new_ch, size_t n)
    __attribute__((alias("gj_str_replace_n_u")));
