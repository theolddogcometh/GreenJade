/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch605: left-trim of ASCII whitespace (read-only).
 *
 * Surface (unique symbols):
 *   const char *gj_str_ltrim(const char *s);
 *     — Return a pointer into the NUL-terminated string s at the first
 *       character that is not space / tab / CR / LF. Leading trim only;
 *       the string is not modified. Empty / all-trim content yields a
 *       pointer to the terminating NUL. s == NULL → NULL.
 *   const char *__gj_str_ltrim  (alias)
 *   __libcgj_batch605_marker = "libcgj-batch605"
 *
 * Distinct from gj_str_trim (batch357, in-place L+R trim on char *).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers (no strlen / isspace). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch605_marker[] = "libcgj-batch605";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is a left-trim target: space, tab, CR, or LF. */
static int
b605_is_trim(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ltrim — skip leading space/tab/CR/LF; return pointer into s.
 *
 * s: NUL-terminated C string, or NULL.
 *
 * Returns a pointer into s (or equal to s) at the first non-trim
 * character. Does not write through s. If every character is a trim
 * character (or s is empty), returns a pointer to the final NUL.
 * NULL s returns NULL.
 */
const char *
gj_str_ltrim(const char *s)
{
	const char *sz;

	if (s == NULL) {
		return NULL;
	}

	sz = s;
	while (b605_is_trim(*sz)) {
		sz++;
	}

	return sz;
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_str_ltrim(const char *s)
    __attribute__((alias("gj_str_ltrim")));
