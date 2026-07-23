/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch691: ASCII-only buffer probe (UTF-8 1-byte range).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_ascii(const unsigned char *s, size_t n);
 *     — Walk s[0..n). Returns 1 if every byte is < 0x80 (ASCII / UTF-8
 *       single-byte range U+0000..U+007F). Returns 0 if any byte is
 *       >= 0x80. Empty window (n == 0) is ASCII (returns 1) even when
 *       s is NULL. Returns -1 if s is NULL and n > 0.
 *   int __gj_utf8_is_ascii  (alias)
 *   __libcgj_batch691_marker = "libcgj-batch691"
 *
 * Distinct from batch259 gj_utf8_validate (full UTF-8 well-formedness),
 * batch502 decode1, batch503 encode1, and batch504 scrub. This only
 * checks the high bit of each byte — no multi-byte structure.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch691_marker[] = "libcgj-batch691";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_ascii — true when every byte in s[0..n) is ASCII (< 0x80).
 *
 * Returns:
 *   1  — all bytes < 0x80 (including empty n == 0)
 *   0  — at least one byte >= 0x80
 *  -1  — s is NULL and n > 0
 */
int
gj_utf8_is_ascii(const unsigned char *s, size_t n)
{
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		if (s[i] >= 0x80u) {
			return 0;
		}
	}
	return 1;
}

int __gj_utf8_is_ascii(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_is_ascii")));
