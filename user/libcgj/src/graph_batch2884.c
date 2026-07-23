/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2884: binary search on sorted uint64_t (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_binsearch_u64_u(const uint64_t *a, size_t n, uint64_t v);
 *     - Binary-search sorted nondecreasing a[0..n) for v. Return any
 *       index i with a[i] == v, or n if not found (also when a == NULL
 *       or n == 0). Caller must ensure a is sorted ascending.
 *   size_t __gj_binsearch_u64_u  (alias)
 *   __libcgj_batch2884_marker = "libcgj-batch2884"
 *
 * Exclusive sort/search helpers wave (2881-2890). Companion to
 * gj_binsearch_u32_u (batch2883). Unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2884_marker[] = "libcgj-batch2884";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic bsearch; any matching index. pA non-NULL, cN > 0. */
static size_t
b2884_binsearch(const uint64_t *pA, size_t cN, uint64_t u64V)
{
	size_t iLo;
	size_t iHi;

	iLo = 0u;
	iHi = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + ((iHi - iLo) >> 1);

		if (pA[iMid] < u64V) {
			iLo = iMid + 1u;
		} else if (pA[iMid] > u64V) {
			iHi = iMid;
		} else {
			return iMid;
		}
	}
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_binsearch_u64_u - index of v in sorted a[0..n), or n if absent.
 *
 * a: sorted nondecreasing base (NULL → n)
 * n: element count (0 → 0; not-found sentinel is n)
 * v: probe value
 *
 * Multiple matches → any matching index. Does not call libc bsearch.
 */
size_t
gj_binsearch_u64_u(const uint64_t *pA, size_t cN, uint64_t u64V)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return cN;
	}
	return b2884_binsearch(pA, cN, u64V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_binsearch_u64_u(const uint64_t *pA, size_t cN, uint64_t u64V)
    __attribute__((alias("gj_binsearch_u64_u")));
