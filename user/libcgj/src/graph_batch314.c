/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch314: Mo's algorithm query-index order.
 *
 * Surface (unique symbols):
 *   int gj_mo_order(size_t n, size_t q, const size_t *L, const size_t *R,
 *                   size_t *order, size_t block);
 *     — Write a permutation of query indices 0..q-1 into order[0..q) that
 *       is sorted for classic Mo processing of offline range queries on an
 *       array of length n. Primary key is left-endpoint block L[i]/block;
 *       secondary key is right endpoint R[i] ascending. Caps q at 256 so
 *       a stack insertion sort stays freestanding (no malloc). Returns 0
 *       on success, -1 on invalid arguments.
 *   __gj_mo_order  (alias)
 *   __libcgj_batch314_marker = "libcgj-batch314"
 *
 * Mo (Mo's algorithm) processes offline queries [L[i], R[i]] by walking a
 * sliding window across the array. Ordering by (L/B, R) keeps left endpoints
 * in the same block clustered and right endpoints monotonic within a block,
 * which bounds total add/remove moves to roughly O((n+q)·sqrt(n)) when
 * block ≈ sqrt(n) (caller chooses block).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch314_marker[] = "libcgj-batch314";

/* Max number of queries (stack-friendly insertion sort). */
#define B314_Q_MAX 256u

/* ---- helpers ----------------------------------------------------------- */

/*
 * b314_mo_less — true if query iA should precede iB in Mo order.
 *
 * Order: (L[i]/block, R[i]) lexicographic ascending. Indices themselves
 * are not part of the key; insertion sort keeps equal keys stable.
 */
static int
b314_mo_less(size_t iA, size_t iB, const size_t *pL, const size_t *pR,
    size_t uBlock)
{
	size_t uBlkA = pL[iA] / uBlock;
	size_t uBlkB = pL[iB] / uBlock;

	if (uBlkA != uBlkB) {
		return (uBlkA < uBlkB) ? 1 : 0;
	}
	return (pR[iA] < pR[iB]) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mo_order — permutation of query indices for Mo's algorithm.
 *
 * n:     length of the underlying array; each query must satisfy
 *        L[i] <= R[i] < n (inclusive endpoints, 0-based).
 * q:     number of queries in L/R; must be in [0, 256].
 * L, R:  left and right inclusive endpoints for each query.
 * order: caller-owned buffer of at least q size_t elements; on success
 *        holds a permutation of 0..q-1 in Mo process order.
 * block: Mo block size B > 0; left key is L[i]/B. Typical choice is
 *        roughly sqrt(n); this helper does not compute B for the caller.
 *
 * Returns:
 *   0  success (q == 0 is success and a no-op write)
 *  -1  invalid: q > 256, block == 0, NULL L/R/order when q > 0, or a
 *      query with L[i] > R[i] or R[i] >= n
 *
 * Does not set errno (freestanding).
 */
int
gj_mo_order(size_t n, size_t q, const size_t *L, const size_t *R,
    size_t *order, size_t block)
{
	size_t i;
	size_t j;

	if (q > (size_t)B314_Q_MAX) {
		return -1;
	}
	if (block == 0u) {
		return -1;
	}
	if (q == 0u) {
		return 0;
	}
	if (L == NULL || R == NULL || order == NULL) {
		return -1;
	}

	/* Endpoint checks also consume n so -Wextra is clean. */
	for (i = 0u; i < q; i++) {
		if (L[i] > R[i] || R[i] >= n) {
			return -1;
		}
	}

	/* Identity permutation, then insertion-sort by Mo keys. */
	for (i = 0u; i < q; i++) {
		order[i] = i;
	}

	for (i = 1u; i < q; i++) {
		size_t uIdx = order[i];

		j = i;
		while (j > 0u &&
		    b314_mo_less(uIdx, order[j - 1u], L, R, block)) {
			order[j] = order[j - 1u];
			j--;
		}
		order[j] = uIdx;
	}

	return 0;
}

int __gj_mo_order(size_t n, size_t q, const size_t *L, const size_t *R,
    size_t *order, size_t block) __attribute__((alias("gj_mo_order")));
