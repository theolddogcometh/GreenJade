/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch255: Bellman-Ford single-source shortest paths
 * on a directed edge list (negative weights allowed; detects cycles).
 *
 * Surface (unique symbols):
 *   int gj_bellman_ford(size_t n, size_t m, const int *u, const int *v,
 *                       const int64_t *w, size_t src, int64_t *dist);
 *     — O(n*m) Bellman-Ford on a digraph with n vertices (0 .. n-1) and
 *       m directed edges. Edge e is u[e] → v[e] with weight w[e].
 *       Weights may be negative. Writes dist[0..n): shortest-path distance
 *       from src, or B255_INF (INT64_MAX/4) if unreachable. dist[src] = 0
 *       on success (when no negative cycle is reported).
 *     Returns 0 on success (distances finite or B255_INF).
 *     Returns 1 if a negative cycle is reachable from src (dist holds the
 *       values after n-1 relaxation rounds; detection does not mutate further).
 *     Returns -1 on bad args: n > B255_MAX_N, m > B255_MAX_M, src >= n
 *       (when n > 0), dist NULL when n > 0, or (when m > 0) u/v/w NULL.
 *       n == 0 is a no-op success (0). Out-of-range edge endpoints are
 *       ignored (not an error).
 *   __gj_bellman_ford  (alias)
 *   __libcgj_batch255_marker = "libcgj-batch255"
 *
 * Algorithm (classic Bellman-Ford on an edge list):
 *   - Initialize dist[] to B255_INF, dist[src] = 0.
 *   - Relax all m edges for n-1 rounds (only from reachable tails).
 *   - One extra pass: if any reachable edge still improves a dist, a
 *     negative cycle is reachable from src → return 1.
 *   - Additions clamp into [-B255_INF, B255_INF] so signed int64 never
 *     overflows during relax.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 * Distinct from dense Dijkstra (batch224) and grid BFS (batch225).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch255_marker[] = "libcgj-batch255";

/* Max vertices / edges for the fixed freestanding surface. */
#define B255_MAX_N 64u
#define B255_MAX_M 512u

/*
 * Unreachable sentinel: INT64_MAX/4.
 * Leaves headroom so finite dist + weight can be formed without int64 wrap
 * when both operands stay within about ±INF.
 */
#define B255_INF ((int64_t)(INT64_MAX / 4))

/* ---- helpers ----------------------------------------------------------- */

/*
 * b255_add — signed distance + edge weight with clamp to ±B255_INF.
 *
 * Caller guarantees du is a finite tentative distance (du < B255_INF).
 * Uses modular uint64 add + overflow detect (no signed-overflow UB),
 * then clamps the mathematical sum into [-B255_INF, B255_INF].
 */
static int64_t
b255_add(int64_t du, int64_t nW)
{
	uint64_t uSum = (uint64_t)du + (uint64_t)nW;
	int64_t nSum = (int64_t)uSum;

	/* Signed overflow of du + nW → saturate to the matching INF side. */
	if (nW > 0 && nSum < du) {
		return B255_INF;
	}
	if (nW < 0 && nSum > du) {
		return -B255_INF;
	}
	if (nSum > B255_INF) {
		return B255_INF;
	}
	if (nSum < -B255_INF) {
		return -B255_INF;
	}
	return nSum;
}

/*
 * b255_edge_ok — true if endpoints are in [0, n).
 */
static int
b255_edge_ok(int nU, int nV, size_t n)
{
	if (nU < 0 || nV < 0) {
		return 0;
	}
	if ((size_t)nU >= n || (size_t)nV >= n) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bellman_ford — single-source shortest paths on a directed edge list.
 *
 * n <= 64, m <= 512. Allows negative edge weights; reports negative cycles
 * reachable from src. Caller supplies dist[n].
 */
int
gj_bellman_ford(size_t n, size_t m, const int *u, const int *v,
                const int64_t *w, size_t src, int64_t *dist)
{
	size_t i;
	size_t cRound;
	size_t e;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B255_MAX_N || m > (size_t)B255_MAX_M) {
		return -1;
	}
	if (dist == NULL) {
		return -1;
	}
	if (src >= n) {
		return -1;
	}
	if (m > 0u && (u == NULL || v == NULL || w == NULL)) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		dist[i] = B255_INF;
	}
	dist[src] = 0;

	/*
	 * n-1 full edge-list relax rounds. A shortest simple path has at
	 * most n-1 edges; further improvement implies a negative cycle
	 * (checked on the extra pass below).
	 */
	for (cRound = 1u; cRound < n; cRound++) {
		for (e = 0u; e < m; e++) {
			int nU = u[e];
			int nV = v[e];
			int64_t nCand;
			size_t iU;
			size_t iV;

			if (!b255_edge_ok(nU, nV, n)) {
				continue;
			}
			iU = (size_t)nU;
			iV = (size_t)nV;
			if (dist[iU] >= B255_INF) {
				continue;
			}
			nCand = b255_add(dist[iU], w[e]);
			if (nCand < dist[iV]) {
				dist[iV] = nCand;
			}
		}
	}

	/* Extra pass: any improving reachable edge ⇒ negative cycle. */
	for (e = 0u; e < m; e++) {
		int nU = u[e];
		int nV = v[e];
		int64_t nCand;
		size_t iU;
		size_t iV;

		if (!b255_edge_ok(nU, nV, n)) {
			continue;
		}
		iU = (size_t)nU;
		iV = (size_t)nV;
		if (dist[iU] >= B255_INF) {
			continue;
		}
		nCand = b255_add(dist[iU], w[e]);
		if (nCand < dist[iV]) {
			return 1;
		}
	}

	return 0;
}

int __gj_bellman_ford(size_t n, size_t m, const int *u, const int *v,
                      const int64_t *w, size_t src, int64_t *dist)
    __attribute__((alias("gj_bellman_ford")));
