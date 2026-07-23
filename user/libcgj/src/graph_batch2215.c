/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2215: freestanding n-byte suffix equality.
 *
 * Surface (unique symbols):
 *   int gj_mem_suff_eq_n(const void *a, size_t na, const void *b,
 *                        size_t nb, size_t n);
 *     - Return 1 if the last n octets of a[0..na) equal the last n
 *       octets of b[0..nb). n == 0 is vacuously equal (returns 1).
 *       If n > na or n > nb, or either base is NULL with n > 0, return 0.
 *   int __gj_mem_suff_eq_n  (alias)
 *   __libcgj_batch2215_marker = "libcgj-batch2215"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from
 * gj_mem_pref_eq_n (batch2214) / gj_mem_eq_n - unique gj_mem_suff_eq_n
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2215_marker[] = "libcgj-batch2215";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if last cb octets of pA[0..cbA) equal last cb of pB[0..cbB).
 * Caller: both non-NULL, cb > 0, cb <= cbA, cb <= cbB.
 */
static int
b2215_suff_eq(const unsigned char *pA, size_t cbA,
    const unsigned char *pB, size_t cbB, size_t cb)
{
	size_t iByte;
	const unsigned char *pSa;
	const unsigned char *pSb;

	pSa = pA + (cbA - cb);
	pSb = pB + (cbB - cb);
	for (iByte = 0u; iByte < cb; iByte++) {
		if (pSa[iByte] != pSb[iByte]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_suff_eq_n - true if trailing n-byte suffixes of a and b match.
 *
 * a, na: first buffer base and total length (NULL with n > 0 -> 0)
 * b, nb: second buffer base and total length (NULL with n > 0 -> 0)
 * n:     suffix length in octets (0 -> 1; must not exceed na or nb)
 *
 * Does not call libc. Early-exits on the first mismatch.
 */
int
gj_mem_suff_eq_n(const void *a, size_t na, const void *b, size_t nb,
    size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (n > na || n > nb) {
		return 0;
	}
	return b2215_suff_eq((const unsigned char *)a, na,
	    (const unsigned char *)b, nb, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_suff_eq_n(const void *a, size_t na, const void *b, size_t nb,
    size_t n)
    __attribute__((alias("gj_mem_suff_eq_n")));
