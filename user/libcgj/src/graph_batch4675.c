/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4675: freestanding C-string byte count
 * (plural measure surface).
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_chars_u(const char *s, int c);
 *     - Count occurrences of byte (unsigned char)c in the NUL-terminated
 *       string s (payload octets before NUL only; terminator never
 *       counted). s == NULL → 0.
 *   size_t __gj_str_count_chars_u  (alias)
 *   __libcgj_batch4675_marker = "libcgj-batch4675"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_count_char_u (batch2958 singular) and gj_str_count_char
 * (batch604) — unique gj_str_count_chars_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4675_marker[] = "libcgj-batch4675";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4675_count_chars(const char *s, int nC)
{
	size_t n;
	unsigned char ch;
	const unsigned char *p;

	if (s == NULL) {
		return 0u;
	}

	ch = (unsigned char)nC;
	n = 0u;
	p = (const unsigned char *)(const void *)s;
	while (*p != (unsigned char)'\0') {
		if (*p == ch) {
			n++;
		}
		p++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_chars_u - count occurrences of byte c in s.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Walks s until NUL, tallying matches against (unsigned char)c.
 * Returns 0 when s is NULL. Does not call libc. No parent wires.
 */
size_t
gj_str_count_chars_u(const char *s, int c)
{
	(void)NULL;
	return b4675_count_chars(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_chars_u(const char *s, int c)
    __attribute__((alias("gj_str_count_chars_u")));
