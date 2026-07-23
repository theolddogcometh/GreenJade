/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch701: freestanding int64_t min-heap push.
 *
 * Surface (unique symbols):
 *   int gj_i64_heap_push(int64_t *a, size_t *n, size_t cap, int64_t v);
 *     — Append v to a caller-owned min-heap a[0..*n) with capacity cap,
 *       then sift-up. Returns 0 on success, -1 if a/n is NULL or heap
 *       is full (*n >= cap).
 *   int __gj_i64_heap_push  (alias)
 *   __libcgj_batch701_marker = "libcgj-batch701"
 *
 * Distinct from gj_heap_push / gj_heap_pop (batch179 int64 top-k heap)
 * and from gj_u32_heap_push (batch673). Unique gj_i64_heap_push names
 * only; no multi-def.
 *
 * Layout: 0-based indices; parent (i-1)/2, children 2i+1 and 2i+2.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch701_marker[] = "libcgj-batch701";

/* ---- internal helpers -------------------------------------------------- */

static void
b701_swap(int64_t *pA, int64_t *pB)
{
	int64_t nTmp;

	nTmp = *pA;
	*pA = *pB;
	*pB = nTmp;
}

/*
 * Sift element at index i up toward root while it is smaller than its parent.
 * Restores min-heap order after an append at the end of the array.
 */
static void
b701_sift_up(int64_t *a, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (a[i] >= a[iParent]) {
			break;
		}
		b701_swap(&a[i], &a[iParent]);
		i = iParent;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_heap_push — min-heap insert of v into a[0..*n) with capacity cap.
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
gj_i64_heap_push(int64_t *a, size_t *n, size_t cap, int64_t v)
{
	size_t nCur;

	if (a == NULL || n == NULL) {
		return -1;
	}

	nCur = *n;
	if (nCur >= cap) {
		return -1;
	}

	a[nCur] = v;
	*n = nCur + 1u;
	b701_sift_up(a, nCur);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_heap_push(int64_t *a, size_t *n, size_t cap, int64_t v)
    __attribute__((alias("gj_i64_heap_push")));
