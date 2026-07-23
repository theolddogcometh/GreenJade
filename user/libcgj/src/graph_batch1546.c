/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1546: freestanding int32_t min-heap push.
 *
 * Surface (unique symbols):
 *   int gj_i32_heap_push_min(int32_t *a, size_t *n, size_t cap, int32_t v);
 *     — Append v to a caller-owned min-heap a[0..*n) with capacity cap,
 *       then sift-up. Returns 0 on success, -1 if a/n is NULL or heap
 *       is full (*n >= cap).
 *   int __gj_i32_heap_push_min  (alias)
 *   __libcgj_batch1546_marker = "libcgj-batch1546"
 *
 * Distinct from gj_u32_heap_push (batch673), gj_u64_heap_push (batch1543),
 * and gj_heap_push (batch179). Unique gj_i32_heap_push_min names only; no
 * multi-def. Self-contained static sift-up.
 *
 * Layout: 0-based indices; parent (i-1)/2, children 2i+1 and 2i+2.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1546_marker[] = "libcgj-batch1546";

/* ---- internal helpers -------------------------------------------------- */

static void
b1546_swap(int32_t *pA, int32_t *pB)
{
	int32_t i32Tmp;

	i32Tmp = *pA;
	*pA = *pB;
	*pB = i32Tmp;
}

/*
 * Sift element at index i up toward root while it is smaller than its parent.
 * Restores min-heap order after an append at the end of the array.
 */
static void
b1546_sift_up(int32_t *pA, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pA[i] >= pA[iParent]) {
			break;
		}
		b1546_swap(&pA[i], &pA[iParent]);
		i = iParent;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_heap_push_min — min-heap insert of v into a[0..*n) with capacity cap.
 *
 * a:   heap storage (NULL → -1)
 * n:   live element count in/out (NULL → -1)
 * cap: maximum elements a can hold; full when *n >= cap → -1
 * v:   signed value to insert
 *
 * On success: a[*n] = v, *n increased by 1, sift-up restores heap order;
 * returns 0. Does not replace root when full.
 */
int
gj_i32_heap_push_min(int32_t *pA, size_t *pN, size_t cCap, int32_t i32V)
{
	size_t cCur;

	if (pA == NULL || pN == NULL) {
		return -1;
	}

	cCur = *pN;
	if (cCur >= cCap) {
		return -1;
	}

	pA[cCur] = i32V;
	*pN = cCur + 1u;
	b1546_sift_up(pA, cCur);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i32_heap_push_min(int32_t *pA, size_t *pN, size_t cCap, int32_t i32V)
    __attribute__((alias("gj_i32_heap_push_min")));
