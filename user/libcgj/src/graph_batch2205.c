/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2205: bounded first character search (pointer form).
 *
 * Surface (unique symbols):
 *   char *gj_str_chr_n_u(const char *s, int c, size_t n);
 *     - Search s[0..n) for the first octet equal to (unsigned char)c.
 *       Binary-safe: n is the sole bound (embedded NULs are ordinary
 *       bytes). Returns a pointer into s on hit, or NULL if not found,
 *       if s is NULL with n > 0, or if n == 0.
 *   char *__gj_str_chr_n_u  (alias)
 *   __libcgj_batch2205_marker = "libcgj-batch2205"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_chr
 * (batch415 NUL-string) and gj_str_index_char_n (batch2007 index form)
 * - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2205_marker[] = "libcgj-batch2205";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2205_chr_n(const char *sz, int nC, size_t n)
{
	size_t i;
	unsigned char ch;
	const unsigned char *p;

	if (sz == NULL || n == 0u) {
		return NULL;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)sz;
	for (i = 0u; i < n; i++) {
		if (p[i] == ch) {
			return (char *)(void *)(sz + i);
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_chr_n_u - first pointer to byte c in s[0..n), or NULL.
 *
 * s: base (NULL → NULL)
 * c: search byte (truncated to unsigned char)
 * n: byte count to scan
 *
 * Binary-safe bounded scan; does not call libc strchr.
 */
char *
gj_str_chr_n_u(const char *s, int c, size_t n)
{
	(void)NULL;
	return b2205_chr_n(s, c, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_chr_n_u(const char *s, int c, size_t n)
    __attribute__((alias("gj_str_chr_n_u")));
