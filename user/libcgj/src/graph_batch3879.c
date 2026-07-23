/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3879: in-place L+R ASCII whitespace trim.
 *
 * Surface (unique symbols):
 *   char *gj_str_trim_both_u(char *s);
 *     - Trim leading and trailing space / tab / CR / LF from the
 *       NUL-terminated string at s, in place. Leading trim advances
 *       the returned pointer; trailing trim writes a new NUL after the
 *       last non-trim character. Empty / all-trim content yields a
 *       pointer to an empty string (NUL). NULL s returns NULL.
 *   char *__gj_str_trim_both_u  (alias)
 *   __libcgj_batch3879_marker = "libcgj-batch3879"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Distinct from
 * gj_str_trim (batch357), gj_str_trim_left_u (batch2956), and
 * gj_str_trim_right_u (batch2957) — unique gj_str_trim_both_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3879_marker[] = "libcgj-batch3879";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is a trim target: space, tab, CR, or LF. */
static int
b3879_is_trim(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static char *
b3879_trim_both(char *s)
{
	char *szStart;
	char *szEnd;

	if (s == NULL) {
		return NULL;
	}

	/* Leading: advance past space / tab / CR / LF. */
	szStart = s;
	while (b3879_is_trim((unsigned char)*szStart)) {
		szStart++;
	}

	/* Empty after leading trim. */
	if (*szStart == '\0') {
		return szStart;
	}

	/* Find last character (one before the existing NUL). */
	szEnd = szStart;
	while (szEnd[1] != '\0') {
		szEnd++;
	}

	/* Trailing: walk back over trim characters; write new NUL. */
	while (szEnd > szStart && b3879_is_trim((unsigned char)*szEnd)) {
		szEnd--;
	}
	szEnd[1] = '\0';

	return szStart;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_both_u - in-place trim of leading/trailing space/tab/CR/LF.
 *
 * s: mutable NUL-terminated C string; NULL → NULL.
 *
 * Returns a pointer into s (or equal to s) at the first non-trim
 * character after leading strip, with trailing trim characters cut by
 * a written NUL. If every character is a trim character, returns a
 * pointer to the final NUL (empty string). Does not call libc.
 * No parent wires.
 */
char *
gj_str_trim_both_u(char *s)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3879_trim_both(s);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_trim_both_u(char *s)
    __attribute__((alias("gj_str_trim_both_u")));
