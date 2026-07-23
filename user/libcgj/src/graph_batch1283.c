/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1283: last-match binary search on sorted u32.
 *
 * Surface (unique symbols — distinct from gj_u32_binary_search_first
 * (batch1282) and gj_u32_upper_bound (batch668)):
 *   size_t gj_u32_binary_search_last(const uint32_t *a, size_t n,
 *                                    uint32_t v);
 *     — Last index i with a[i] == v in sorted nondecreasing a[0..n),
 *       or n if absent. NULL a or n == 0 → n.
 *   size_t __gj_u32_binary_search_last  (alias)
 *   __libcgj_batch1283_marker = "libcgj-batch1283"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1283_marker[] = "libcgj-batch1283";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_binary_search_last — rightmost index of v, or n if absent.
 */
size_t
gj_u32_binary_search_last(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iLo;
	size_t iHi;
	size_t iFound;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	iLo = 0u;
	iHi = cN;
	iFound = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + (iHi - iLo) / 2u;

		if (pA[iMid] > u32V) {
			iHi = iMid;
		} else {
			if (pA[iMid] == u32V) {
				iFound = iMid;
			}
			iLo = iMid + 1u;
		}
	}
	return iFound;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_binary_search_last(const uint32_t *pA, size_t cN,
    uint32_t u32V)
    __attribute__((alias("gj_u32_binary_search_last")));
