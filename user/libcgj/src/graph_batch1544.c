/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1544: binary min-heap extract (pop min) for
 * uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_u64_heap_pop(uint64_t *a, size_t *n, uint64_t *out);
 *     — remove the minimum (root) into *out; shrink *n; re-heapify.
 *       Returns 0 (OK) on success, -1 (FAIL) if empty or any pointer is
 *       NULL.
 *   int __gj_u64_heap_pop  (alias)
 *   __libcgj_batch1544_marker = "libcgj-batch1544"
 *
 * Layout: caller-owned array a[]; *n is live element count. 0-based
 * indices: parent (i-1)/2, children 2i+1 and 2i+2. Assumes a[0..*n) is
 * already a min-heap.
 *
 * Distinct from gj_u32_heap_pop (batch674), gj_heappop_min_u64 (batch246),
 * gj_heap_pop (batch179), and gj_i64_heap_pop (batch702). Unique gj_ names
 * only; no multi-def. Self-contained static sift-down.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1544_marker[] = "libcgj-batch1544";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1544_swap(uint64_t *pA, uint64_t *pB)
{
	uint64_t u64Tmp;

	u64Tmp = *pA;
	*pA = *pB;
	*pB = u64Tmp;
}

/*
 * Sift element at index i down toward leaves while it is larger than a child.
 * Restores min-heap order after replacing the root (or after a pop hole-fill).
 */
static void
b1544_sift_down(uint64_t *pA, size_t cN, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;

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
		b1544_swap(&pA[i], &pA[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_heap_pop — extract the minimum (root) into *out.
 *
 * Assumes a[0..*n) is already a min-heap. On success: *out = a[0], *n
 * decreased by one, last leaf moved to root and sifted down.
 *
 * Returns:
 *   0  (OK)   — popped into *out
 *   -1 (FAIL) — empty heap or any of a / n / out is NULL
 */
int
gj_u64_heap_pop(uint64_t *pA, size_t *pN, uint64_t *pOut)
{
	size_t cCur;

	if (pA == NULL || pN == NULL || pOut == NULL) {
		return -1;
	}
	cCur = *pN;
	if (cCur == 0u) {
		return -1;
	}

	*pOut = pA[0];
	cCur -= 1u;
	*pN = cCur;
	if (cCur > 0u) {
		pA[0] = pA[cCur];
		b1544_sift_down(pA, cCur, 0u);
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_heap_pop(uint64_t *pA, size_t *pN, uint64_t *pOut)
    __attribute__((alias("gj_u64_heap_pop")));
