/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch256: Floyd–Warshall all-pairs shortest paths on
 * small dense digraphs (n <= 32) given as a full int64 weight matrix.
 *
 * Surface (unique symbols):
 *   int gj_floyd_warshall(int64_t *w, size_t n);
 *     — O(n^3) Floyd–Warshall in place on an n×n row-major weight matrix.
 *       Edge/path costs are int64_t; missing edges and unreachable pairs
 *       use the large INF sentinel B256_INF (= INT64_MAX/4). After success,
 *       w[i*n + j] holds the shortest-path distance i→j, or B256_INF if
 *       still unreachable under the INF convention.
 *     Returns 0 on success.
 *     Returns -1 if n > B256_MAX_N, or w is NULL when n > 0.
 *       n == 0 is a no-op success (0).
 *   __gj_floyd_warshall  (alias)
 *   __libcgj_batch256_marker = "libcgj-batch256"
 *
 * Algorithm (classic dense Floyd–Warshall):
 *   for k, for i, for j:
 *     if either leg i→k or k→j is INF, skip (no overflow / no path).
 *     else cand = w[i,k] + w[k,j]; if cand < w[i,j], update.
 *   B256_INF = INT64_MAX/4 so two finite legs always sum without signed
 *   overflow (each < INF ⇒ sum < INT64_MAX/2).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 * Distinct from dense Dijkstra (batch224) and BFS grid (batch225).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch256_marker[] = "libcgj-batch256";

/* Max vertices (task bound; O(n^3) stays small). */
#define B256_MAX_N 32u

/*
 * Unreachable / missing-edge sentinel: INT64_MAX/4.
 * Two finite legs each strictly less than this sum without overflow.
 */
#define B256_INF ((int64_t)(INT64_MAX / 4))

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b256_is_inf — true if v is the INF sentinel (or larger).
 * Values >= B256_INF are treated as non-finite for path legs.
 */
static int
b256_is_inf(int64_t v)
{
	return v >= B256_INF;
}

/*
 * b256_relax — consider path i → k → j against current w[i,j].
 * Skips if either leg is INF. Safe add: both finite ⇒ no int64 overflow.
 * Updates w[i*n + j] when the candidate is strictly better.
 */
static void
b256_relax(int64_t *w, size_t n, size_t i, size_t k, size_t j)
{
	int64_t nIk = w[i * n + k];
	int64_t nKj = w[k * n + j];
	int64_t nCand;
	int64_t nIj;

	if (b256_is_inf(nIk) || b256_is_inf(nKj)) {
		return;
	}
	nCand = nIk + nKj;
	nIj = w[i * n + j];
	if (nCand < nIj) {
		w[i * n + j] = nCand;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_floyd_warshall — all-pairs shortest paths on a dense n×n digraph.
 *
 * In-place: w is both input adjacency (row-major) and output distance
 * matrix. n <= 32. Callers seed missing edges with B256_INF (= INT64_MAX/4).
 * Negative finite weights are allowed (standard FW); negative cycles are
 * not detected (distances may go arbitrarily low if present).
 */
int
gj_floyd_warshall(int64_t *w, size_t n)
{
	size_t k, i, j;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B256_MAX_N) {
		return -1;
	}
	if (w == NULL) {
		return -1;
	}

	for (k = 0u; k < n; k++) {
		for (i = 0u; i < n; i++) {
			for (j = 0u; j < n; j++) {
				b256_relax(w, n, i, k, j);
			}
		}
	}

	return 0;
}

int __gj_floyd_warshall(int64_t *w, size_t n)
    __attribute__((alias("gj_floyd_warshall")));
