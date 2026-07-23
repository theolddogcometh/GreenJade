/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3784: odd-length palindrome at center (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_palindrome_odd_u(const unsigned char *s, size_t n,
 *                              size_t center);
 *     - Expand around single center index: return the length of the
 *       longest odd-length palindromic substring centered at center
 *       (always odd: 1, 3, 5, ...). Single character → 1. NULL s,
 *       n == 0, or center >= n → 0.
 *   size_t __gj_palindrome_odd_u  (alias)
 *   __libcgj_batch3784_marker = "libcgj-batch3784"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_longest_palindrome_len (batch215 whole-string max) — single-center
 * odd radius only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3784_marker[] = "libcgj-batch3784";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_palindrome_odd_u — odd palindrome length centered at one index.
 *
 * Does not allocate. No parent wires.
 */
size_t
gj_palindrome_odd_u(const unsigned char *s, size_t n, size_t center)
{
	size_t l;
	size_t r;

	(void)NULL;
	if (s == NULL || n == 0u || center >= n) {
		return 0u;
	}

	l = center;
	r = center;
	while (l > 0u && r + 1u < n && s[l - 1u] == s[r + 1u]) {
		l--;
		r++;
	}
	/* length = r - l + 1 */
	return (r - l) + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_palindrome_odd_u(const unsigned char *s, size_t n,
                             size_t center)
    __attribute__((alias("gj_palindrome_odd_u")));
