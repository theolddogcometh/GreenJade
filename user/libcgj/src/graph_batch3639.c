/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3639: strip one pair of matching end quotes.
 *
 * Surface (unique symbols):
 *   char *gj_str_strip_quotes_u(char *s);
 *     - If s is non-NULL, length >= 2, and begins and ends with the same
 *       quote character (' or "), remove that pair in place by shifting
 *       the interior left one octet and writing a new NUL. Otherwise
 *       leave s unchanged. Returns s (or NULL if s is NULL).
 *   char *__gj_str_strip_quotes_u  (alias)
 *   __libcgj_batch3639_marker = "libcgj-batch3639"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_strip_cr / trim helpers — unique quote-pair surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3639_marker[] = "libcgj-batch3639";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3639_len(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b3639_is_quote(unsigned char u)
{
	if (u == (unsigned char)'"' || u == (unsigned char)'\'') {
		return 1;
	}
	return 0;
}

static char *
b3639_strip_quotes(char *s)
{
	size_t cb;
	size_t i;
	unsigned char uOpen;
	unsigned char uClose;

	if (s == NULL) {
		return NULL;
	}

	cb = b3639_len(s);
	if (cb < 2u) {
		return s;
	}

	uOpen = (unsigned char)s[0];
	uClose = (unsigned char)s[cb - 1u];
	if (b3639_is_quote(uOpen) == 0 || uOpen != uClose) {
		return s;
	}

	/* Shift interior left by one; drop both quotes. */
	for (i = 0u; i + 2u < cb; i++) {
		s[i] = s[i + 1u];
	}
	s[cb - 2u] = '\0';
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_strip_quotes_u - strip one matching surrounding quote pair.
 *
 * s: mutable NUL-terminated C string; NULL → NULL
 *
 * Only strips when both ends are the same ' or " character and the
 * string is at least two octets long. Nested or mixed quotes are not
 * parsed. Does not allocate. No parent wires.
 *
 * Examples:
 *   "\"hi\""  → hi
 *   "'x'"     → x
 *   "\"a'"    → unchanged (mismatched)
 *   "hi"      → unchanged (no surrounding quotes)
 */
char *
gj_str_strip_quotes_u(char *s)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3639_strip_quotes(s);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_strip_quotes_u(char *s)
    __attribute__((alias("gj_str_strip_quotes_u")));
