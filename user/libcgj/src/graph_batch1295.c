/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1295: max-heap property check over u32 array.
 *
 * Surface (unique symbols — distinct from gj_heap_is_min_u32 (batch1294)):
 *   int gj_heap_is_max_u32(const uint32_t *a, size_t n);
 *     — Return 1 if a[0..n) satisfies the max-heap order property
 *       (parent >= both children when present). Empty or single-element
 *       heaps are valid (1). NULL a with n > 0 → 0. n == 0 → 1.
 *   int __gj_heap_is_max_u32  (alias)
 *   __libcgj_batch1295_marker = "libcgj-batch1295"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1295_marker[] = "libcgj-batch1295";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_is_max_u32 — 1 if a is a max-heap of n uint32_t elements.
 */
int
gj_heap_is_max_u32(const uint32_t *pA, size_t cN)
{
	size_t iIdx;

	if (cN == 0u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}

	for (iIdx = 0u; iIdx < cN; iIdx++) {
		size_t iLeft = iIdx * 2u + 1u;
		size_t iRight = iIdx * 2u + 2u;

		if (iLeft >= cN) {
			break;
		}
		if (pA[iIdx] < pA[iLeft]) {
			return 0;
		}
		if (iRight < cN && pA[iIdx] < pA[iRight]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_heap_is_max_u32(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_heap_is_max_u32")));
