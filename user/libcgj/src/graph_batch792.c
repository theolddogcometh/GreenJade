/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch792: right-strip whitespace over a bounded span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_rstrip_n(const char *s, size_t n);
 *     — Return the length of s[0..n) after stripping trailing whitespace.
 *       Whitespace: space, tab, LF, VT, FF, CR (0x09..0x0D, 0x20).
 *       s == NULL → 0. All-ws / empty → 0.
 *   size_t __gj_str_rstrip_n  (alias)
 *   __libcgj_batch792_marker = "libcgj-batch792"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch792_marker[] = "libcgj-batch792";

/* ---- helpers ----------------------------------------------------------- */

static int
b792_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rstrip_n — length after stripping trailing ws from s[0..n).
 *
 * s: buffer of n bytes (NULL → 0)
 * n: length of span
 *
 * Returns length in [0, n].
 */
size_t
gj_str_rstrip_n(const char *sz, size_t cb)
{
	size_t iEnd;

	if (sz == NULL || cb == 0u) {
		return 0u;
	}

	iEnd = cb;
	while (iEnd > 0u && b792_is_ws((unsigned char)sz[iEnd - 1u])) {
		iEnd--;
	}
	return iEnd;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_rstrip_n(const char *sz, size_t cb)
    __attribute__((alias("gj_str_rstrip_n")));
