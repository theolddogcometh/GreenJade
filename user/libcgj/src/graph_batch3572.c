/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3572: freestanding n-octet inequality (_n_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_ne_n_u(const void *a, const void *b, size_t n);
 *     - Return 1 if the n-byte regions at a and b differ as unsigned
 *       octets, else 0. n == 0 is vacuously equal (returns 0) even if
 *       a or b is NULL. If n > 0 and either pointer is NULL, returns 1
 *       when the other is non-NULL, or 0 when both are NULL.
 *   int __gj_mem_ne_n_u  (alias)
 *   __libcgj_batch3572_marker = "libcgj-batch3572"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_ne_n_u surface only; no multi-def. Distinct from
 * gj_mem_eq_n_u2 (batch3571) / gj_mem_eq_n_u (batch2616) — boolean
 * inequality predicate. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3572_marker[] = "libcgj-batch3572";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pA[0..n) differs from pB[0..n). Caller: both non-NULL, n > 0. */
static int
b3572_ne(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pA[i] != pB[i]) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_ne_n_u - true if a[0..n) differs from b[0..n) as unsigned octets.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare (0 -> 0, empty windows are equal)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 * No parent wires.
 */
int
gj_mem_ne_n_u(const void *a, const void *b, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0;
	}
	if (a == NULL || b == NULL) {
		return (a != b) ? 1 : 0;
	}
	if (a == b) {
		return 0;
	}
	return b3572_ne((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_ne_n_u(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_ne_n_u")));
