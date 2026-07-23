/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch357: in-place ASCII string trim.
 *
 * Surface (unique symbols):
 *   char *gj_str_trim(char *s);
 *     — Trim leading and trailing space / tab / CR / LF from the
 *       NUL-terminated string at s, in place. Leading trim advances
 *       the returned pointer; trailing trim writes a new NUL after the
 *       last non-trim character. Empty / all-trim content yields a
 *       pointer to an empty string (NUL). NULL s returns NULL.
 *   char *__gj_str_trim  (alias)
 *   __libcgj_batch357_marker = "libcgj-batch357"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers (no strlen / isspace). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch357_marker[] = "libcgj-batch357";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is a trim target: space, tab, CR, or LF. */
static int
b357_is_trim(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim — in-place trim of leading/trailing space/tab/CR/LF.
 *
 * s: mutable NUL-terminated C string; NULL → NULL.
 *
 * Returns a pointer into s (or equal to s) at the first non-trim
 * character after leading strip, with trailing trim characters cut by
 * a written NUL. The bytes before the returned pointer are left
 * unchanged. If every character is a trim character, returns a pointer
 * to the final NUL (empty string).
 */
char *
gj_str_trim(char *s)
{
	char *szStart;
	char *szEnd;

	if (s == NULL) {
		return NULL;
	}

	/* Leading: advance past space / tab / CR / LF. */
	szStart = s;
	while (b357_is_trim(*szStart)) {
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
	while (szEnd > szStart && b357_is_trim(*szEnd)) {
		szEnd--;
	}
	szEnd[1] = '\0';

	return szStart;
}

char *__gj_str_trim(char *s) __attribute__((alias("gj_str_trim")));
