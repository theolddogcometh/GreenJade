/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2957: in-place trailing ASCII whitespace trim.
 *
 * Surface (unique symbols):
 *   char *gj_str_trim_right_u(char *s);
 *     - Trim trailing space / tab / CR / LF from the NUL-terminated
 *       string at s, in place, by writing a new NUL after the last
 *       non-trim character. Leading content is not moved. Empty /
 *       all-trim content yields an empty string at s. Returns s.
 *       s == NULL → NULL.
 *   char *__gj_str_trim_right_u  (alias)
 *   __libcgj_batch2957_marker = "libcgj-batch2957"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_str_rtrim_len (batch606, length-only) and gj_str_trim (batch357,
 * L+R) — unique gj_str_trim_right_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2957_marker[] = "libcgj-batch2957";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is a trailing trim target: space, tab, CR, or LF. */
static int
b2957_is_trim(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

/* Byte length of a NUL-terminated string (sz must be non-NULL). */
static size_t
b2957_len(const char *sz)
{
	size_t cb;

	cb = 0u;
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static char *
b2957_trim_right(char *s)
{
	size_t cb;
	size_t n;

	if (s == NULL) {
		return NULL;
	}

	cb = b2957_len(s);
	n = cb;
	while (n > 0u && b2957_is_trim((unsigned char)s[n - 1u])) {
		n--;
	}
	s[n] = '\0';
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_right_u - in-place trailing space/tab/CR/LF trim.
 *
 * s: mutable NUL-terminated C string; NULL → NULL.
 *
 * Walks backward from the last character while octets are space/tab/
 * CR/LF, then writes a new NUL. Returns s (or NULL). Does not call
 * libc isspace / strlen.
 */
char *
gj_str_trim_right_u(char *s)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2957_trim_right(s);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_trim_right_u(char *s)
    __attribute__((alias("gj_str_trim_right_u")));
