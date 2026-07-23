/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch246: binary min-heapify / extract for uint64_t.
 *
 * Surface (unique symbols):
 *   void gj_heapify_min_u64(uint64_t *a, size_t n);
 *     — in-place bottom-up build of a binary min-heap over a[0..n).
 *       NULL a or n < 2 is a no-op. O(n).
 *   int  gj_heappop_min_u64(uint64_t *a, size_t *n, uint64_t *out);
 *     — remove the minimum (root) into *out; shrink *n; re-heapify.
 *       Returns 0 on success, -1 if empty or any pointer is NULL.
 *   __gj_heapify_min_u64 / __gj_heappop_min_u64  (aliases)
 *   __libcgj_batch246_marker = "libcgj-batch246"
 *
 * Layout: caller-owned array a[]; 0-based indices: parent (i-1)/2,
 * children 2i+1 and 2i+2. Distinct from batch179 (int64 top-k PQ).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch246_marker[] = "libcgj-batch246";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b246_swap(uint64_t *pA, uint64_t *pB)
{
	uint64_t nTmp;

	nTmp = *pA;
	*pA = *pB;
	*pB = nTmp;
}

/*
 * Sift element at index i down toward leaves while it is larger than a child.
 * Restores min-heap order for a subtree rooted at i (array length n).
 */
static void
b246_sift_down(uint64_t *a, size_t n, size_t i)
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
		b246_swap(&a[i], &a[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heapify_min_u64 — build a binary min-heap in place over a[0..n).
 *
 * Floyd bottom-up: sift down from the last non-leaf (n/2 - 1) to root.
 * NULL a or n < 2 leaves the array unchanged.
 */
void
gj_heapify_min_u64(uint64_t *a, size_t n)
{
	size_t i;

	if (a == NULL || n < 2u) {
		return;
	}

	i = n / 2u;
	while (i > 0u) {
		i -= 1u;
		b246_sift_down(a, n, i);
	}
}

void __gj_heapify_min_u64(uint64_t *a, size_t n)
    __attribute__((alias("gj_heapify_min_u64")));

/*
 * gj_heappop_min_u64 — extract the minimum (root) into *out.
 *
 * Assumes a[0..*n) is already a min-heap (e.g. after gj_heapify_min_u64
 * or a prior sequence of heappops). On success: *out = a[0], *n decreased
 * by one, last leaf moved to root and sifted down. Returns 0 on success,
 * -1 if empty or any of a/n/out is NULL.
 */
int
gj_heappop_min_u64(uint64_t *a, size_t *n, uint64_t *out)
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
		b246_sift_down(a, nCur, 0u);
	}
	return 0;
}

int __gj_heappop_min_u64(uint64_t *a, size_t *n, uint64_t *out)
    __attribute__((alias("gj_heappop_min_u64")));
