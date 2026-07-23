/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch322: minimum-cost assignment on a dense square
 * cost matrix (bipartite matching / assignment problem) for n <= 8.
 *
 * Surface (unique symbols):
 *   int gj_assign_min_cost(size_t n, const int64_t *cost, int *match_row,
 *                          int64_t *out_cost);
 *     — Find a minimum-cost perfect matching on the complete bipartite
 *       graph K_{n,n} with edge costs given by a dense n x n matrix:
 *         cost[row * n + col] = cost of assigning row -> col
 *       Writes match_row[row] = chosen column (bijection), and
 *       *out_cost = sum of selected edge costs. Returns 0 on success;
 *       returns -1 on NULL / invalid args or n > B322_MAX_N.
 *       n == 0 is success with *out_cost == 0 (match_row may be NULL).
 *   __gj_assign_min_cost  (alias)
 *   __libcgj_batch322_marker = "libcgj-batch322"
 *
 * Algorithm: exhaustive column-permutation search via DFS backtracking
 * with a used-column bitmask (n! leaves; 8! = 40320). Stack-only aux.
 * Correct for any int64 edge costs (including negatives). Ties keep the
 * first minimum found under the natural search order.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch322_marker[] = "libcgj-batch322";

/* Max dimension: n! stays tiny; stack aux is O(n). */
#define B322_MAX_N 8u

/* ---- recursive search -------------------------------------------------- */

/*
 * b322_search — assign rows [iRow .. n) to free columns.
 *
 * pCost:      dense n x n costs (row-major).
 * n:          dimension.
 * uUsed:      bitmask of columns already taken (bit c set => col c used).
 * i64Partial: sum of costs for rows [0 .. iRow).
 * aCur:       current assignment aCur[row] = col.
 * aBest:      best assignment found so far.
 * pi64Best:   best total cost; INT64_MAX until first complete assignment.
 */
static void
b322_search(const int64_t *pCost, size_t n, unsigned uUsed, size_t iRow,
            int64_t i64Partial, int *aCur, int *aBest, int64_t *pi64Best)
{
	size_t iCol;

	if (iRow == n) {
		if (i64Partial < *pi64Best) {
			*pi64Best = i64Partial;
			for (iCol = 0u; iCol < n; iCol++) {
				aBest[iCol] = aCur[iCol];
			}
		}
		return;
	}

	for (iCol = 0u; iCol < n; iCol++) {
		unsigned uBit;
		int64_t i64Next;

		uBit = 1u << (unsigned)iCol;
		if ((uUsed & uBit) != 0u) {
			continue;
		}

		/* No partial prune: costs may be negative. */
		i64Next = i64Partial + pCost[iRow * n + iCol];
		aCur[iRow] = (int)iCol;
		b322_search(pCost, n, uUsed | uBit, iRow + 1u, i64Next,
		            aCur, aBest, pi64Best);
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assign_min_cost — minimum-cost assignment (dense n x n, n <= 8).
 *
 * n:         matrix order (0 .. B322_MAX_N).
 * cost:      row-major costs; required when n > 0.
 * match_row: on success, match_row[i] = column matched to row i (length n).
 * out_cost:  on success, total assignment cost (required when n == 0 too).
 *
 * Returns 0 on success, -1 on bad input (freestanding, no errno).
 */
int
gj_assign_min_cost(size_t n, const int64_t *cost, int *match_row,
                   int64_t *out_cost)
{
	int aCur[B322_MAX_N];
	int aBest[B322_MAX_N];
	int64_t i64Best;
	size_t i;

	if (out_cost == NULL) {
		return -1;
	}
	*out_cost = 0;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B322_MAX_N) {
		return -1;
	}
	if (cost == NULL || match_row == NULL) {
		return -1;
	}

	i64Best = (int64_t)0x7fffffffffffffffLL;
	for (i = 0u; i < n; i++) {
		aCur[i] = -1;
		aBest[i] = -1;
	}

	b322_search(cost, n, 0u, 0u, 0, aCur, aBest, &i64Best);

	/* A complete assignment always exists for square dense K_{n,n}. */
	for (i = 0u; i < n; i++) {
		match_row[i] = aBest[i];
	}
	*out_cost = i64Best;
	return 0;
}

int __gj_assign_min_cost(size_t n, const int64_t *cost, int *match_row,
                         int64_t *out_cost)
    __attribute__((alias("gj_assign_min_cost")));
