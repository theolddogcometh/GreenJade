/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch497: freestanding top-k min-heap of uint64_t.
 *
 * Surface (unique symbols):
 *   void gj_topk_init(uint64_t *h, size_t *n, size_t k);
 *     — empty the heap: *n = 0. Caller-owned array h[0 .. k-1]; k is the
 *       capacity (top-k bound). NULL n is a no-op; h/k unused beyond API.
 *   int  gj_topk_offer(uint64_t *h, size_t *n, size_t k, uint64_t v);
 *     — offer v for membership in the top-k largest values:
 *         * if *n < k: append v, sift-up, return 1 (entered)
 *         * if full and v > h[0]: replace root (drop current min), sift-down,
 *           return 1 (entered)
 *         * else return 0 (rejected — not among the k largest seen so far)
 *       Root always holds the smallest of the retained top-k.
 *   __gj_topk_init / __gj_topk_offer  (aliases)
 *   __libcgj_batch497_marker = "libcgj-batch497"
 *
 * Layout: 0-based indices; parent (i-1)/2, children 2i+1 and 2i+2.
 * Distinct from batch179 (int64_t gj_heap_*) and batch246 (heapify/pop).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch497_marker[] = "libcgj-batch497";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b497_swap(uint64_t *pA, uint64_t *pB)
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
b497_sift_up(uint64_t *pH, size_t i)
{
	size_t iParent;

	while (i > 0u) {
		iParent = (i - 1u) / 2u;
		if (pH[i] >= pH[iParent]) {
			break;
		}
		b497_swap(&pH[i], &pH[iParent]);
		i = iParent;
	}
}

/*
 * Sift element at index i down toward leaves while it is larger than a child.
 * Restores min-heap order after replacing the root.
 */
static void
b497_sift_down(uint64_t *pH, size_t cN, size_t i)
{
	size_t iLeft;
	size_t iRight;
	size_t iSmall;

	for (;;) {
		iLeft = 2u * i + 1u;
		iRight = 2u * i + 2u;
		iSmall = i;

		if (iLeft < cN && pH[iLeft] < pH[iSmall]) {
			iSmall = iLeft;
		}
		if (iRight < cN && pH[iRight] < pH[iSmall]) {
			iSmall = iRight;
		}
		if (iSmall == i) {
			break;
		}
		b497_swap(&pH[i], &pH[iSmall]);
		i = iSmall;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topk_init — mark heap empty (*n = 0). Does not touch h[].
 * NULL n is a no-op. h and k are accepted for API symmetry with offer.
 */
void
gj_topk_init(uint64_t *pH, size_t *pN, size_t cK)
{
	(void)pH;
	(void)cK;
	if (pN == NULL) {
		return;
	}
	*pN = 0u;
}

void __gj_topk_init(uint64_t *pH, size_t *pN, size_t cK)
    __attribute__((alias("gj_topk_init")));

/*
 * gj_topk_offer — try to place v among the k largest values in the min-heap.
 *
 * Returns 1 if v entered the heap (append or replace-root), 0 if rejected
 * or on NULL h/n / k == 0. When full, only values strictly greater than the
 * current root (minimum of top-k) are accepted.
 */
int
gj_topk_offer(uint64_t *pH, size_t *pN, size_t cK, uint64_t u64V)
{
	size_t cCur;

	if (pH == NULL || pN == NULL || cK == 0u) {
		return 0;
	}

	cCur = *pN;
	if (cCur < cK) {
		pH[cCur] = u64V;
		*pN = cCur + 1u;
		b497_sift_up(pH, cCur);
		return 1;
	}

	/* full: keep only if v beats the current minimum (root) */
	if (u64V > pH[0]) {
		pH[0] = u64V;
		b497_sift_down(pH, cCur, 0u);
		return 1;
	}
	return 0;
}

int __gj_topk_offer(uint64_t *pH, size_t *pN, size_t cK, uint64_t u64V)
    __attribute__((alias("gj_topk_offer")));
