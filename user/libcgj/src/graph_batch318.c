/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch318: bipartite check via BFS 2-coloring on a
 * CSR-like adjacency list (undirected or directed out-edges).
 *
 * Surface (unique symbols):
 *   int gj_is_bipartite(size_t n, const size_t *off, const int *to);
 *     — Test whether an n-vertex graph is bipartite by BFS 2-coloring.
 *       CSR form: edges of vertex u are to[off[u] .. off[u+1])
 *       (off has n+1 entries; vertices are 0 .. n-1). Follows each listed
 *       out-neighbor; for undirected input, store both directions.
 *     Returns 1 if bipartite (including empty / n == 0).
 *     Returns 0 if an odd cycle (or self-loop) is found under coloring.
 *     Returns -1 on bad args: n > B318_MAX_N, off NULL when n > 0,
 *       off[u+1] < off[u], or to NULL when any edge span is non-empty.
 *       Out-of-range neighbor indices are treated as bad (-1).
 *   __gj_is_bipartite  (alias)
 *   __libcgj_batch318_marker = "libcgj-batch318"
 *
 * Algorithm (classic BFS 2-coloring over all components):
 *   - color[v] in {-1, 0, 1}: uncolored / side A / side B.
 *   - For each uncolored start s: color[s] = 0, BFS; neighbors get 1-color.
 *   - Same-color edge (including self-loop) → not bipartite (0).
 *   - Stack-only color[] + FIFO queue[]; n <= 64.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from CSR topo-sort (batch223).
 */

#include <stddef.h>

const char __libcgj_batch318_marker[] = "libcgj-batch318";

/* Max vertices for stack color + queue (no heap). */
#define B318_MAX_N 64u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_bipartite — BFS 2-color bipartiteness test on CSR adjacency.
 *
 * n:   vertex count (0 .. B318_MAX_N). n == 0 → bipartite (1).
 * off: offset array of length n+1; edge range for u is [off[u], off[u+1]).
 * to:  neighbor array; required only when some range is non-empty.
 *
 * Returns 1 (yes), 0 (no), or -1 (bad input).
 */
int
gj_is_bipartite(size_t n, const size_t *off, const int *to)
{
	/* -1 = uncolored, 0 / 1 = partition side */
	signed char aColor[B318_MAX_N];
	int aQueue[B318_MAX_N];
	size_t iNode;

	if (n == 0u) {
		return 1;
	}
	if (n > (size_t)B318_MAX_N) {
		return -1;
	}
	if (off == NULL) {
		return -1;
	}

	/* Validate CSR ranges and require to when edges exist. */
	for (iNode = 0u; iNode < n; iNode++) {
		size_t iBeg = off[iNode];
		size_t iEnd = off[iNode + 1u];

		if (iEnd < iBeg) {
			return -1;
		}
		if (iEnd > iBeg && to == NULL) {
			return -1;
		}
	}

	for (iNode = 0u; iNode < n; iNode++) {
		aColor[iNode] = (signed char)-1;
	}

	/*
	 * Multi-source BFS over all components: start a new BFS at each
	 * still-uncolored vertex, assign side 0, and color neighbors with
	 * the opposite side.
	 */
	for (iNode = 0u; iNode < n; iNode++) {
		size_t cHead;
		size_t cTail;

		if (aColor[iNode] != (signed char)-1) {
			continue;
		}

		aColor[iNode] = 0;
		aQueue[0] = (int)iNode;
		cHead = 0u;
		cTail = 1u;

		while (cHead < cTail) {
			int nU = aQueue[cHead];
			signed char cU;
			size_t iEdge;
			size_t iBeg;
			size_t iEnd;

			cHead++;
			cU = aColor[(size_t)nU];
			iBeg = off[(size_t)nU];
			iEnd = off[(size_t)nU + 1u];

			for (iEdge = iBeg; iEdge < iEnd; iEdge++) {
				int nTo = to[iEdge];
				signed char cWant;

				if (nTo < 0 || (size_t)nTo >= n) {
					return -1;
				}

				/* Self-loop: odd cycle of length 1. */
				if (nTo == nU) {
					return 0;
				}

				cWant = (signed char)(1 - (int)cU);
				if (aColor[(size_t)nTo] == (signed char)-1) {
					aColor[(size_t)nTo] = cWant;
					aQueue[cTail] = nTo;
					cTail++;
				} else if (aColor[(size_t)nTo] != cWant) {
					return 0;
				}
			}
		}
	}

	return 1;
}

int __gj_is_bipartite(size_t n, const size_t *off, const int *to)
    __attribute__((alias("gj_is_bipartite")));
