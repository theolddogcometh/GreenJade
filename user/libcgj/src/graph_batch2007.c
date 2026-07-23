/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2007: bounded first-index of a character.
 *
 * Surface (unique symbols):
 *   size_t gj_str_index_char_n(const char *s, size_t n, int c);
 *     — Return the first index i in [0, n) where s[i] equals
 *       (unsigned char)c, or n if not found. Binary-safe: n is the
 *       sole bound (embedded NULs are ordinary bytes). NULL s with
 *       n > 0 → n (not found); n == 0 → 0.
 *   size_t __gj_str_index_char_n  (alias)
 *   __libcgj_batch2007_marker = "libcgj-batch2007"
 *
 * Distinct from gj_str_chr (batch415 pointer form) — avoid multi-def.
 * Post-2000 continuum str helpers exclusive wave (2001–2010). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2007_marker[] = "libcgj-batch2007";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2007_index(const char *sz, size_t n, int nC)
{
	size_t i;
	unsigned char ch;
	const unsigned char *p;

	if (n == 0u) {
		return 0u;
	}
	if (sz == NULL) {
		return n;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)sz;
	for (i = 0u; i < n; i++) {
		if (p[i] == ch) {
			return i;
		}
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_index_char_n — first index of byte c in s[0..n), or n.
 *
 * s: base (NULL with n > 0 → return n)
 * n: byte count to scan
 * c: search byte (truncated to unsigned char)
 *
 * Binary-safe bounded scan; does not call libc.
 */
size_t
gj_str_index_char_n(const char *s, size_t n, int c)
{
	(void)NULL;
	return b2007_index(s, n, c);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_index_char_n(const char *s, size_t n, int c)
    __attribute__((alias("gj_str_index_char_n")));
