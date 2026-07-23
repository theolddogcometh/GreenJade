/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch365: bounded memory equality predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_eq(const void *a, const void *b, size_t n);
 *     — Return 1 if the n-byte regions at a and b are octet-identical,
 *       else 0. n == 0 is vacuously equal (returns 1) even if a or b
 *       is NULL. If n > 0 and either pointer is NULL, returns 0.
 *   int __gj_mem_eq  (alias)
 *   __libcgj_batch365_marker = "libcgj-batch365"
 *
 * Distinct from libc memcmp / bcmp in string.c — unique gj_ names only;
 * no multi-def with the existing string graph. Returns a boolean-style
 * 0/1 rather than a signed lexicographic difference.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch365_marker[] = "libcgj-batch365";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_eq — true if a[0..n) equals b[0..n) as unsigned octets.
 *
 * a, b: regions of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n:    number of octets to compare (0 → 1, equal empty windows)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 */
int
gj_mem_eq(const void *a, const void *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	for (i = 0u; i < n; i++) {
		if (pa[i] != pb[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_eq(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_eq")));
