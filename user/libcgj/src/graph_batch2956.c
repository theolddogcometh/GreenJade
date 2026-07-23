/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2956: left-trim of ASCII whitespace (mutable).
 *
 * Surface (unique symbols):
 *   char *gj_str_trim_left_u(char *s);
 *     - Return a pointer into the NUL-terminated string s at the first
 *       character that is not space / tab / CR / LF. Leading trim only;
 *       the string is not modified. Empty / all-trim content yields a
 *       pointer to the terminating NUL. s == NULL → NULL.
 *   char *__gj_str_trim_left_u  (alias)
 *   __libcgj_batch2956_marker = "libcgj-batch2956"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_str_ltrim (batch605, const return) and gj_str_trim (batch357,
 * L+R in-place) — unique gj_str_trim_left_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2956_marker[] = "libcgj-batch2956";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is a left-trim target: space, tab, CR, or LF. */
static int
b2956_is_trim(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static char *
b2956_trim_left(char *s)
{
	char *sz;

	if (s == NULL) {
		return NULL;
	}

	sz = s;
	while (b2956_is_trim((unsigned char)*sz)) {
		sz++;
	}
	return sz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_left_u - skip leading space/tab/CR/LF; return pointer into s.
 *
 * s: mutable NUL-terminated C string, or NULL.
 *
 * Returns a pointer into s (or equal to s) at the first non-trim
 * character. Does not write through s. If every character is a trim
 * character (or s is empty), returns a pointer to the final NUL.
 * NULL s returns NULL.
 */
char *
gj_str_trim_left_u(char *s)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2956_trim_left(s);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_trim_left_u(char *s)
    __attribute__((alias("gj_str_trim_left_u")));
