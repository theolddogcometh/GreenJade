/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2101: left-trim ASCII whitespace on a span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_ltrim_ascii_span(const char *s, size_t n);
 *     - Return the offset of the first non-whitespace octet in s[0..n),
 *       or n if the whole span is whitespace / empty / s is NULL.
 *       Whitespace: space, HT, LF, VT, FF, CR (0x09..0x0D, 0x20).
 *   size_t __gj_str_ltrim_ascii_span  (alias)
 *   __libcgj_batch2101_marker = "libcgj-batch2101"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_ltrim (batch605, NUL string) and gj_str_lstrip_n (batch791) -
 * unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2101_marker[] = "libcgj-batch2101";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2101_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

static size_t
b2101_ltrim(const char *sz, size_t cb)
{
	size_t i;

	if (sz == NULL) {
		return cb;
	}
	i = 0u;
	while (i < cb && b2101_is_ws((unsigned char)sz[i])) {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_ltrim_ascii_span - offset of first non-ws byte in s[0..n).
 *
 * s: buffer of n octets (NULL treated as empty window of length n)
 * n: length of the span
 *
 * Returns index in [0, n]. Does not write through s. Does not call libc.
 */
size_t
gj_str_ltrim_ascii_span(const char *s, size_t n)
{
	return b2101_ltrim(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_ltrim_ascii_span(const char *s, size_t n)
    __attribute__((alias("gj_str_ltrim_ascii_span")));
