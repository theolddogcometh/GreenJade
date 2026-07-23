/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3781: suffix-array LCP fill (_u).
 *
 * Surface (unique symbols):
 *   int gj_sa_lcp_u(const unsigned char *s, size_t n,
 *                   const size_t *sa, size_t *lcp);
 *     - Fill lcp[0..n) from string s[0..n) and suffix array sa[0..n).
 *       lcp[0] = 0; for i > 0, lcp[i] is the longest common prefix length
 *       of suffixes s[sa[i-1]..) and s[sa[i]..). sa must be a permutation
 *       of 0..n-1 (not re-validated beyond range checks). Returns 0 on
 *       success (including n == 0: no writes). Returns -1 if s/sa/lcp is
 *       NULL with n > 0, or any sa[i] >= n.
 *   int __gj_sa_lcp_u  (alias)
 *   __libcgj_batch3781_marker = "libcgj-batch3781"
 *
 * Exclusive continuum CREATE-ONLY (3781-3790: sa_lcp_u, suffix_cmp_u,
 * manacher_expand_u, palindrome_odd_u, palindrome_even_u, rolling_hash_u,
 * string_hash_pair_u, aho_fail_u, aho_output_u, batch_id_3790). Distinct
 * from gj_lcp_len / gj_lcp_naive / gj_suffix_array — unique _u surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3781_marker[] = "libcgj-batch3781";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * LCP of s[i..) and s[j..) within length n (unsigned byte order).
 */
static size_t
b3781_lcp(const unsigned char *s, size_t n, size_t i, size_t j)
{
	size_t k;

	k = 0u;
	while (i < n && j < n && s[i] == s[j]) {
		k++;
		i++;
		j++;
	}
	return k;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sa_lcp_u — LCP array for a suffix array (naive adjacent compare).
 *
 * Does not allocate. No parent wires.
 */
int
gj_sa_lcp_u(const unsigned char *s, size_t n, const size_t *sa, size_t *lcp)
{
	size_t i;

	(void)NULL;
	if (n == 0u) {
		return 0;
	}
	if (s == NULL || sa == NULL || lcp == NULL) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		if (sa[i] >= n) {
			return -1;
		}
	}

	lcp[0] = 0u;
	for (i = 1u; i < n; i++) {
		lcp[i] = b3781_lcp(s, n, sa[i - 1u], sa[i]);
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sa_lcp_u(const unsigned char *s, size_t n, const size_t *sa,
                  size_t *lcp)
    __attribute__((alias("gj_sa_lcp_u")));
