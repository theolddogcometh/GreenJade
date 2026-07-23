/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2558: freestanding in-place min-heapify over a
 * bounded uint32_t array (sort exclusive wave 2551-2560).
 *
 * Surface (unique symbols):
 *   void gj_u32_heapify_min(uint32_t *pA, size_t cN);
 *     - Build a binary min-heap in place over pA[0..cN) via Floyd
 *       bottom-up sift-down. NULL pA or cN < 2 -> no-op. O(cN).
 *   void __gj_u32_heapify_min  (alias)
 *   __libcgj_batch2558_marker = "libcgj-batch2558"
 *
 * Layout: 0-based; children of i are 2i+1 and 2i+2. Distinct from
 * gj_u32_heapify_max (batch2557), gj_heapify_min_u64 (batch246), and
 * sift helpers. Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2558_marker[] = "libcgj-batch2558";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2558_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t u32Tmp;

	u32Tmp = *pA;
	*pA = *pB;
	*pB = u32Tmp;
}

/*
 * Sift element at index i down toward leaves while it is larger than a
 * child. Restores min-heap order for the subtree rooted at i.
 */
static void
b2558_sift_down_min(uint32_t *pA, size_t cN, size_t i)
{
	for (;;) {
		size_t iLeft = 2u * i + 1u;
		size_t iRight = 2u * i + 2u;
		size_t iSmall = i;

		if (iLeft < cN && pA[iLeft] < pA[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < cN && pA[iRight] < pA[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}
		b2558_swap(&pA[i], &pA[iSmall]);
		i = iSmall;
	}
}

static void
b2558_heapify_min(uint32_t *pA, size_t cN)
{
	size_t i;

	i = cN / 2u;
	while (i > 0u) {
		i -= 1u;
		b2558_sift_down_min(pA, cN, i);
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_heapify_min - build a binary min-heap in place over pA[0..cN).
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Floyd bottom-up: sift down from the last non-leaf (cN/2 - 1) to root.
 * Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_heapify_min(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2558_heapify_min(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_heapify_min(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_heapify_min")));
