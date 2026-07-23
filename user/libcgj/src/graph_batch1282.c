/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1282: first-match binary search on sorted u32.
 *
 * Surface (unique symbols — distinct from gj_bsearch_u32 (batch407) and
 * gj_u32_binary_search (batch1281)):
 *   size_t gj_u32_binary_search_first(const uint32_t *a, size_t n,
 *                                     uint32_t v);
 *     — First index i with a[i] == v in sorted nondecreasing a[0..n),
 *       or n if absent. NULL a or n == 0 → n.
 *   size_t __gj_u32_binary_search_first  (alias)
 *   __libcgj_batch1282_marker = "libcgj-batch1282"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1282_marker[] = "libcgj-batch1282";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_binary_search_first — leftmost index of v, or n if absent.
 */
size_t
gj_u32_binary_search_first(const uint32_t *pA, size_t cN, uint32_t u32V)
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

		if (pA[iMid] < u32V) {
			iLo = iMid + 1u;
		} else {
			if (pA[iMid] == u32V) {
				iFound = iMid;
			}
			iHi = iMid;
		}
	}
	return iFound;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_binary_search_first(const uint32_t *pA, size_t cN,
    uint32_t u32V)
    __attribute__((alias("gj_u32_binary_search_first")));
