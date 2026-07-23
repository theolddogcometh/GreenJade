/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch224: Dijkstra single-source shortest paths on
 * small dense digraphs (n <= 64) given as a full weight matrix.
 *
 * Surface (unique symbols):
 *   int gj_dijkstra_dense(size_t n, const int64_t *w, size_t src,
 *                         int64_t *dist);
 *     — O(n^2) Dijkstra on a directed graph with n vertices (0 .. n-1).
 *       w is an n*n row-major weight matrix: edge u→v has cost w[u*n + v].
 *       Edge present iff 0 <= w[u*n+v] < B224_INF; negative and B224_INF
 *       entries mean "no edge". Zero-weight edges are allowed. Weights
 *       must be non-negative (negatives are treated as absent, not as
 *       negative-cost edges). Writes dist[0..n): shortest-path distance
 *       from src, or B224_INF if unreachable. dist[src] = 0 on success.
 *     Returns 0 on success.
 *     Returns -1 if n > B224_MAX_N, src >= n (when n > 0), or w/dist is
 *       NULL when n > 0. n == 0 is a no-op success (0).
 *   __gj_dijkstra_dense  (alias)
 *   __libcgj_batch224_marker = "libcgj-batch224"
 *
 * Algorithm (classic dense Dijkstra, no heap):
 *   - Stack-only: uint64_t done bitset (one bit per vertex) + dist[].
 *   - n times: pick unvisited u with minimal dist[u]; mark done; relax
 *     all out-edges u→v with non-negative finite weight.
 *   - Additions clamp to B224_INF on overflow so sums never wrap negative.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 * Distinct from CSR topo-sort (batch223) and matrix multiply (batch226).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch224_marker[] = "libcgj-batch224";

/* Max vertices: fits a done-mask in one uint64_t. */
#define B224_MAX_N 64u

/* Unreachable / no-edge sentinel (INT64_MAX). */
#define B224_INF ((int64_t)0x7fffffffffffffffLL)

/* ---- helpers ----------------------------------------------------------- */

/*
 * Saturated add for non-negative a,b in [0, B224_INF]:
 * returns min(a+b, B224_INF) without signed overflow.
 */
static int64_t
b224_add_sat(int64_t a, int64_t b)
{
	if (a >= B224_INF || b >= B224_INF) {
		return B224_INF;
	}
	/* a,b in [0, INF); INF - a is the room left before saturating. */
	if (b > B224_INF - a) {
		return B224_INF;
	}
	return a + b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dijkstra_dense — single-source shortest paths on a dense n×n digraph.
 *
 * n <= 64, stack bitset for the settled set. Caller supplies dist[n].
 * On success every dist[v] is either a finite non-negative distance or
 * B224_INF (unreachable from src under the non-negative edge convention).
 */
int
gj_dijkstra_dense(size_t n, const int64_t *w, size_t src, int64_t *dist)
{
	uint64_t uDone;
	size_t i;
	size_t cStep;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B224_MAX_N) {
		return -1;
	}
	if (w == NULL || dist == NULL) {
		return -1;
	}
	if (src >= n) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		dist[i] = B224_INF;
	}
	dist[src] = 0;

	uDone = 0ull;

	/*
	 * At most n settle steps. Each step selects the unsettled vertex
	 * with smallest tentative distance (ties: lowest index).
	 */
	for (cStep = 0u; cStep < n; cStep++) {
		size_t uBest = n;
		int64_t nBest = B224_INF;
		size_t v;

		for (i = 0u; i < n; i++) {
			if ((uDone & (1ull << i)) != 0ull) {
				continue;
			}
			if (dist[i] < nBest) {
				nBest = dist[i];
				uBest = i;
			}
		}

		/* All remaining vertices are unreachable. */
		if (uBest >= n || nBest >= B224_INF) {
			break;
		}

		uDone |= (1ull << uBest);

		/* Relax every outgoing edge from uBest. */
		for (v = 0u; v < n; v++) {
			int64_t nW = w[uBest * n + v];
			int64_t nCand;

			if ((uDone & (1ull << v)) != 0ull) {
				continue;
			}
			/* Missing edge: negative or INF weight. */
			if (nW < 0 || nW >= B224_INF) {
				continue;
			}
			nCand = b224_add_sat(dist[uBest], nW);
			if (nCand < dist[v]) {
				dist[v] = nCand;
			}
		}
	}

	return 0;
}

int __gj_dijkstra_dense(size_t n, const int64_t *w, size_t src, int64_t *dist)
    __attribute__((alias("gj_dijkstra_dense")));
