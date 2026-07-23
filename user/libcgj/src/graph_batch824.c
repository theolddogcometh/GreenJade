/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch824: ASCII-only buffer probe (utf8_is_ascii style).
 *
 * Surface (unique symbols):
 *   int gj_utf8_valid_ascii(const unsigned char *s, size_t n);
 *     — Walk s[0..n). Returns 1 if every byte is < 0x80 (ASCII / UTF-8
 *       single-byte range). Returns 0 if any byte is >= 0x80.
 *       Empty window (n == 0) is ASCII (returns 1) even when s is NULL.
 *       Returns -1 if s is NULL and n > 0.
 *   int __gj_utf8_valid_ascii  (alias)
 *   __libcgj_batch824_marker = "libcgj-batch824"
 *
 * Same semantics as gj_utf8_is_ascii (batch691) under a unique name —
 * does NOT redefine that surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch824_marker[] = "libcgj-batch824";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_valid_ascii — true when every byte in s[0..n) is ASCII (< 0x80).
 *
 * Returns:
 *   1  — all bytes < 0x80 (including empty n == 0)
 *   0  — at least one byte >= 0x80
 *  -1  — s is NULL and n > 0
 */
int
gj_utf8_valid_ascii(const unsigned char *s, size_t n)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_valid_ascii(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_valid_ascii")));
