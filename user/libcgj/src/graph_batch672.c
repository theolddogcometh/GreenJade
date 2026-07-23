/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch672: freestanding min-heap sift-down over a
 * bounded uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_heap_sift_down(uint32_t *a, size_t n, size_t i);
 *     — Restore min-heap order for the subtree rooted at index i in
 *       a[0..n) by sifting a[i] down toward the leaves while it is
 *       strictly larger than a child. 0-based layout: children of i
 *       are 2i+1 and 2i+2. NULL a, n < 2, or i >= n → no-op.
 *   void __gj_u32_heap_sift_down  (alias)
 *   __libcgj_batch672_marker = "libcgj-batch672"
 *
 * Distinct from gj_heapify_min_u64 / gj_heappop_min_u64 (batch246),
 * gj_heap_* (batch179), and gj_heapsort (batch100). Unique gj_ names
 * only; no multi-def with those surfaces.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch672_marker[] = "libcgj-batch672";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_heap_sift_down — min-heap sift-down of a[i] within a[0..n).
 *
 * a: base of n uint32_t elements forming a nearly-valid min-heap
 *    (NULL → no-op)
 * n: element count (n < 2 → no-op; no child can exist)
 * i: root of the subtree to repair (i >= n → no-op)
 *
 * While a[i] is strictly larger than at least one child, swap with the
 * smaller child and continue from that child. Equal keys are not swapped
 * (stable relative to siblings that already hold equal values). Mutates
 * only the path from i toward a leaf. Does not call other heap surfaces.
 */
void
gj_u32_heap_sift_down(uint32_t *pA, size_t cN, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;
	uint32_t u32Tmp;

	if (pA == NULL || cN < 2u || i >= cN) {
		return;
	}

	for (;;) {
		iLeft = 2u * i + 1u;
		iRight = 2u * i + 2u;
		iSmall = i;

		if (iLeft < cN && pA[iLeft] < pA[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < cN && pA[iRight] < pA[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}

		u32Tmp = pA[i];
		pA[i] = pA[iSmall];
		pA[iSmall] = u32Tmp;
		i = iSmall;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_heap_sift_down(uint32_t *pA, size_t cN, size_t i)
    __attribute__((alias("gj_u32_heap_sift_down")));
