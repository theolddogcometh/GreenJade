/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch320: Tarjan strongly connected components on
 * CSR-like adjacency lists (directed graph), stack-only for n <= 64.
 *
 * Surface (unique symbols):
 *   int gj_scc(size_t n, const size_t *off, const int *to,
 *              int *comp, int *ncomp);
 *     — Tarjan SCC of an n-vertex digraph given as CSR:
 *         edges of vertex u are to[off[u] .. off[u+1])
 *       (off has n+1 entries; vertices are 0 .. n-1). Writes dense
 *       component ids into comp[0..n) in [0, *ncomp). Component ids are
 *       assigned in reverse topological order of the condensation
 *       (SCCs finished first get lower ids). Returns 0 on success with
 *       *ncomp equal to the number of SCCs; returns -1 on NULL / invalid
 *       args or n > B320_MAX_N. n == 0 is success with *ncomp == 0.
 *   __gj_scc  (alias)
 *   __libcgj_batch320_marker = "libcgj-batch320"
 *
 * Algorithm (Tarjan 1972, iterative stack DFS — no recursion):
 *   - Discovery index / low-link / on-stack flags on fixed arrays.
 *   - Explicit DFS frame stack (vertex + next edge index) + Tarjan stack.
 *   - When low[u] == index[u], pop the SCC rooted at u and assign the
 *     next component id to every popped vertex.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Stack aux capped at B320_MAX_N vertices. No third-party source copied.
 * Distinct from Kahn topo-sort (batch223).
 */

#include <stddef.h>

const char __libcgj_batch320_marker[] = "libcgj-batch320";

/* Max vertices for stack DFS / Tarjan aux (no heap). */
#define B320_MAX_N 64u

/* Unvisited discovery sentinel. */
#define B320_UNDEF (-1)

/* DFS frame: vertex and next CSR edge index to resume. */
struct b320_frame {
	int nU;
	size_t iEdge;
};

/* ---- helpers ----------------------------------------------------------- */

static int
b320_min(int nA, int nB)
{
	return (nA < nB) ? nA : nB;
}

/*
 * b320_enter — first visit of nU: assign index/low, push Tarjan stack.
 * Returns 0 on success, -1 if Tarjan stack would overflow (should not
 * happen for n <= B320_MAX_N).
 */
static int
b320_enter(int nU, int *pTime, int *aIndex, int *aLow,
           unsigned char *aOnStack, int *aStack, size_t *pcStack)
{
	aIndex[(size_t)nU] = *pTime;
	aLow[(size_t)nU] = *pTime;
	(*pTime)++;
	if (*pcStack >= (size_t)B320_MAX_N) {
		return -1;
	}
	aStack[*pcStack] = nU;
	(*pcStack)++;
	aOnStack[(size_t)nU] = 1u;
	return 0;
}

/*
 * b320_pop_scc — Tarjan root: low[u] == index[u]. Pop the SCC containing
 * nRoot and assign nCompId to every member.
 */
static void
b320_pop_scc(int nRoot, int nCompId, int *comp, int *aStack,
             size_t *pcStack, unsigned char *aOnStack)
{
	int nW;

	do {
		if (*pcStack == 0u) {
			return;
		}
		(*pcStack)--;
		nW = aStack[*pcStack];
		aOnStack[(size_t)nW] = 0u;
		comp[(size_t)nW] = nCompId;
	} while (nW != nRoot);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scc — Tarjan strongly connected components (CSR digraph).
 *
 * n:     vertex count; must be <= B320_MAX_N (64). Zero is allowed.
 * off:   CSR row offsets, length n+1; edges of u live in [off[u], off[u+1]).
 * to:    CSR column indices (targets). May be NULL when every row is empty.
 * comp:  output component id per vertex (caller supplies n ints).
 * ncomp: on success, number of SCCs written (*ncomp in [0, n]).
 *
 * Invalid neighbor indices (to[e] < 0 or >= n) are ignored so a corrupt
 * edge cannot overflow stack arrays. Returns 0 on success, -1 on error
 * (does not set errno — freestanding).
 */
int
gj_scc(size_t n, const size_t *off, const int *to, int *comp, int *ncomp)
{
	int aIndex[B320_MAX_N];
	int aLow[B320_MAX_N];
	unsigned char aOnStack[B320_MAX_N];
	int aStack[B320_MAX_N];
	struct b320_frame aDfs[B320_MAX_N];
	size_t cStack;
	size_t cDfs;
	int nTime;
	int nCompId;
	size_t iNode;

	if (ncomp == NULL) {
		return -1;
	}
	*ncomp = 0;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B320_MAX_N) {
		return -1;
	}
	if (off == NULL || comp == NULL) {
		return -1;
	}

	/* Init discovery state. */
	for (iNode = 0u; iNode < n; iNode++) {
		aIndex[iNode] = B320_UNDEF;
		aLow[iNode] = B320_UNDEF;
		aOnStack[iNode] = 0u;
		comp[iNode] = B320_UNDEF;
	}

	/* Validate CSR rows up front (strictly non-decreasing off). */
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

	cStack = 0u;
	nTime = 0;
	nCompId = 0;

	for (iNode = 0u; iNode < n; iNode++) {
		if (aIndex[iNode] != B320_UNDEF) {
			continue;
		}

		/* Seed DFS at iNode. */
		if (b320_enter((int)iNode, &nTime, aIndex, aLow, aOnStack,
		               aStack, &cStack) != 0) {
			return -1;
		}
		cDfs = 0u;
		aDfs[cDfs].nU = (int)iNode;
		aDfs[cDfs].iEdge = off[iNode];
		cDfs++;

		while (cDfs > 0u) {
			struct b320_frame *pFr = &aDfs[cDfs - 1u];
			int nU = pFr->nU;
			size_t iEnd = off[(size_t)nU + 1u];

			if (pFr->iEdge < iEnd) {
				int nTo = to[pFr->iEdge];

				pFr->iEdge++;

				if (nTo < 0 || (size_t)nTo >= n) {
					continue;
				}

				if (aIndex[(size_t)nTo] == B320_UNDEF) {
					/* Tree edge: recurse. */
					if (b320_enter(nTo, &nTime, aIndex,
					               aLow, aOnStack, aStack,
					               &cStack) != 0) {
						return -1;
					}
					if (cDfs >= (size_t)B320_MAX_N) {
						return -1;
					}
					aDfs[cDfs].nU = nTo;
					aDfs[cDfs].iEdge = off[(size_t)nTo];
					cDfs++;
				} else if (aOnStack[(size_t)nTo] != 0u) {
					/* Back edge into open SCC. */
					aLow[(size_t)nU] = b320_min(
					    aLow[(size_t)nU],
					    aIndex[(size_t)nTo]);
				}
			} else {
				/* Finished all out-edges of nU. */
				if (aLow[(size_t)nU] == aIndex[(size_t)nU]) {
					b320_pop_scc(nU, nCompId, comp, aStack,
					             &cStack, aOnStack);
					nCompId++;
				}

				cDfs--;

				/* Propagate low-link to parent frame. */
				if (cDfs > 0u) {
					int nParent = aDfs[cDfs - 1u].nU;

					aLow[(size_t)nParent] = b320_min(
					    aLow[(size_t)nParent],
					    aLow[(size_t)nU]);
				}
			}
		}
	}

	*ncomp = nCompId;
	return 0;
}

int __gj_scc(size_t n, const size_t *off, const int *to, int *comp, int *ncomp)
    __attribute__((alias("gj_scc")));
