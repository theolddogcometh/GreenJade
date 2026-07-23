/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch399: freestanding memory compare (lexicographic).
 *
 * Surface (unique symbols):
 *   int gj_mem_cmp(const void *a, const void *b, size_t n);
 *     — Compare n octets of a and b as unsigned chars.
 *       Returns 0 if equal; negative if a < b; positive if a > b
 *       (first differing octet: (int)a[i] - (int)b[i]).
 *       n == 0 → 0 even if a or b is NULL.
 *       n > 0 and either NULL → treat missing side as less
 *       (both NULL → 0; a NULL → -1; b NULL → 1).
 *   int __gj_mem_cmp  (alias)
 *   __libcgj_batch399_marker = "libcgj-batch399"
 *
 * Distinct from libc memcmp / gj_mem_eq — signed difference, not boolean.
 * Unique gj_ names only; no multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch399_marker[] = "libcgj-batch399";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_cmp — lexicographic unsigned-octet compare of n-byte windows.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare (0 → equal)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 */
int
gj_mem_cmp(const void *a, const void *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (n == 0u) {
		return 0;
	}
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	if (a == b) {
		return 0;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	for (i = 0u; i < n; i++) {
		if (pa[i] != pb[i]) {
			return (int)pa[i] - (int)pb[i];
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_cmp(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_cmp")));
