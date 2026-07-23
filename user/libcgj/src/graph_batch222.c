/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch222: disjoint-set / union-find with path
 * compression and union-by-rank. Clean-room freestanding pure C —
 * integer/pointer only. Compiles with -ffreestanding -msse2. No malloc,
 * no errno. No third-party source copied.
 *
 * Surface (unique symbols):
 *   void gj_uf_init(int *p, size_t n);
 *     — Initialize parent array p[0..n): each element is its own root
 *       (p[i] = i). Caller supplies storage; n may be 0 (no-op).
 *   int  gj_uf_find(int *p, int x);
 *     — Find root of x with full path compression (recursive two-pass
 *       style: compress on unwind so every node on the path points to
 *       the root). Undefined if x is out of range for the parent array.
 *   void gj_uf_union(int *p, int *rank, int a, int b);
 *     — Unite sets containing a and b by rank. rank[] must be sized like
 *       p[] and zero-initialized for fresh sets (rank of a singleton is
 *       0). No-op when a and b already share a root.
 *   __gj_uf_init / __gj_uf_find / __gj_uf_union  (aliases)
 *   __libcgj_batch222_marker = "libcgj-batch222"
 */

#include <stddef.h>

const char __libcgj_batch222_marker[] = "libcgj-batch222";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uf_init — make every index a singleton set.
 *
 * For i in [0, n): p[i] = i. Rank array (if used later) should be zeroed
 * by the caller (calloc / explicit loop); this function only touches p.
 */
void
gj_uf_init(int *p, size_t n)
{
	size_t i;

	if (p == NULL) {
		return;
	}
	for (i = 0u; i < n; i++) {
		p[i] = (int)i;
	}
}

/*
 * gj_uf_find — root of x with path compression.
 *
 * While p[x] != x, walk to the parent; on return, set p[x] to the root
 * so subsequent finds on this path are O(1) amortized (with union-by-rank
 * overall α(n) — inverse Ackermann).
 */
int
gj_uf_find(int *p, int x)
{
	if (p == NULL) {
		return x;
	}
	if (p[x] != x) {
		p[x] = gj_uf_find(p, p[x]);
	}
	return p[x];
}

/*
 * gj_uf_union — link roots of a and b by rank.
 *
 * Find ra = find(a), rb = find(b). If equal, already united. Otherwise
 * attach the lower-rank tree under the higher-rank root; if ranks are
 * equal, attach rb under ra and increment rank[ra].
 *
 * rank may be NULL only if the caller never relies on rank balance; when
 * non-NULL it must be a valid array covering both roots. If rank is NULL,
 * always attach rb under ra (still correct, worse worst-case depth).
 */
void
gj_uf_union(int *p, int *rank, int a, int b)
{
	int ra;
	int rb;

	if (p == NULL) {
		return;
	}

	ra = gj_uf_find(p, a);
	rb = gj_uf_find(p, b);
	if (ra == rb) {
		return;
	}

	if (rank == NULL) {
		p[rb] = ra;
		return;
	}

	if (rank[ra] < rank[rb]) {
		p[ra] = rb;
	} else if (rank[ra] > rank[rb]) {
		p[rb] = ra;
	} else {
		p[rb] = ra;
		rank[ra]++;
	}
}

void __gj_uf_init(int *p, size_t n)
    __attribute__((alias("gj_uf_init")));
int __gj_uf_find(int *p, int x)
    __attribute__((alias("gj_uf_find")));
void __gj_uf_union(int *p, int *rank, int a, int b)
    __attribute__((alias("gj_uf_union")));
