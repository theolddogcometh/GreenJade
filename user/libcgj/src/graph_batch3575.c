/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3575: freestanding bounded suffix equality (_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_suffix_eq_u(const void *hay, size_t n,
 *                          const void *sfx, size_t ns);
 *     - Return 1 if hay[0..n) ends with the ns octets of sfx, else 0.
 *       ns == 0 is a vacuous match (returns 1) even if hay or sfx is
 *       NULL. If ns > 0 and either pointer is NULL, or if ns > n,
 *       returns 0.
 *   int __gj_mem_suffix_eq_u  (alias)
 *   __libcgj_batch3575_marker = "libcgj-batch3575"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_suffix_eq_u surface only; no multi-def. Distinct from
 * gj_mem_suff_eq_n (batch2215) / gj_mem_prefix_eq_u (batch3574).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3575_marker[] = "libcgj-batch3575";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if last ns octets of pH[0..n) equal pS[0..ns).
 * Caller: both non-NULL, ns > 0, ns <= n.
 */
static int
b3575_suff_eq(const unsigned char *pH, size_t n,
    const unsigned char *pS, size_t ns)
{
	size_t i;
	const unsigned char *pTail;

	pTail = pH + (n - ns);
	for (i = 0u; i < ns; i++) {
		if (pTail[i] != pS[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_suffix_eq_u - true if hay[0..n) ends with sfx[0..ns).
 *
 * hay, n:  haystack base and length (NULL with ns > 0 -> 0)
 * sfx, ns: required suffix base and length (0 -> 1, vacuous match)
 *
 * Does not call libc. Early-exits on the first mismatch.
 * No parent wires.
 */
int
gj_mem_suffix_eq_u(const void *hay, size_t n, const void *sfx, size_t ns)
{
	(void)NULL;
	if (ns == 0u) {
		return 1;
	}
	if (ns > n) {
		return 0;
	}
	if (hay == NULL || sfx == NULL) {
		return 0;
	}
	return b3575_suff_eq((const unsigned char *)hay, n,
	    (const unsigned char *)sfx, ns);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_suffix_eq_u(const void *hay, size_t n, const void *sfx,
    size_t ns)
    __attribute__((alias("gj_mem_suffix_eq_u")));
