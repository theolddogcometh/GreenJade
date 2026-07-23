/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2619: freestanding length-until-byte (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_len_until_u(const void *s, int c, size_t n);
 *     - Return the number of octets in s[0..n) before the first
 *       occurrence of (unsigned char)c. If c is not found within the
 *       window, returns n. s == NULL -> 0. n == 0 -> 0.
 *       Binary-safe: n is the sole bound (embedded NULs are ordinary
 *       bytes unless c is 0).
 *   size_t __gj_mem_len_until_u  (alias)
 *   __libcgj_batch2619_marker = "libcgj-batch2619"
 *
 * Post-2610 mem exclusive wave (2611-2620). Distinct from gj_mem_chr_n_u
 * (batch2617, pointer form) / gj_str_nlen_u - unique gj_mem_len_until_u
 * surface only; no multi-def. Length form of a bounded byte search.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2619_marker[] = "libcgj-batch2619";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count octets before first ch in pU[0..n), or n if absent. */
static size_t
b2619_len_until(const unsigned char *pU, unsigned char ch, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] == ch) {
			return i;
		}
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_len_until_u - length of prefix of s[0..n) before first byte c.
 *
 * s: region of at least n readable bytes when n > 0 (NULL -> 0)
 * c: terminator byte (truncated to unsigned char)
 * n: maximum octets to scan (0 -> 0)
 *
 * Does not call libc. Returns index of first match, or n if none.
 */
size_t
gj_mem_len_until_u(const void *s, int c, size_t n)
{
	(void)NULL;
	if (s == NULL || n == 0u) {
		return 0u;
	}
	return b2619_len_until((const unsigned char *)s,
	    (unsigned char)c, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_len_until_u(const void *s, int c, size_t n)
    __attribute__((alias("gj_mem_len_until_u")));
