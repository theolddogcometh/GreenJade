/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1281: classic binary search on sorted u32.
 *
 * Surface (unique symbols — distinct from gj_bsearch_u32 (batch407)):
 *   size_t gj_u32_binary_search(const uint32_t *a, size_t n, uint32_t v);
 *     — Binary-search sorted nondecreasing a[0..n) for v. Return any
 *       index i with a[i] == v, or n if not found (also when a == NULL
 *       or n == 0). Caller must ensure a is sorted ascending.
 *   size_t __gj_u32_binary_search  (alias)
 *   __libcgj_batch1281_marker = "libcgj-batch1281"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1281_marker[] = "libcgj-batch1281";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_binary_search — index of v in sorted a[0..n), or n if absent.
 *
 * Edge cases:
 *   a == NULL → n
 *   n == 0 → 0 (not-found sentinel is n)
 *   multiple matches → any matching index (classic bsearch)
 */
size_t
gj_u32_binary_search(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iLo;
	size_t iHi;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	iLo = 0u;
	iHi = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + (iHi - iLo) / 2u;

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

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_binary_search(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_u32_binary_search")));
