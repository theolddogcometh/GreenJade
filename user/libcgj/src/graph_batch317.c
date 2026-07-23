/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch317: tree diameter via two BFS on undirected
 * CSR-like adjacency (n <= 64).
 *
 * Surface (unique symbols):
 *   int gj_tree_diameter(size_t n, const size_t *off, const int *to,
 *                        int *out_diam);
 *     — Diameter (longest simple path length in edges) of an undirected
 *       tree given as CSR:
 *         neighbors of vertex u are to[off[u] .. off[u+1])
 *       (off has n+1 entries; vertices are 0 .. n-1). Each undirected edge
 *       should appear twice (both directions). Uses the classic two-BFS
 *       method: BFS from 0 to find a farthest vertex u, then BFS from u;
 *       the eccentricity of u is the diameter. Writes the result into
 *       *out_diam. Returns 0 on success, -1 on NULL/invalid args or
 *       n > B317_MAX_N. n == 0 or n == 1 yields diameter 0.
 *   __gj_tree_diameter  (alias)
 *   __libcgj_batch317_marker = "libcgj-batch317"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Stack aux capped at B317_MAX_N vertices. No third-party source copied.
 * Distinct from grid BFS (batch225) and CSR topo-sort (batch223).
 */

#include <stddef.h>

const char __libcgj_batch317_marker[] = "libcgj-batch317";

/* Max vertices for stack dist + queue (task bound; freestanding). */
#define B317_MAX_N 64u

/* ---- helpers ----------------------------------------------------------- */

/*
 * b317_bfs_farthest — single-source BFS on undirected CSR.
 *
 * Fills aDist[0..n) with distances from nSrc (aDist[nSrc] == 0; -1 if
 * unreachable — should not occur in a connected tree). Writes the index
 * of a farthest vertex into *pFar and its distance into *pMaxDist.
 * aQ is scratch queue storage of length n.
 *
 * Invalid neighbor indices (to[e] < 0 or >= n) are skipped so a corrupt
 * edge cannot overflow the stack arrays.
 *
 * Returns 0 on success, -1 if CSR offsets for a dequeued vertex are
 * inverted (off[u+1] < off[u]).
 */
static int
b317_bfs_farthest(size_t n, const size_t *off, const int *to, int nSrc,
                  int *aDist, int *aQ, int *pFar, int *pMaxDist)
{
	size_t iNode;
	size_t cHead;
	size_t cTail;
	int nFar;
	int nMaxDist;

	for (iNode = 0u; iNode < n; iNode++) {
		aDist[iNode] = -1;
	}

	aDist[(size_t)nSrc] = 0;
	cHead = 0u;
	cTail = 0u;
	aQ[cTail] = nSrc;
	cTail++;

	nFar = nSrc;
	nMaxDist = 0;

	while (cHead < cTail) {
		int nU = aQ[cHead];
		size_t iEdge;
		size_t iBeg;
		size_t iEnd;
		int nDu;

		cHead++;
		nDu = aDist[(size_t)nU];
		if (nDu > nMaxDist) {
			nMaxDist = nDu;
			nFar = nU;
		}

		iBeg = off[(size_t)nU];
		iEnd = off[(size_t)nU + 1u];
		if (iEnd < iBeg) {
			return -1;
		}
		if (iEnd > iBeg && to == NULL) {
			return -1;
		}

		for (iEdge = iBeg; iEdge < iEnd; iEdge++) {
			int nTo = to[iEdge];

			if (nTo < 0 || (size_t)nTo >= n) {
				continue;
			}
			if (aDist[(size_t)nTo] >= 0) {
				continue;
			}
			aDist[(size_t)nTo] = nDu + 1;
			aQ[cTail] = nTo;
			cTail++;
		}
	}

	*pFar = nFar;
	*pMaxDist = nMaxDist;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tree_diameter — tree diameter via two BFS on undirected CSR.
 *
 * n:        vertex count; must be <= 64. Zero or one vertex → diameter 0.
 * off:      CSR row offsets, length n+1 (required when n > 0).
 * to:       neighbor targets; may be NULL only when every row is empty.
 * out_diam: on success, *out_diam = diameter in edges (>= 0).
 *
 * Algorithm:
 *   1. BFS from vertex 0 → farthest vertex u (an endpoint of a diameter).
 *   2. BFS from u → max distance is the diameter.
 *
 * Returns 0 on success, -1 on invalid arguments / inverted offsets /
 * n > B317_MAX_N. Does not set errno (freestanding).
 */
int
gj_tree_diameter(size_t n, const size_t *off, const int *to, int *out_diam)
{
	int aDist[B317_MAX_N];
	int aQ[B317_MAX_N];
	int nFar;
	int nMaxDist;
	int nSt;

	if (out_diam == NULL) {
		return -1;
	}
	*out_diam = 0;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B317_MAX_N) {
		return -1;
	}
	if (off == NULL) {
		return -1;
	}
	if (n == 1u) {
		*out_diam = 0;
		return 0;
	}

	/* First BFS: any vertex (0) → a diameter endpoint. */
	nSt = b317_bfs_farthest(n, off, to, 0, aDist, aQ, &nFar, &nMaxDist);
	if (nSt != 0) {
		return -1;
	}

	/* Second BFS: from that endpoint → diameter length. */
	nSt = b317_bfs_farthest(n, off, to, nFar, aDist, aQ, &nFar, &nMaxDist);
	if (nSt != 0) {
		return -1;
	}

	*out_diam = nMaxDist;
	return 0;
}

int __gj_tree_diameter(size_t n, const size_t *off, const int *to, int *out_diam)
    __attribute__((alias("gj_tree_diameter")));
