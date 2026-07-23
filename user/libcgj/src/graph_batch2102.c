/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2102: right-trim ASCII whitespace on a span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_rtrim_ascii_span(const char *s, size_t n);
 *     - Return the exclusive end index of s[0..n) after stripping
 *       trailing ASCII whitespace (i.e. length of the right-trimmed
 *       prefix). Whitespace: space, HT, LF, VT, FF, CR (0x09..0x0D,
 *       0x20). s == NULL or n == 0 -> 0. All-ws -> 0.
 *   size_t __gj_str_rtrim_ascii_span  (alias)
 *   __libcgj_batch2102_marker = "libcgj-batch2102"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_rtrim_len (batch606) and gj_str_rstrip_n (batch792) - unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2102_marker[] = "libcgj-batch2102";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2102_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

static size_t
b2102_rtrim(const char *sz, size_t cb)
{
	size_t iEnd;

	if (sz == NULL || cb == 0u) {
		return 0u;
	}
	iEnd = cb;
	while (iEnd > 0u && b2102_is_ws((unsigned char)sz[iEnd - 1u])) {
		iEnd--;
	}
	return iEnd;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rtrim_ascii_span - exclusive end after trailing ws strip on span.
 *
 * s: buffer of n octets (NULL -> 0)
 * n: length of the span
 *
 * Returns length in [0, n]. Does not write through s. Does not call libc.
 */
size_t
gj_str_rtrim_ascii_span(const char *s, size_t n)
{
	return b2102_rtrim(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_rtrim_ascii_span(const char *s, size_t n)
    __attribute__((alias("gj_str_rtrim_ascii_span")));
