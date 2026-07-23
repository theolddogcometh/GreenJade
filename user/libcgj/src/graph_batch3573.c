/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3573: freestanding n-octet lexicographic less.
 *
 * Surface (unique symbols):
 *   int gj_mem_lt_n_u(const void *a, const void *b, size_t n);
 *     - Return 1 if a[0..n) is strictly less than b[0..n) as unsigned
 *       octets (first differing octet a[i] < b[i]), else 0.
 *       n == 0 -> 0 (equal empty windows). n > 0 and either NULL:
 *       both NULL -> 0; a NULL -> 1; b NULL -> 0.
 *   int __gj_mem_lt_n_u  (alias)
 *   __libcgj_batch3573_marker = "libcgj-batch3573"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_lt_n_u surface only; no multi-def. Distinct from
 * gj_mem_cmp_n_u (batch2615) / gj_mem_eq_n_u2 — boolean less-than
 * predicate rather than signed difference. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3573_marker[] = "libcgj-batch3573";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pA[0..n) < pB[0..n) unsigned-octet lex order. Both non-NULL. */
static int
b3573_lt(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pA[i] < pB[i]) {
			return 1;
		}
		if (pA[i] > pB[i]) {
			return 0;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_lt_n_u - true if a[0..n) is strictly less than b[0..n).
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare (0 -> 0)
 *
 * Does not call libc memcmp. Early-exits on the first differing octet.
 * No parent wires.
 */
int
gj_mem_lt_n_u(const void *a, const void *b, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0;
	}
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? 1 : 0;
	}
	if (a == b) {
		return 0;
	}
	return b3573_lt((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_lt_n_u(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_lt_n_u")));
