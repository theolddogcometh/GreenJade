/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch223: Kahn topological sort on CSR-like
 * adjacency lists (directed graph).
 *
 * Surface (unique symbols):
 *   int gj_topo_sort(size_t n, const size_t *off, const int *to,
 *                    int *order, size_t *norder);
 *     — Kahn BFS topo-order of an n-vertex digraph given as CSR:
 *         edges of vertex u are to[off[u] .. off[u+1])
 *       (off has n+1 entries; vertices are 0 .. n-1). Writes a linear
 *       extension into order[0 .. *norder). Returns 0 if the graph is
 *       a DAG and every vertex appears (*norder == n); returns -1 if a
 *       cycle remains (or on NULL / invalid args, n > B223_MAX_N).
 *       On cycle, *norder is the count of vertices that were ordered
 *       before stuck (strictly < n).
 *   __gj_topo_sort  (alias)
 *   __libcgj_batch223_marker = "libcgj-batch223"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. Stack aux capped at
 * B223_MAX_N vertices. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch223_marker[] = "libcgj-batch223";

/* Max vertices for stack indegree + queue (no heap). */
#define B223_MAX_N 256u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topo_sort — Kahn topological sort.
 *
 * 1. Count indegrees from every CSR out-edge.
 * 2. Seed a FIFO queue with all indegree-0 vertices.
 * 3. Repeatedly dequeue u, append to order, and decrement indegree of
 *    each successor; enqueue successors that reach 0.
 * 4. If fewer than n vertices were emitted, a cycle (or unreachable
 *    component from the indegree-0 frontier) remains → -1.
 *
 * Invalid neighbor indices (to[e] < 0 or >= n) are ignored for indegree
 * updates so a corrupt edge cannot overflow the stack arrays.
 */
int
gj_topo_sort(size_t n, const size_t *off, const int *to, int *order,
             size_t *norder)
{
	size_t aIndeg[B223_MAX_N];
	int aQueue[B223_MAX_N];
	size_t iNode;
	size_t cHead;
	size_t cTail;
	size_t cOut;

	if (norder == NULL) {
		return -1;
	}
	*norder = 0u;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B223_MAX_N) {
		return -1;
	}
	if (off == NULL || order == NULL) {
		return -1;
	}

	/* Zero indegrees. */
	for (iNode = 0u; iNode < n; iNode++) {
		aIndeg[iNode] = 0u;
	}

	/* Count incoming edges (skip out-of-range targets). */
	for (iNode = 0u; iNode < n; iNode++) {
		size_t iEdge;
		size_t iBeg = off[iNode];
		size_t iEnd = off[iNode + 1u];

		if (iEnd < iBeg) {
			return -1;
		}
		if (iEnd > iBeg && to == NULL) {
			return -1;
		}
		for (iEdge = iBeg; iEdge < iEnd; iEdge++) {
			int nTo = to[iEdge];

			if (nTo >= 0 && (size_t)nTo < n) {
				aIndeg[(size_t)nTo]++;
			}
		}
	}

	/* Seed queue with sources (indegree 0). */
	cHead = 0u;
	cTail = 0u;
	for (iNode = 0u; iNode < n; iNode++) {
		if (aIndeg[iNode] == 0u) {
			aQueue[cTail] = (int)iNode;
			cTail++;
		}
	}

	cOut = 0u;
	while (cHead < cTail) {
		int nU = aQueue[cHead];
		size_t iEdge;
		size_t iBeg;
		size_t iEnd;

		cHead++;
		order[cOut] = nU;
		cOut++;

		iBeg = off[(size_t)nU];
		iEnd = off[(size_t)nU + 1u];
		for (iEdge = iBeg; iEdge < iEnd; iEdge++) {
			int nTo = to[iEdge];

			if (nTo < 0 || (size_t)nTo >= n) {
				continue;
			}
			aIndeg[(size_t)nTo]--;
			if (aIndeg[(size_t)nTo] == 0u) {
				aQueue[cTail] = nTo;
				cTail++;
			}
		}
	}

	*norder = cOut;
	if (cOut != n) {
		return -1; /* cycle (or remaining non-zero indegree) */
	}
	return 0;
}

int __gj_topo_sort(size_t n, const size_t *off, const int *to, int *order,
                   size_t *norder)
    __attribute__((alias("gj_topo_sort")));
