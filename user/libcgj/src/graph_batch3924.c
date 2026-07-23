/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3924: binary min-heap extract (pop min) for
 * uint64_t (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_heap_u64_pop_u(uint64_t *a, size_t *n, uint64_t *out);
 *     - Remove the minimum (root) into *out; shrink *n; re-heapify.
 *       Returns 0 (OK) on success, -1 (FAIL) if empty or any pointer is
 *       NULL.
 *   int __gj_heap_u64_pop_u  (alias)
 *   __libcgj_batch3924_marker = "libcgj-batch3924"
 *
 * Layout: caller-owned array a[]; *n is live element count. 0-based
 * indices: parent (i-1)/2, children 2i+1 and 2i+2. Assumes a[0..*n) is
 * already a min-heap.
 *
 * Exclusive continuum CREATE-ONLY (3921-3930). Distinct from
 * gj_heap_u32_pop_u (batch3331), gj_heap_pop (batch179), and
 * gj_heappop_min_u64 (batch246) - unique gj_heap_u64_pop_u surface
 * only; no multi-def. No parent wires (local sift-down only).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3924_marker[] = "libcgj-batch3924";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3924_swap(uint64_t *pA, uint64_t *pB)
{
	uint64_t u64Tmp;

	u64Tmp = *pA;
	*pA = *pB;
	*pB = u64Tmp;
}

/*
 * Sift element at index i down toward leaves while it is larger than a
 * child. Restores min-heap order after replacing the root (or after a
 * pop hole-fill). Local to this TU — no call to gj_heap_u64_sift_down_u.
 */
static void
b3924_sift_down(uint64_t *a, size_t n, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;

	for (;;) {
		iLeft = 2u * i + 1u;
		iRight = 2u * i + 2u;
		iSmall = i;

		if (iLeft < n && a[iLeft] < a[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < n && a[iRight] < a[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}
		b3924_swap(&a[i], &a[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_u64_pop_u - extract the minimum (root) into *out.
 *
 * Assumes a[0..*n) is already a min-heap. On success: *out = a[0], *n
 * decreased by one, last leaf moved to root and sifted down.
 *
 * Returns:
 *   0  (OK)   - popped into *out
 *   -1 (FAIL) - empty heap or any of a / n / out is NULL
 *
 * No parent wires. Does not call libc.
 */
int
gj_heap_u64_pop_u(uint64_t *a, size_t *n, uint64_t *out)
{
	size_t nCur;

	if (a == NULL || n == NULL || out == NULL) {
		return -1;
	}
	nCur = *n;
	if (nCur == 0u) {
		return -1;
	}

	*out = a[0];
	nCur -= 1u;
	*n = nCur;
	if (nCur > 0u) {
		a[0] = a[nCur];
		b3924_sift_down(a, nCur, 0u);
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_heap_u64_pop_u(uint64_t *a, size_t *n, uint64_t *out)
    __attribute__((alias("gj_heap_u64_pop_u")));
