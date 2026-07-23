/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2006: bounded byte-count of a character.
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_char_n(const char *s, size_t n, int c);
 *     — Count occurrences of byte (unsigned char)c in s[0..n). Binary-
 *       safe: does not stop at embedded NULs; n is the sole bound.
 *       NULL s with n > 0 → 0; n == 0 → 0.
 *   size_t __gj_str_count_char_n  (alias)
 *   __libcgj_batch2006_marker = "libcgj-batch2006"
 *
 * Distinct from gj_str_count_char (batch604 NUL-terminated form) —
 * avoid multi-def. Post-2000 continuum str helpers exclusive wave
 * (2001–2010). Clean-room freestanding pure C (integer/pointer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc,
 * no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2006_marker[] = "libcgj-batch2006";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2006_count(const char *sz, size_t n, int nC)
{
	size_t i;
	size_t nHit;
	unsigned char ch;
	const unsigned char *p;

	if (n == 0u || sz == NULL) {
		return 0u;
	}

	ch = (unsigned char)nC;
	nHit = 0u;
	p = (const unsigned char *)sz;
	for (i = 0u; i < n; i++) {
		if (p[i] == ch) {
			nHit++;
		}
	}
	return nHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_char_n — count byte c in s[0..n).
 *
 * s: base (NULL with n > 0 → 0)
 * n: byte count to scan
 * c: search byte (truncated to unsigned char)
 *
 * Binary-safe bounded scan; does not call libc.
 */
size_t
gj_str_count_char_n(const char *s, size_t n, int c)
{
	(void)NULL;
	return b2006_count(s, n, c);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_char_n(const char *s, size_t n, int c)
    __attribute__((alias("gj_str_count_char_n")));
