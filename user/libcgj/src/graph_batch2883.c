/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2883: binary search on sorted uint32_t (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_binsearch_u32_u(const uint32_t *a, size_t n, uint32_t v);
 *     - Binary-search sorted nondecreasing a[0..n) for v. Return any
 *       index i with a[i] == v, or n if not found (also when a == NULL
 *       or n == 0). Caller must ensure a is sorted ascending.
 *   size_t __gj_binsearch_u32_u  (alias)
 *   __libcgj_batch2883_marker = "libcgj-batch2883"
 *
 * Exclusive sort/search helpers wave (2881-2890). Distinct from
 * gj_bsearch_u32 (batch407) and gj_u32_binary_search (batch1281) —
 * unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2883_marker[] = "libcgj-batch2883";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic bsearch; any matching index. pA non-NULL, cN > 0. */
static size_t
b2883_binsearch(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iLo;
	size_t iHi;

	iLo = 0u;
	iHi = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + ((iHi - iLo) >> 1);

		if (pA[iMid] < u32V) {
			iLo = iMid + 1u;
		} else if (pA[iMid] > u32V) {
			iHi = iMid;
		} else {
			return iMid;
		}
	}
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_binsearch_u32_u - index of v in sorted a[0..n), or n if absent.
 *
 * a: sorted nondecreasing base (NULL → n)
 * n: element count (0 → 0; not-found sentinel is n)
 * v: probe value
 *
 * Multiple matches → any matching index. Does not call libc bsearch.
 */
size_t
gj_binsearch_u32_u(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return cN;
	}
	return b2883_binsearch(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_binsearch_u32_u(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_binsearch_u32_u")));
