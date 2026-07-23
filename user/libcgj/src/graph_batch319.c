/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch319: find bridges (Tarjan) on an undirected
 * CSR adjacency list.
 *
 * Surface (unique symbols):
 *   int gj_find_bridges(size_t n, const size_t *off, const int *to,
 *                       int *bu, int *bv, size_t *nb, size_t cap);
 *     — Tarjan DFS bridge finding on an n-vertex undirected graph given
 *       as CSR: edges of vertex u are to[off[u] .. off[u+1])
 *       (off has n+1 entries; vertices are 0 .. n-1). Caller represents
 *       each undirected edge in both directions. Writes bridge endpoints
 *       into bu[i], bv[i] for i in [0, min(found, cap)). *nb is always
 *       the total number of bridges discovered (may exceed cap).
 *       Returns 0 if every bridge fit in cap (or none exist);
 *       returns 1 if truncated (found > cap, only cap pairs written);
 *       returns -1 on NULL / invalid args or n > B319_MAX_N.
 *       n == 0 is success with *nb == 0. cap == 0 with bu/bv NULL is a
 *       count-only pass. Self-loops are ignored; parallel edges between
 *       the same pair are handled (only the tree-edge reverse is skipped
 *       once so a double edge is not reported as a bridge).
 *   __gj_find_bridges  (alias)
 *   __libcgj_batch319_marker = "libcgj-batch319"
 *
 * Algorithm (Tarjan discovery / low-link on undirected graphs):
 *   disc[u] = discovery time; low[u] = earliest disc reachable from the
 *   subtree of u via tree edges plus a single back edge. Tree edge u→v
 *   is a bridge iff low[v] > disc[u]. Disconnected components are each
 *   started with parent = -1.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Stack aux capped at B319_MAX_N vertices. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch319_marker[] = "libcgj-batch319";

/* Max vertices for stack disc/low and recursive DFS depth (no heap). */
#define B319_MAX_N 64u

/* ---- context for DFS --------------------------------------------------- */

struct b319_ctx {
	size_t n;
	const size_t *off;
	const int *to;
	int *bu;
	int *bv;
	size_t cap;
	size_t cFound;
	int aDisc[B319_MAX_N];
	int aLow[B319_MAX_N];
	int nTimer;
};

/*
 * b319_min_i — integer min (avoids a macro and signed/unsigned noise).
 */
static int
b319_min_i(int nA, int nB)
{
	return (nA < nB) ? nA : nB;
}

/*
 * b319_record — append bridge (nU, nV) if room remains under cap.
 * Always increments cFound so *nb can report the true total.
 */
static void
b319_record(struct b319_ctx *pCtx, int nU, int nV)
{
	if (pCtx->cFound < pCtx->cap && pCtx->bu != NULL && pCtx->bv != NULL) {
		pCtx->bu[pCtx->cFound] = nU;
		pCtx->bv[pCtx->cFound] = nV;
	}
	pCtx->cFound++;
}

/*
 * b319_dfs — Tarjan DFS from nU with tree-parent nParent (-1 at roots).
 *
 * Multigraph note: the reverse of the tree edge nParent→nU is skipped
 * only once (fParentSeen). A second CSR entry to nParent is treated as a
 * true back edge so parallel edges correctly prevent a false bridge.
 */
static void
b319_dfs(struct b319_ctx *pCtx, int nU, int nParent)
{
	size_t iU = (size_t)nU;
	size_t iEdge;
	size_t iBeg;
	size_t iEnd;
	int fParentSeen = 0;

	pCtx->nTimer++;
	pCtx->aDisc[iU] = pCtx->nTimer;
	pCtx->aLow[iU] = pCtx->nTimer;

	iBeg = pCtx->off[iU];
	iEnd = pCtx->off[iU + 1u];
	if (iEnd < iBeg) {
		return;
	}
	if (iEnd > iBeg && pCtx->to == NULL) {
		return;
	}

	for (iEdge = iBeg; iEdge < iEnd; iEdge++) {
		int nV = pCtx->to[iEdge];
		size_t iV;

		if (nV < 0 || (size_t)nV >= pCtx->n) {
			continue;
		}
		if (nV == nU) {
			/* Self-loop: never a bridge. */
			continue;
		}
		iV = (size_t)nV;

		if (pCtx->aDisc[iV] == 0) {
			/* Tree edge. */
			b319_dfs(pCtx, nV, nU);
			pCtx->aLow[iU] = b319_min_i(pCtx->aLow[iU],
			    pCtx->aLow[iV]);
			if (pCtx->aLow[iV] > pCtx->aDisc[iU]) {
				b319_record(pCtx, nU, nV);
			}
		} else if (nV == nParent) {
			/*
			 * First occurrence is the reverse of the tree edge;
			 * further occurrences are parallel back edges.
			 */
			if (fParentSeen == 0) {
				fParentSeen = 1;
				continue;
			}
			pCtx->aLow[iU] = b319_min_i(pCtx->aLow[iU],
			    pCtx->aDisc[iV]);
		} else {
			/* Back edge to an ancestor (or cross in same DFS). */
			pCtx->aLow[iU] = b319_min_i(pCtx->aLow[iU],
			    pCtx->aDisc[iV]);
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_find_bridges — list bridges of an undirected CSR graph (Tarjan).
 *
 * n:   vertex count; must be <= B319_MAX_N (64). Zero is empty success.
 * off: CSR offsets, length n+1; edges of u are to[off[u] .. off[u+1]).
 * to:  neighbor array (required when any adjacency list is non-empty).
 * bu:  out bridge endpoint u (tree parent side); may be NULL if cap == 0.
 * bv:  out bridge endpoint v (tree child side);  may be NULL if cap == 0.
 * nb:  out total bridge count discovered (not clamped to cap). Required.
 * cap: max pairs writable into bu/bv.
 *
 * Returns:
 *   0  — success; *nb <= cap (all bridges written, or none)
 *   1  — success with truncation; *nb > cap (only cap pairs written)
 *  -1  — invalid arguments
 */
int
gj_find_bridges(size_t n, const size_t *off, const int *to, int *bu, int *bv,
    size_t *nb, size_t cap)
{
	struct b319_ctx ctx;
	size_t iNode;

	if (nb == NULL) {
		return -1;
	}
	*nb = 0u;

	if (n == 0u) {
		return 0;
	}
	if (n > (size_t)B319_MAX_N) {
		return -1;
	}
	if (off == NULL) {
		return -1;
	}
	if (cap > 0u && (bu == NULL || bv == NULL)) {
		return -1;
	}

	/* Validate CSR ranges and require to when any list is non-empty. */
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

	ctx.n = n;
	ctx.off = off;
	ctx.to = to;
	ctx.bu = bu;
	ctx.bv = bv;
	ctx.cap = cap;
	ctx.cFound = 0u;
	ctx.nTimer = 0;

	for (iNode = 0u; iNode < n; iNode++) {
		ctx.aDisc[iNode] = 0;
		ctx.aLow[iNode] = 0;
	}

	for (iNode = 0u; iNode < n; iNode++) {
		if (ctx.aDisc[iNode] == 0) {
			b319_dfs(&ctx, (int)iNode, -1);
		}
	}

	*nb = ctx.cFound;
	if (ctx.cFound > cap) {
		return 1;
	}
	return 0;
}

int __gj_find_bridges(size_t n, const size_t *off, const int *to, int *bu,
    int *bv, size_t *nb, size_t cap)
    __attribute__((alias("gj_find_bridges")));
