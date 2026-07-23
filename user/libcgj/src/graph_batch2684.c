/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2684: ASCII-only n-byte window probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_ascii_n_u(const unsigned char *s, size_t n);
 *     - Walk s[0..n). Returns 1 if every byte is < 0x80 (ASCII /
 *       UTF-8 single-byte range). Returns 0 if any byte is >= 0x80,
 *       or if s is NULL and n > 0. Empty window (n == 0) is ASCII
 *       (returns 1) even when s is NULL.
 *   int __gj_utf8_is_ascii_n_u  (alias)
 *   __libcgj_batch2684_marker = "libcgj-batch2684"
 *
 * UTF-8 exclusive wave (2681-2690). Distinct from gj_utf8_is_ascii
 * (batch691, NULL+n>0 -> -1) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2684_marker[] = "libcgj-batch2684";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff every byte in p[0..n) is < 0x80. p must be non-NULL when n > 0. */
static int
b2684_is_ascii(const unsigned char *p, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (p[i] >= 0x80u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_ascii_n_u - true when every byte in s[0..n) is ASCII.
 *
 * s: buffer (may be NULL when n == 0)
 * n: window length in bytes
 *
 * Returns:
 *   1  - all bytes < 0x80 (including empty n == 0)
 *   0  - at least one byte >= 0x80, or s is NULL with n > 0
 */
int
gj_utf8_is_ascii_n_u(const unsigned char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return 0;
	}
	return b2684_is_ascii(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_ascii_n_u(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_is_ascii_n_u")));
