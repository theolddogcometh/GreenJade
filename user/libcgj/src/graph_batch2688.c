/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2688: next UTF-8 code-unit boundary (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_next_boundary_u(const unsigned char *s, size_t n,
 *                                  size_t i);
 *     - Return the least index j with j >= min(i, n) that is a
 *       code-unit boundary in s[0..n): j == n, or j < n and s[j] is
 *       not a continuation byte (10xxxxxx). Structural only.
 *       Returns 0 if s is NULL and n > 0 (and i is ignored for the
 *       NULL case when n > 0 yields 0).
 *   size_t __gj_utf8_next_boundary_u  (alias)
 *   __libcgj_batch2688_marker = "libcgj-batch2688"
 *
 * UTF-8 exclusive wave (2681-2690). Pairs with gj_utf8_prev_boundary_u
 * (batch2687). Unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2688_marker[] = "libcgj-batch2688";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff octet is a UTF-8 continuation (10xxxxxx). */
static int
b2688_is_cont(unsigned char u8B)
{
	return ((u8B & 0xC0u) == 0x80u) ? 1 : 0;
}

/*
 * Least boundary index j >= min(i, n).
 * p must be non-NULL when n > 0.
 */
static size_t
b2688_next(const unsigned char *p, size_t n, size_t i)
{
	if (i >= n) {
		return n;
	}
	while (i < n && b2688_is_cont(p[i])) {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_next_boundary_u - least UTF-8 boundary index at or after i.
 *
 * s: buffer (NULL with n > 0 -> 0)
 * n: buffer length
 * i: probe index (clamped: i >= n yields n)
 *
 * A boundary is any index where a new sequence may start (not mid
 * continuation), or the end of the buffer. Does not call libc.
 */
size_t
gj_utf8_next_boundary_u(const unsigned char *s, size_t n, size_t i)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2688_next(s, n, i);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_utf8_next_boundary_u(const unsigned char *s, size_t n, size_t i)
    __attribute__((alias("gj_utf8_next_boundary_u")));
