/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3921: freestanding uint64_t min-heap sift-up (_u).
 *
 * Surface (unique symbols):
 *   void gj_heap_u64_sift_up_u(uint64_t *a, size_t i);
 *     - Min-heap sift-up of element a[i] toward the root. While i > 0 and
 *       a[i] < a[parent], swap with parent where parent = (i-1)/2, then
 *       continue from the parent index. NULL a -> no-op; i == 0 -> no-op.
 *   void __gj_heap_u64_sift_up_u  (alias)
 *   __libcgj_batch3921_marker = "libcgj-batch3921"
 *
 * Exclusive continuum CREATE-ONLY (3921-3930): heap_u64_sift_up_u,
 * heap_u64_sift_down_u, heap_u64_push_u, heap_u64_pop_u, heap_u64_peek_u,
 * pq_u32_better_u, pq_u64_better_u, deque_u64_push_front_u,
 * deque_u64_push_back_u, batch_id_3930.
 * Distinct from gj_heap_u32_sift_up_u (batch3327) and gj_u32_heap_sift_up
 * (batch671) — unique heap_u64_*_u surface only; no multi-def.
 * No parent wires.
 *
 * Layout: 0-based indices; parent (i-1)/2.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3921_marker[] = "libcgj-batch3921";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_u64_sift_up_u - restore min-heap order by bubbling a[i] upward.
 *
 * a: heap array base (NULL -> no-op)
 * i: index of the element to sift up (0 -> already at root, no-op)
 *
 * While i > 0, compare a[i] with a[(i-1)/2]. If a[i] is strictly smaller than
 * its parent, swap and set i to the parent index; otherwise stop. Mutates
 * a[] along the path from i to the final resting index only. Does not
 * observe or require a heap size argument (sift-up walks only toward root).
 */
void
gj_heap_u64_sift_up_u(uint64_t *pA, size_t i)
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

void __gj_heap_u64_sift_up_u(uint64_t *pA, size_t i)
    __attribute__((alias("gj_heap_u64_sift_up_u")));
