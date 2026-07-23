/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2885: lower_bound on sorted uint32_t (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_lower_bound_u32_u(const uint32_t *a, size_t n, uint32_t v);
 *     - First index i in [0, n] such that a[i] >= v (a sorted ascending).
 *       Returns n if every element is < v, or if n == 0, or if a is NULL
 *       with n > 0. a[0..n) must be nondecreasing.
 *   size_t __gj_lower_bound_u32_u  (alias)
 *   __libcgj_batch2885_marker = "libcgj-batch2885"
 *
 * Exclusive sort/search helpers wave (2881-2890). Distinct from
 * gj_u32_lower_bound (batch667) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2885_marker[] = "libcgj-batch2885";

/* ---- freestanding helpers ---------------------------------------------- */

/* First i with pA[i] >= u32V. pA non-NULL, cN > 0. */
static size_t
b2885_lower(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iLo;
	size_t iHi;

	iLo = 0u;
	iHi = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + ((iHi - iLo) >> 1);

		if (pA[iMid] < u32V) {
			iLo = iMid + 1u;
		} else {
			iHi = iMid;
		}
	}
	return iLo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lower_bound_u32_u - first index i with a[i] >= v, or n if none.
 *
 * a: sorted nondecreasing array of n uint32_t (NULL with n > 0 → n)
 * n: element count (0 → 0)
 * v: threshold value
 *
 * Elements in [0, result) are all < v; elements in [result, n) are
 * all >= v. O(log n) comparisons. Does not set errno.
 */
size_t
gj_lower_bound_u32_u(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	if (cN == 0u || pA == NULL) {
		return cN;
	}
	return b2885_lower(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lower_bound_u32_u(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_lower_bound_u32_u")));
