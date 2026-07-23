/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1409: last-match binary search (index or -1).
 *
 * Surface (unique symbols — distinct return convention from the size_t
 * sentinel form in batch1283; this TU is intended as the signed-index
 * surface if/when the older size_t form is retired from the map):
 *   int gj_u32_bsearch_last_i(const uint32_t *a, size_t n,
 *                                 uint32_t v);
 *     — Last index i with a[i] == v in sorted nondecreasing a[0..n),
 *       or -1 if absent. NULL a or n == 0 → -1. n must fit in int
 *       (n > INT_MAX → -1).
 *   int __gj_u32_bsearch_last_i  (alias)
 *   __libcgj_batch1409_marker = "libcgj-batch1409"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1409_marker[] = "libcgj-batch1409";

/* INT_MAX without limits.h (freestanding). */
#define B1409_INT_MAX 0x7fffffff

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1409_last_eq — rightmost equal index, or (size_t)-1 if absent.
 * Caller guarantees a != NULL and n > 0.
 */
static size_t
b1409_last_eq(const uint32_t *a, size_t n, uint32_t v)
{
	size_t iLo;
	size_t iHi;
	size_t iFound;

	iLo = 0u;
	iHi = n;
	iFound = (size_t)-1;
	while (iLo < iHi) {
		size_t iMid = iLo + (iHi - iLo) / 2u;

		if (a[iMid] > v) {
			iHi = iMid;
		} else {
			if (a[iMid] == v) {
				iFound = iMid;
			}
			iLo = iMid + 1u;
		}
	}
	return iFound;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bsearch_last_i — rightmost index of v, or -1 if absent.
 */
int
gj_u32_bsearch_last_i(const uint32_t *a, size_t n, uint32_t v)
{
	size_t iFound;

	if (a == NULL || n == 0u) {
		return -1;
	}
	if (n > (size_t)B1409_INT_MAX) {
		return -1;
	}

	iFound = b1409_last_eq(a, n, v);
	if (iFound == (size_t)-1) {
		return -1;
	}
	return (int)iFound;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_bsearch_last_i(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_bsearch_last_i")));
