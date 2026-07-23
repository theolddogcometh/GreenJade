/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2964: freestanding lexicographic compare (_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_cmp_u(const void *a, const void *b, size_t n);
 *     - Compare n octets of a and b as unsigned chars.
 *       Returns 0 if equal; negative if a < b; positive if a > b
 *       (first differing octet: (int)a[i] - (int)b[i]).
 *       n == 0 -> 0 even if a or b is NULL.
 *       n > 0 and either NULL -> treat missing side as less
 *       (both NULL -> 0; a NULL -> -1; b NULL -> 1).
 *   int __gj_mem_cmp_u  (alias)
 *   __libcgj_batch2964_marker = "libcgj-batch2964"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_cmp
 * (batch399) / gj_mem_cmp_n_u (batch2615) / libc memcmp - unique
 * gj_mem_cmp_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2964_marker[] = "libcgj-batch2964";

/* ---- freestanding helpers ---------------------------------------------- */

/* Lexicographic signed difference of n octets. Both non-NULL, n > 0. */
static int
b2964_cmp(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pA[i] != pB[i]) {
			return (int)pA[i] - (int)pB[i];
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_cmp_u - lexicographic unsigned-octet compare of n-byte windows.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare (0 -> equal)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 */
int
gj_mem_cmp_u(const void *a, const void *b, size_t n)
{
	(void)NULL;
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
	return b2964_cmp((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_cmp_u(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_cmp_u")));
