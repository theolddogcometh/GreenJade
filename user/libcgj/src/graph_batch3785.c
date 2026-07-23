/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3785: even-length palindrome at gap (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_palindrome_even_u(const unsigned char *s, size_t n,
 *                               size_t left);
 *     - Expand around the even-length gap between left and left+1:
 *       return the length of the longest even palindromic substring
 *       with those two centers (0, 2, 4, ...). If left+1 is OOB or
 *       s[left] != s[left+1], returns 0. NULL s, n < 2, or left >= n
 *       → 0.
 *   size_t __gj_palindrome_even_u  (alias)
 *   __libcgj_batch3785_marker = "libcgj-batch3785"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from
 * gj_palindrome_odd_u / gj_longest_palindrome_len — even-center only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3785_marker[] = "libcgj-batch3785";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_palindrome_even_u — even palindrome length around (left, left+1).
 *
 * Does not allocate. No parent wires.
 */
size_t
gj_palindrome_even_u(const unsigned char *s, size_t n, size_t left)
{
	size_t l;
	size_t r;

	(void)NULL;
	if (s == NULL || n < 2u || left >= n || left + 1u >= n) {
		return 0u;
	}
	if (s[left] != s[left + 1u]) {
		return 0u;
	}

	l = left;
	r = left + 1u;
	while (l > 0u && r + 1u < n && s[l - 1u] == s[r + 1u]) {
		l--;
		r++;
	}
	return (r - l) + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_palindrome_even_u(const unsigned char *s, size_t n,
                              size_t left)
    __attribute__((alias("gj_palindrome_even_u")));
