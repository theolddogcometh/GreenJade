/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2206: bounded last character search (pointer form).
 *
 * Surface (unique symbols):
 *   char *gj_str_rchr_n_u(const char *s, int c, size_t n);
 *     - Search s[0..n) for the last octet equal to (unsigned char)c.
 *       Binary-safe: n is the sole bound. Returns a pointer into s on
 *       hit, or NULL if not found, if s is NULL with n > 0, or if n == 0.
 *   char *__gj_str_rchr_n_u  (alias)
 *   __libcgj_batch2206_marker = "libcgj-batch2206"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_chr
 * (batch415) and gj_str_rindex_char_n (batch2008 index form) - unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2206_marker[] = "libcgj-batch2206";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2206_rchr_n(const char *sz, int nC, size_t n)
{
	size_t i;
	unsigned char ch;
	const unsigned char *p;

	if (sz == NULL || n == 0u) {
		return NULL;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)sz;
	i = n;
	while (i > 0u) {
		i--;
		if (p[i] == ch) {
			return (char *)(void *)(sz + i);
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rchr_n_u - last pointer to byte c in s[0..n), or NULL.
 *
 * s: base (NULL → NULL)
 * c: search byte (truncated to unsigned char)
 * n: byte count to scan
 *
 * Binary-safe reverse scan; does not call libc strrchr.
 */
char *
gj_str_rchr_n_u(const char *s, int c, size_t n)
{
	(void)NULL;
	return b2206_rchr_n(s, c, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_rchr_n_u(const char *s, int c, size_t n)
    __attribute__((alias("gj_str_rchr_n_u")));
