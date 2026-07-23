/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3782: lexicographic suffix compare (_u).
 *
 * Surface (unique symbols):
 *   int gj_suffix_cmp_u(const unsigned char *s, size_t n, size_t i,
 *                       size_t j);
 *     - Compare suffixes s[i..n) and s[j..n) as unsigned bytes.
 *       Returns <0 if s[i..) < s[j..), 0 if equal, >0 if s[i..) > s[j..).
 *       If one is a proper prefix of the other, the shorter remaining
 *       suffix is smaller. NULL s with n > 0, or i/j >= n → 0 (tie /
 *       invalid treated as equal for a stable comparator default).
 *       n == 0 → 0.
 *   int __gj_suffix_cmp_u  (alias)
 *   __libcgj_batch3782_marker = "libcgj-batch3782"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790). Distinct from the private
 * comparator inside gj_suffix_array (batch242) — unique public _u surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3782_marker[] = "libcgj-batch3782";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_suffix_cmp_u — compare two suffixes of a bounded byte string.
 *
 * Does not allocate. No parent wires.
 */
int
gj_suffix_cmp_u(const unsigned char *s, size_t n, size_t i, size_t j)
{
	size_t a;
	size_t b;

	(void)NULL;
	if (s == NULL || n == 0u || i >= n || j >= n) {
		return 0;
	}

	a = i;
	b = j;
	while (a < n && b < n) {
		if (s[a] != s[b]) {
			return (s[a] < s[b]) ? -1 : 1;
		}
		a++;
		b++;
	}
	if (a == b) {
		return 0;
	}
	/* shorter remaining length sorts first */
	return (a == n) ? -1 : 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_suffix_cmp_u(const unsigned char *s, size_t n, size_t i, size_t j)
    __attribute__((alias("gj_suffix_cmp_u")));
