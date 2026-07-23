/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2214: freestanding n-byte prefix equality.
 *
 * Surface (unique symbols):
 *   int gj_mem_pref_eq_n(const void *a, const void *b, size_t n);
 *     - Return 1 if a[0..n) equals b[0..n) as unsigned octets, else 0.
 *       n == 0 is vacuously equal (returns 1) even if a or b is NULL.
 *       If n > 0 and either pointer is NULL, returns 0.
 *   int __gj_mem_pref_eq_n  (alias)
 *   __libcgj_batch2214_marker = "libcgj-batch2214"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from gj_mem_eq /
 * gj_mem_eq_n (batch365/634) - unique gj_mem_pref_eq_n surface only;
 * no multi-def. Prefix-named for call-site clarity vs full-buffer eq.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2214_marker[] = "libcgj-batch2214";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pA[0..cb) equals pB[0..cb). Caller: both non-NULL when cb > 0. */
static int
b2214_pref_eq(const unsigned char *pA, const unsigned char *pB, size_t cb)
{
	size_t iByte;

	for (iByte = 0u; iByte < cb; iByte++) {
		if (pA[iByte] != pB[iByte]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_pref_eq_n - true if the n-byte prefixes of a and b match.
 *
 * a, b: regions of at least n bytes when n > 0 (NULL with n > 0 -> 0)
 * n:    prefix length in octets (0 -> 1, empty prefixes are equal)
 *
 * Does not call libc. Early-exits on the first mismatch.
 */
int
gj_mem_pref_eq_n(const void *a, const void *b, size_t n)
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
	return b2214_pref_eq((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_pref_eq_n(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_pref_eq_n")));
