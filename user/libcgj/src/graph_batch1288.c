/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1288: interpolation search on sorted u32.
 *
 * Surface (unique symbols — intended for uniformly distributed keys):
 *   size_t gj_u32_interpolation_search(const uint32_t *a, size_t n,
 *                                      uint32_t v);
 *     — Interpolation-search sorted nondecreasing a[0..n) for v.
 *       Return any matching index, or n if absent. NULL a or n == 0 → n.
 *       Falls back safely when range is flat (all equal span).
 *   size_t __gj_u32_interpolation_search  (alias)
 *   __libcgj_batch1288_marker = "libcgj-batch1288"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1288_marker[] = "libcgj-batch1288";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_interpolation_search — probe by value density; n if absent.
 *
 * Uses 64-bit arithmetic for the linear interpolate to avoid mid-range
 * overflow: pos ≈ lo + (v - a[lo]) * (hi - lo) / (a[hi] - a[lo]).
 */
size_t
gj_u32_interpolation_search(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t iLo;
	size_t iHi;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	iLo = 0u;
	iHi = cN - 1u;

	if (u32V < pA[iLo] || u32V > pA[iHi]) {
		return cN;
	}

	while (iLo <= iHi && u32V >= pA[iLo] && u32V <= pA[iHi]) {
		size_t iPos;
		uint32_t u32Lo;
		uint32_t u32Hi;
		uint64_t u64Span;
		uint64_t u64Off;
		uint64_t u64Width;

		u32Lo = pA[iLo];
		u32Hi = pA[iHi];

		if (u32Lo == u32Hi) {
			if (u32Lo == u32V) {
				return iLo;
			}
			return cN;
		}

		u64Span = (uint64_t)u32Hi - (uint64_t)u32Lo;
		u64Off = (uint64_t)u32V - (uint64_t)u32Lo;
		u64Width = (uint64_t)(iHi - iLo);
		iPos = iLo + (size_t)((u64Off * u64Width) / u64Span);

		/* Clamp (should already be in range, belt-and-suspenders). */
		if (iPos < iLo) {
			iPos = iLo;
		}
		if (iPos > iHi) {
			iPos = iHi;
		}

		if (pA[iPos] == u32V) {
			return iPos;
		}
		if (pA[iPos] < u32V) {
			iLo = iPos + 1u;
		} else {
			if (iPos == 0u) {
				return cN;
			}
			iHi = iPos - 1u;
		}
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_interpolation_search(const uint32_t *pA, size_t cN,
    uint32_t u32V)
    __attribute__((alias("gj_u32_interpolation_search")));
