/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1543: freestanding uint64_t min-heap push.
 *
 * Surface (unique symbols):
 *   int gj_u64_heap_push(uint64_t *a, size_t *n, size_t cap, uint64_t v);
 *     — Append v to a caller-owned min-heap a[0..*n) with capacity cap,
 *       then sift-up. Returns 0 on success, -1 if a/n is NULL or heap
 *       is full (*n >= cap).
 *   int __gj_u64_heap_push  (alias)
 *   __libcgj_batch1543_marker = "libcgj-batch1543"
 *
 * Distinct from gj_u32_heap_push (batch673), gj_i64_heap_push (batch701),
 * and gj_heap_push (batch179). Unique gj_u64_heap_push names only; no
 * multi-def. Self-contained (static sift-up; no parent TU linkage).
 *
 * Layout: 0-based indices; parent (i-1)/2, children 2i+1 and 2i+2.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1543_marker[] = "libcgj-batch1543";

/* ---- internal helpers -------------------------------------------------- */

static void
b1543_swap(uint64_t *pA, uint64_t *pB)
{
	uint64_t u64Tmp;

	u64Tmp = *pA;
	*pA = *pB;
	*pB = u64Tmp;
}

/*
 * Sift element at index i up toward root while it is smaller than its parent.
 * Restores min-heap order after an append at the end of the array.
 */
static void
b1543_sift_up(uint64_t *pA, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pA[i] >= pA[iParent]) {
			break;
		}
		b1543_swap(&pA[i], &pA[iParent]);
		i = iParent;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_heap_push — min-heap insert of v into a[0..*n) with capacity cap.
 *
 * a:   heap storage (NULL → -1)
 * n:   live element count in/out (NULL → -1)
 * cap: maximum elements a can hold; full when *n >= cap → -1
 * v:   value to insert
 *
 * On success: a[*n] = v, *n increased by 1, sift-up restores heap order;
 * returns 0. Does not replace root when full (unlike batch179 top-k push).
 */
int
gj_u64_heap_push(uint64_t *pA, size_t *pN, size_t cCap, uint64_t u64V)
{
	size_t cCur;

	if (pA == NULL || pN == NULL) {
		return -1;
	}

	cCur = *pN;
	if (cCur >= cCap) {
		return -1;
	}

	pA[cCur] = u64V;
	*pN = cCur + 1u;
	b1543_sift_up(pA, cCur);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_heap_push(uint64_t *pA, size_t *pN, size_t cCap, uint64_t u64V)
    __attribute__((alias("gj_u64_heap_push")));
