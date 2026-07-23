/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch791: left-strip whitespace over a bounded span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_lstrip_n(const char *s, size_t n);
 *     — Return the offset of the first non-whitespace byte in s[0..n),
 *       or n if the whole span is whitespace / empty / s is NULL.
 *       Whitespace: space, tab, LF, VT, FF, CR (0x09..0x0D, 0x20).
 *   size_t __gj_str_lstrip_n  (alias)
 *   __libcgj_batch791_marker = "libcgj-batch791"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch791_marker[] = "libcgj-batch791";

/* ---- helpers ----------------------------------------------------------- */

static int
b791_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_lstrip_n — offset of first non-ws byte in s[0..n).
 *
 * s: buffer of n bytes (NULL treated as empty → return n, which is 0
 *    when n == 0; when s is NULL and n > 0, return n for safety)
 * n: length of span
 *
 * Returns index in [0, n].
 */
size_t
gj_str_lstrip_n(const char *sz, size_t cb)
{
	size_t i;

	if (sz == NULL) {
		return cb;
	}

	i = 0u;
	while (i < cb && b791_is_ws((unsigned char)sz[i])) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_lstrip_n(const char *sz, size_t cb)
    __attribute__((alias("gj_str_lstrip_n")));
