/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2685: count ASCII bytes in an n-byte window (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_count_ascii_n_u(const unsigned char *s, size_t n);
 *     - Count how many octets in s[0..n) are ASCII (< 0x80). Returns 0
 *       if s is NULL and n > 0. Empty window yields 0.
 *   size_t __gj_utf8_count_ascii_n_u  (alias)
 *   __libcgj_batch2685_marker = "libcgj-batch2685"
 *
 * UTF-8 exclusive wave (2681-2690). Distinct from gj_utf8_is_ascii_n_u
 * (batch2684 boolean probe) and gj_utf8_is_ascii (batch691) — unique
 * count surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2685_marker[] = "libcgj-batch2685";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count bytes < 0x80 in p[0..n). p must be non-NULL when n > 0. */
static size_t
b2685_count(const unsigned char *p, size_t n)
{
	size_t i;
	size_t uCnt;

	uCnt = 0u;
	for (i = 0u; i < n; i++) {
		if (p[i] < 0x80u) {
			uCnt++;
		}
	}
	return uCnt;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_count_ascii_n_u - number of ASCII octets in s[0..n).
 *
 * s: buffer (NULL with n > 0 -> 0)
 * n: window length in bytes
 *
 * Returns the count of bytes with value < 0x80. Does not stop early
 * at the first non-ASCII byte (full-window tally).
 */
size_t
gj_utf8_count_ascii_n_u(const unsigned char *s, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (s == NULL) {
		return 0u;
	}
	return b2685_count(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_utf8_count_ascii_n_u(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_count_ascii_n_u")));
