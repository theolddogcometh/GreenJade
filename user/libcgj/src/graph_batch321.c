/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch321: Ford-Fulkerson max flow via Edmonds-Karp
 * (BFS augmenting paths) on a dense residual capacity matrix, n <= 16.
 *
 * Surface (unique symbols):
 *   int64_t gj_maxflow_ek(size_t n, int64_t *cap, size_t s, size_t t);
 *     — Maximum s→t flow on a directed graph given as a mutable dense
 *       residual capacity matrix cap of length n*n:
 *         residual(u → v) lives at cap[u * n + v]
 *       Vertices are 0 .. n-1. Caller seeds cap with non-negative forward
 *       capacities (and usually zeros on reverse / unused edges). The
 *       routine mutates cap in place: each augmentation subtracts the
 *       bottleneck along the path and adds it on the reverse residual
 *       edges. Returns the total max flow as int64_t (>= 0).
 *       Returns -1 on bad args: n == 0, n > B321_MAX_N, cap NULL, or
 *       s/t out of range. s == t is valid and yields flow 0 (cap untouched).
 *       Only strictly positive residual entries are used as edges; negative
 *       cells are ignored as zero capacity.
 *   __gj_maxflow_ek  (alias)
 *   __libcgj_batch321_marker = "libcgj-batch321"
 *
 * Algorithm (Edmonds-Karp = Ford-Fulkerson + BFS shortest augmenting path):
 *   - While BFS finds a residual path s ↝ t, compute bottleneck b = min
 *     residual along the path, then for each edge u→v on the path:
 *       cap[u*n+v] -= b;  cap[v*n+u] += b;
 *   - Accumulate b into the total flow. Stack-only parent/queue/seen for
 *     n <= 16; O(V E^2) worst-case with V = n, E <= n^2.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch321_marker[] = "libcgj-batch321";

/* Max vertices for stack BFS aux and dense n*n residual (no heap). */
#define B321_MAX_N 16u

/* Sentinel parent: unvisited / no predecessor. */
#define B321_NIL (-1)

/* ---- helpers ----------------------------------------------------------- */

/*
 * b321_cap — residual capacity u → v at dense index u*n + v.
 */
static int64_t
b321_cap(const int64_t *pCap, size_t n, size_t iU, size_t iV)
{
	return pCap[iU * n + iV];
}

/*
 * b321_set_cap — write residual capacity u → v.
 */
static void
b321_set_cap(int64_t *pCap, size_t n, size_t iU, size_t iV, int64_t i64Val)
{
	pCap[iU * n + iV] = i64Val;
}

/*
 * b321_bfs — Edmonds-Karp BFS for a residual s->t path.
 *
 * Fills aParent[v] with the predecessor of v on the path from s (aParent[s]
 * left as B321_NIL). Uses aSeen bitflags and a FIFO aQueue.
 *
 * Returns 1 if t is reachable from s with positive residual edges, else 0.
 * Negative residual cells are treated as zero capacity.
 */
static int
b321_bfs(size_t n, const int64_t *pCap, size_t iS, size_t iT,
         int *aParent, unsigned char *aSeen, int *aQueue)
{
	size_t cHead;
	size_t cTail;
	size_t iNode;

	for (iNode = 0u; iNode < n; iNode++) {
		aParent[iNode] = B321_NIL;
		aSeen[iNode] = 0u;
	}

	aSeen[iS] = 1u;
	aQueue[0] = (int)iS;
	cHead = 0u;
	cTail = 1u;

	while (cHead < cTail) {
		size_t iU = (size_t)aQueue[cHead];
		size_t iV;

		cHead++;

		if (iU == iT) {
			return 1;
		}

		for (iV = 0u; iV < n; iV++) {
			int64_t i64R;

			if (aSeen[iV] != 0u) {
				continue;
			}
			i64R = b321_cap(pCap, n, iU, iV);
			if (i64R <= 0) {
				continue;
			}
			aSeen[iV] = 1u;
			aParent[iV] = (int)iU;
			aQueue[cTail] = (int)iV;
			cTail++;
		}
	}

	return (aSeen[iT] != 0u) ? 1 : 0;
}

/*
 * b321_bottleneck — minimum residual capacity along parent-chain path iS→iT.
 * Assumes aParent encodes a valid path ending at iT (as from b321_bfs).
 * Returns 0 if the path is empty / broken (should not happen on success).
 */
static int64_t
b321_bottleneck(size_t n, const int64_t *pCap, size_t iS, size_t iT,
                const int *aParent)
{
	int64_t i64B = 0;
	size_t iV = iT;
	int fFirst = 1;

	while (iV != iS) {
		int nU = aParent[iV];
		int64_t i64R;
		size_t iU;

		if (nU < 0) {
			return 0;
		}
		iU = (size_t)nU;
		i64R = b321_cap(pCap, n, iU, iV);
		if (i64R <= 0) {
			return 0;
		}
		if (fFirst != 0) {
			i64B = i64R;
			fFirst = 0;
		} else if (i64R < i64B) {
			i64B = i64R;
		}
		iV = iU;
	}

	return (fFirst != 0) ? 0 : i64B;
}

/*
 * b321_augment — push bottleneck i64B along the parent path iS→iT:
 *   residual forward -= b, residual reverse += b.
 */
static void
b321_augment(size_t n, int64_t *pCap, size_t iS, size_t iT,
             const int *aParent, int64_t i64B)
{
	size_t iV = iT;

	while (iV != iS) {
		int nU = aParent[iV];
		size_t iU;
		int64_t i64Fwd;
		int64_t i64Rev;

		if (nU < 0) {
			return;
		}
		iU = (size_t)nU;
		i64Fwd = b321_cap(pCap, n, iU, iV);
		i64Rev = b321_cap(pCap, n, iV, iU);
		b321_set_cap(pCap, n, iU, iV, i64Fwd - i64B);
		b321_set_cap(pCap, n, iV, iU, i64Rev + i64B);
		iV = iU;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_maxflow_ek — Edmonds-Karp max flow on a dense residual matrix.
 *
 * n:   vertex count in 1 .. B321_MAX_N (16).
 * cap: mutable residual capacities, length n*n, row-major u*n+v.
 * s:   source vertex index in [0, n).
 * t:   sink vertex index in [0, n).
 *
 * Returns total max flow (>= 0), or -1 on invalid arguments. Does not set
 * errno (freestanding). cap is left as the residual graph after the final
 * augmentation (or untouched when s == t or no path exists).
 */
int64_t
gj_maxflow_ek(size_t n, int64_t *cap, size_t s, size_t t)
{
	int aParent[B321_MAX_N];
	unsigned char aSeen[B321_MAX_N];
	int aQueue[B321_MAX_N];
	int64_t i64Flow;

	if (cap == NULL) {
		return (int64_t)-1;
	}
	if (n == 0u || n > (size_t)B321_MAX_N) {
		return (int64_t)-1;
	}
	if (s >= n || t >= n) {
		return (int64_t)-1;
	}

	/* No path of positive length when source equals sink. */
	if (s == t) {
		return (int64_t)0;
	}

	i64Flow = 0;

	/*
	 * Repeated BFS augmentations until t is unreachable in the residual
	 * graph. Each successful path contributes a positive bottleneck.
	 */
	while (b321_bfs(n, cap, s, t, aParent, aSeen, aQueue) != 0) {
		int64_t i64B = b321_bottleneck(n, cap, s, t, aParent);

		if (i64B <= 0) {
			/* Defensive: path without positive capacity → stop. */
			break;
		}
		b321_augment(n, cap, s, t, aParent, i64B);
		i64Flow += i64B;
	}

	return i64Flow;
}

int64_t __gj_maxflow_ek(size_t n, int64_t *cap, size_t s, size_t t)
    __attribute__((alias("gj_maxflow_ek")));
