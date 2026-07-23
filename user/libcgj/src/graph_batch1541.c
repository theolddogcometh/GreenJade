/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1541: freestanding uint64_t min-heap sift-up.
 *
 * Surface (unique symbols):
 *   void gj_u64_heap_sift_up(uint64_t *a, size_t i);
 *     — Min-heap sift-up of element a[i] toward the root. While i > 0 and
 *       a[i] < a[parent], swap with parent where parent = (i-1)/2, then
 *       continue from the parent index. NULL a → no-op; i == 0 → no-op.
 *   void __gj_u64_heap_sift_up  (alias)
 *   __libcgj_batch1541_marker = "libcgj-batch1541"
 *
 * Layout: 0-based indices; parent (i-1)/2. Distinct from gj_u32_heap_sift_up
 * (batch671), gj_heap_* (batch179), and batch246 heapify/pop. Unique gj_
 * names only; no multi-def with the existing heap graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1541_marker[] = "libcgj-batch1541";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_heap_sift_up — restore min-heap order by bubbling a[i] upward.
 *
 * a: heap array base (NULL → no-op)
 * i: index of the element to sift up (0 → already at root, no-op)
 *
 * While i > 0, compare a[i] with a[(i-1)/2]. If a[i] is strictly smaller than
 * its parent, swap and set i to the parent index; otherwise stop. Mutates
 * a[] along the path from i to the final resting index only. Does not
 * observe or require a heap size argument (sift-up walks only toward root).
 */
void
gj_u64_heap_sift_up(uint64_t *pA, size_t i)
{
	size_t iParent;
	uint64_t u64Tmp;

	if (pA == NULL) {
		return;
	}

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pA[i] >= pA[iParent]) {
			break;
		}
		u64Tmp = pA[i];
		pA[i] = pA[iParent];
		pA[iParent] = u64Tmp;
		i = iParent;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u64_heap_sift_up(uint64_t *pA, size_t i)
    __attribute__((alias("gj_u64_heap_sift_up")));
