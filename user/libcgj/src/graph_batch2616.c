/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2616: freestanding n-octet equality (_n_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_eq_n_u(const void *a, const void *b, size_t n);
 *     - Return 1 if the n-byte regions at a and b are octet-identical,
 *       else 0. n == 0 is vacuously equal (returns 1) even if a or b
 *       is NULL. If n > 0 and either pointer is NULL, returns 0.
 *   int __gj_mem_eq_n_u  (alias)
 *   __libcgj_batch2616_marker = "libcgj-batch2616"
 *
 * Post-2610 mem exclusive wave (2611-2620). Distinct from gj_mem_eq
 * (batch365) / gj_mem_eq_n (batch634) / gj_mem_cmp_n_u (batch2615) -
 * unique gj_mem_eq_n_u surface only; no multi-def. Returns boolean-style
 * 0/1 rather than a signed lexicographic difference.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2616_marker[] = "libcgj-batch2616";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pA[0..n) equals pB[0..n). Caller: both non-NULL when n > 0. */
static int
b2616_eq(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_eq_n_u - true if a[0..n) equals b[0..n) as unsigned octets.
 *
 * a, b: regions of at least n bytes when n > 0 (NULL with n > 0 -> 0)
 * n:    number of octets to compare (0 -> 1, equal empty windows)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 */
int
gj_mem_eq_n_u(const void *a, const void *b, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}
	return b2616_eq((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_eq_n_u(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_eq_n_u")));
