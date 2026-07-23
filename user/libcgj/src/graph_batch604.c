/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch604: freestanding C-string byte count.
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_char(const char *s, int c);
 *     — Count occurrences of byte (unsigned char)c in the NUL-terminated
 *       string s (not counting the terminating NUL unless c == 0, in
 *       which case the terminator is not counted either — only payload
 *       octets before NUL). s == NULL → 0.
 *   size_t __gj_str_count_char  (alias)
 *   __libcgj_batch604_marker = "libcgj-batch604"
 *
 * Distinct from libc helpers — unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch604_marker[] = "libcgj-batch604";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_char — count occurrences of byte c in s.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Walks s until NUL, tallying matches against (unsigned char)c.
 * Returns 0 when s is NULL. Does not call libc.
 */
size_t
gj_str_count_char(const char *s, int c)
{
	size_t n;
	unsigned char ch;
	const unsigned char *p;

	if (s == NULL) {
		return 0u;
	}

	ch = (unsigned char)c;
	n = 0u;
	p = (const unsigned char *)s;
	while (*p != '\0') {
		if (*p == ch) {
			n++;
		}
		p++;
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_char(const char *s, int c)
    __attribute__((alias("gj_str_count_char")));
