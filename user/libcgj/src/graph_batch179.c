/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch179: int64 min-heap for top-k selection.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party heap source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   heapsort / gj_heapsort  → graph_batch18.c / graph_batch100.c
 *   (generic array sort; this TU is a fixed int64_t priority queue.)
 *
 * This TU adds only unique symbols:
 *   void gj_heap_init(int64_t *h, size_t *n);
 *     — empty the heap: *n = 0. NULL args are no-ops.
 *   void gj_heap_push(int64_t *h, size_t *n, size_t cap, int64_t v);
 *     — min-heap insert for top-k (cap == k):
 *         * if *n < cap: append v and sift-up
 *         * if full and v > h[0]: replace root (drop current min) and sift-down
 *         * else ignore v (not among the k largest seen so far)
 *   int  gj_heap_pop(int64_t *h, size_t *n, int64_t *out);
 *     — remove and return the minimum (root). 0 on success, -1 if empty/NULL.
 *   __gj_heap_init / __gj_heap_push / __gj_heap_pop  (aliases)
 *   __libcgj_batch179_marker = "libcgj-batch179"
 *
 * Layout: caller-owned array h[0 .. cap-1]; *n is live element count.
 * Indices are 0-based: parent (i-1)/2, children 2i+1 and 2i+2.
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch179_marker[] = "libcgj-batch179";

/* ---- internal helpers -------------------------------------------------- */

static void
b179_swap(int64_t *pA, int64_t *pB)
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
b179_sift_up(int64_t *h, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (h[i] >= h[iParent]) {
			break;
		}
		b179_swap(&h[i], &h[iParent]);
		i = iParent;
	}
}

/*
 * Sift element at index i down toward leaves while it is larger than a child.
 * Restores min-heap order after replacing the root (or after a pop hole-fill).
 */
static void
b179_sift_down(int64_t *h, size_t n, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;

	for (;;) {
		iLeft = 2u * i + 1u;
		iRight = 2u * i + 2u;
		iSmall = i;

		if (iLeft < n && h[iLeft] < h[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < n && h[iRight] < h[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}
		b179_swap(&h[i], &h[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_init — mark heap empty (*n = 0). Does not touch h[].
 * NULL h or n is ignored (safe no-op).
 */
void
gj_heap_init(int64_t *h, size_t *n)
{
	(void)h;
	if (n == NULL) {
		return;
	}
	*n = 0u;
}

void __gj_heap_init(int64_t *h, size_t *n)
    __attribute__((alias("gj_heap_init")));

/*
 * gj_heap_push — insert v into a capacity-capped min-heap used for top-k.
 *
 * When the heap has fewer than cap elements, v is always accepted.
 * When full (n == cap > 0), v replaces the root only if v > h[0], so the
 * heap retains the cap largest values seen (min among them at the root).
 * cap == 0 or NULL h/n are no-ops.
 */
void
gj_heap_push(int64_t *h, size_t *n, size_t cap, int64_t v)
{
	size_t nCur;

	if (h == NULL || n == NULL || cap == 0u) {
		return;
	}

	nCur = *n;
	if (nCur < cap) {
		h[nCur] = v;
		*n = nCur + 1u;
		b179_sift_up(h, nCur);
		return;
	}

	/* full: keep only if v beats the current minimum (root) */
	if (v > h[0]) {
		h[0] = v;
		b179_sift_down(h, nCur, 0u);
	}
}

void __gj_heap_push(int64_t *h, size_t *n, size_t cap, int64_t v)
    __attribute__((alias("gj_heap_push")));

/*
 * gj_heap_pop — remove the minimum (root) into *out.
 *
 * Returns 0 on success, -1 if empty or any pointer is NULL.
 * After pop, the last leaf fills the hole and is sifted down.
 */
int
gj_heap_pop(int64_t *h, size_t *n, int64_t *out)
{
	size_t nCur;

	if (h == NULL || n == NULL || out == NULL) {
		return -1;
	}
	nCur = *n;
	if (nCur == 0u) {
		return -1;
	}

	*out = h[0];
	nCur -= 1u;
	*n = nCur;
	if (nCur > 0u) {
		h[0] = h[nCur];
		b179_sift_down(h, nCur, 0u);
	}
	return 0;
}

int __gj_heap_pop(int64_t *h, size_t *n, int64_t *out)
    __attribute__((alias("gj_heap_pop")));
