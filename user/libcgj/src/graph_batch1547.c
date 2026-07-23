/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1547: binary min-heap extract (pop min) for
 * int32_t.
 *
 * Surface (unique symbols):
 *   int gj_i32_heap_pop_min(int32_t *a, size_t *n, int32_t *out);
 *     — remove the minimum (root) into *out; shrink *n; re-heapify.
 *       Returns 0 (OK) on success, -1 (FAIL) if empty or any pointer is
 *       NULL.
 *   int __gj_i32_heap_pop_min  (alias)
 *   __libcgj_batch1547_marker = "libcgj-batch1547"
 *
 * Layout: caller-owned array a[]; *n is live element count. 0-based
 * indices: parent (i-1)/2, children 2i+1 and 2i+2. Assumes a[0..*n) is
 * already a min-heap over signed int32_t keys.
 *
 * Distinct from gj_u32_heap_pop (batch674), gj_u64_heap_pop (batch1544),
 * gj_i64_heap_pop (batch702), and gj_heap_pop (batch179). Unique gj_ names
 * only; no multi-def. Self-contained static sift-down.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1547_marker[] = "libcgj-batch1547";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1547_swap(int32_t *pA, int32_t *pB)
{
	int32_t i32Tmp;

	i32Tmp = *pA;
	*pA = *pB;
	*pB = i32Tmp;
}

/*
 * Sift element at index i down toward leaves while it is larger than a child.
 * Restores min-heap order after replacing the root (or after a pop hole-fill).
 */
static void
b1547_sift_down(int32_t *pA, size_t cN, size_t i)
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
		b1547_swap(&pA[i], &pA[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_heap_pop_min — extract the minimum (root) into *out.
 *
 * Assumes a[0..*n) is already a min-heap. On success: *out = a[0], *n
 * decreased by one, last leaf moved to root and sifted down.
 *
 * Returns:
 *   0  (OK)   — popped into *out
 *   -1 (FAIL) — empty heap or any of a / n / out is NULL
 */
int
gj_i32_heap_pop_min(int32_t *pA, size_t *pN, int32_t *pOut)
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
		b1547_sift_down(pA, cCur, 0u);
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i32_heap_pop_min(int32_t *pA, size_t *pN, int32_t *pOut)
    __attribute__((alias("gj_i32_heap_pop_min")));
