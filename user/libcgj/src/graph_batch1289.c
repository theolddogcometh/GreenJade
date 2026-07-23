/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1289: exponential search on sorted u32.
 *
 * Surface (unique symbols — distinct from gj_u32_gallop_search
 * (batch1287) and gj_u32_binary_search (batch1281)):
 *   size_t gj_u32_exponential_search(const uint32_t *a, size_t n,
 *                                    uint32_t v);
 *     — Find v in sorted nondecreasing a[0..n) by exponential range
 *       discovery (1,2,4,...) then binary search within the bracket.
 *       Return any matching index, or n if absent. NULL a or n == 0 → n.
 *   size_t __gj_u32_exponential_search  (alias)
 *   __libcgj_batch1289_marker = "libcgj-batch1289"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1289_marker[] = "libcgj-batch1289";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1289_bin(const uint32_t *pA, size_t iLo, size_t iHi, uint32_t u32V)
{
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
	return (size_t)-1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_exponential_search — exp bound then binary search; n if absent.
 */
size_t
gj_u32_exponential_search(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iBound;
	size_t iLo;
	size_t iHit;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	if (pA[0] == u32V) {
		return 0u;
	}
	if (u32V < pA[0]) {
		return cN;
	}

	iBound = 1u;
	while (iBound < cN && pA[iBound] < u32V) {
		if (iBound > (cN - 1u) / 2u) {
			iBound = cN - 1u;
			break;
		}
		iBound <<= 1;
	}
	if (iBound >= cN) {
		iBound = cN - 1u;
	}

	if (pA[iBound] == u32V) {
		return iBound;
	}

	iLo = iBound / 2u;
	/* Search in [iLo, iBound] inclusive → half-open [iLo, iBound+1). */
	iHit = b1289_bin(pA, iLo, iBound + 1u, u32V);
	if (iHit == (size_t)-1) {
		return cN;
	}
	return iHit;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_exponential_search(const uint32_t *pA, size_t cN,
    uint32_t u32V)
    __attribute__((alias("gj_u32_exponential_search")));
