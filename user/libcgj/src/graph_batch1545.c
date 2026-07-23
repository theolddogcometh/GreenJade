/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1545: freestanding int32_t min-heap sift-up.
 *
 * Surface (unique symbols):
 *   void gj_i32_heap_sift_up_min(int32_t *a, size_t i);
 *     — Min-heap sift-up of element a[i] toward the root. While i > 0 and
 *       a[i] < a[parent], swap with parent where parent = (i-1)/2, then
 *       continue from the parent index. NULL a → no-op; i == 0 → no-op.
 *   void __gj_i32_heap_sift_up_min  (alias)
 *   __libcgj_batch1545_marker = "libcgj-batch1545"
 *
 * Layout: 0-based indices; parent (i-1)/2. Distinct from gj_u32_heap_sift_up
 * (batch671), gj_u64_heap_sift_up (batch1541), and gj_heap_* (batch179).
 * Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1545_marker[] = "libcgj-batch1545";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_heap_sift_up_min — restore min-heap order by bubbling a[i] upward.
 *
 * a: heap array base (NULL → no-op)
 * i: index of the element to sift up (0 → already at root, no-op)
 *
 * Signed int32_t comparisons: while i > 0, if a[i] is strictly smaller than
 * its parent, swap and continue. Equal keys are not swapped.
 */
void
gj_i32_heap_sift_up_min(int32_t *pA, size_t i)
{
	size_t iParent;
	int32_t i32Tmp;

	if (pA == NULL) {
		return;
	}

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pA[i] >= pA[iParent]) {
			break;
		}
		i32Tmp = pA[i];
		pA[i] = pA[iParent];
		pA[iParent] = i32Tmp;
		i = iParent;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_heap_sift_up_min(int32_t *pA, size_t i)
    __attribute__((alias("gj_i32_heap_sift_up_min")));
