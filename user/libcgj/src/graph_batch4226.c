/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4226: freestanding min-length prefix equality (_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_prefix_eq_u2(const void *a, size_t na,
 *                          const void *b, size_t nb);
 *     - Let m = min(na, nb). Return 1 if a[0..m) equals b[0..m) as
 *       unsigned octets, else 0. m == 0 is vacuously equal (returns 1)
 *       even if a or b is NULL. If m > 0 and either pointer is NULL,
 *       returns 0.
 *   int __gj_mem_prefix_eq_u2  (alias)
 *   __libcgj_batch4226_marker = "libcgj-batch4226"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Min-length compare of leading windows (not
 * "is-strict-prefix-of"). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4226_marker[] = "libcgj-batch4226";

/* ---- freestanding helpers ---------------------------------------------- */

/* min of two size_t values. */
static size_t
b4226_min(size_t x, size_t y)
{
	return (x < y) ? x : y;
}

/*
 * 1 if pA[0..m) equals pB[0..m). Caller: both non-NULL, m > 0.
 */
static int
b4226_pref_eq(const unsigned char *pA, const unsigned char *pB, size_t m)
{
	size_t i;

	for (i = 0u; i < m; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_prefix_eq_u2 - min-length leading-window equality of a and b.
 *
 * a, na: first region base and length
 * b, nb: second region base and length
 * m = min(na, nb): compared length (0 -> 1, vacuous equal)
 *
 * Does not call libc. Early-exits on the first mismatch.
 * No parent wires.
 */
int
gj_mem_prefix_eq_u2(const void *a, size_t na, const void *b, size_t nb)
{
	size_t m;

	(void)NULL;
	m = b4226_min(na, nb);
	if (m == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}
	return b4226_pref_eq((const unsigned char *)a,
	    (const unsigned char *)b, m);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_prefix_eq_u2(const void *a, size_t na, const void *b, size_t nb)
    __attribute__((alias("gj_mem_prefix_eq_u2")));
