/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1284: classic binary search on sorted i64.
 *
 * Surface (unique symbols):
 *   size_t gj_i64_binary_search(const int64_t *a, size_t n, int64_t v);
 *     — Binary-search sorted nondecreasing a[0..n) for v. Return any
 *       index i with a[i] == v, or n if not found (also when a == NULL
 *       or n == 0). Caller must ensure a is sorted ascending.
 *   size_t __gj_i64_binary_search  (alias)
 *   __libcgj_batch1284_marker = "libcgj-batch1284"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1284_marker[] = "libcgj-batch1284";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_binary_search — index of v in sorted a[0..n), or n if absent.
 */
size_t
gj_i64_binary_search(const int64_t *pA, size_t cN, int64_t i64V)
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

		if (pA[iMid] < i64V) {
			iLo = iMid + 1u;
		} else if (pA[iMid] > i64V) {
			iHi = iMid;
		} else {
			return iMid;
		}
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_i64_binary_search(const int64_t *pA, size_t cN, int64_t i64V)
    __attribute__((alias("gj_i64_binary_search")));
