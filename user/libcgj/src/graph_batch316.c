/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch316: binary lifting parent table from a parent
 * array (k-th ancestor jumps).
 *
 * Surface (unique symbols):
 *   int gj_blift_build(size_t n, const int *parent, int *up, size_t logn);
 *     — Build jump table up[0..logn*n). Layout: up[k*n + v] is the 2^k-th
 *       parent of node v (or -1 if none). parent[v] is the immediate parent;
 *       root has parent[root] == -1 or parent[root] == root (self). Nodes
 *       are dense indices in [0, n). Caps: n <= 128, logn <= 8. Returns 0
 *       on success, -1 on bad arguments.
 *   int gj_blift_kth(const int *up, size_t n, size_t logn, int v,
 *                    unsigned k);
 *     — Return the k-th ancestor of v using a table from gj_blift_build.
 *       k == 0 returns v. Returns -1 if the ancestor does not exist
 *       (walk past root) or arguments are invalid.
 *   __gj_blift_build / __gj_blift_kth  (aliases)
 *   __libcgj_batch316_marker = "libcgj-batch316"
 *
 * Classic doubling: up[0][v] = parent(v); up[k][v] = up[k-1][up[k-1][v]].
 * Query walks bits of k, jumping 2^i when bit i is set.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from batch222 (union-find parents).
 */

#include <stddef.h>

const char __libcgj_batch316_marker[] = "libcgj-batch316";

/* Freestanding caps: n nodes, logn jump levels (bits 0..logn-1). */
#define B316_MAX_N    128u
#define B316_MAX_LOGN 8u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b316_norm_parent — map parent[v] to a level-0 jump entry.
 *
 * Immediate parent in [0, n) that is not v itself → that parent.
 * Root sentinels (-1, self, or out-of-range) → -1 (no parent).
 */
static int
b316_norm_parent(const int *pParent, size_t cN, int nV)
{
	int nP;

	nP = pParent[nV];
	if (nP < 0 || (size_t)nP >= cN || nP == nV) {
		return -1;
	}
	return nP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_blift_build — fill binary-lifting table up from parent[].
 *
 * n:      node count (0..B316_MAX_N). Zero is success and writes nothing.
 * parent: parent[v] for v in [0, n); root uses -1 or self.
 * up:     caller buffer of at least logn * n ints. Row-major:
 *         up[k * n + v] = 2^k-th parent of v, or -1.
 * logn:   number of doubling levels (1..B316_MAX_LOGN when n > 0).
 *
 * Returns 0 on success, -1 if:
 *   n > 128, logn > 8, n > 0 with logn == 0, or parent/up NULL when n > 0.
 */
int
gj_blift_build(size_t n, const int *parent, int *up, size_t logn)
{
	size_t iV;
	size_t iK;

	if (n > (size_t)B316_MAX_N || logn > (size_t)B316_MAX_LOGN) {
		return -1;
	}
	if (n == 0u) {
		return 0;
	}
	if (logn == 0u || parent == NULL || up == NULL) {
		return -1;
	}

	/* Level 0: immediate parent (normalized). */
	for (iV = 0u; iV < n; iV++) {
		up[iV] = b316_norm_parent(parent, n, (int)iV);
	}

	/* up[k][v] = up[k-1][up[k-1][v]] for k = 1 .. logn-1. */
	for (iK = 1u; iK < logn; iK++) {
		size_t cBase = iK * n;
		size_t cPrev = (iK - 1u) * n;

		for (iV = 0u; iV < n; iV++) {
			int nMid = up[cPrev + iV];

			if (nMid < 0 || (size_t)nMid >= n) {
				up[cBase + iV] = -1;
			} else {
				up[cBase + iV] = up[cPrev + (size_t)nMid];
			}
		}
	}
	return 0;
}

/*
 * gj_blift_kth — k-th ancestor of v via binary lifting table.
 *
 * up/n/logn: table from a matching gj_blift_build.
 * v:         start node in [0, n).
 * k:         ancestor distance (0 → v itself).
 *
 * For each set bit i of k (i < logn), replace v with up[i*n + v].
 * If a jump would leave the tree (entry -1), returns -1. If k needs a
 * bit >= logn (table too shallow), returns -1.
 *
 * Returns ancestor index, or -1 on invalid args / no such ancestor.
 */
int
gj_blift_kth(const int *up, size_t n, size_t logn, int v, unsigned k)
{
	size_t iBit;
	int nCur;

	if (n > (size_t)B316_MAX_N || logn > (size_t)B316_MAX_LOGN) {
		return -1;
	}
	if (n == 0u) {
		return -1;
	}
	if (up == NULL || logn == 0u) {
		return -1;
	}
	if (v < 0 || (size_t)v >= n) {
		return -1;
	}

	nCur = v;
	for (iBit = 0u; iBit < logn && k != 0u; iBit++) {
		if ((k & 1u) != 0u) {
			if (nCur < 0 || (size_t)nCur >= n) {
				return -1;
			}
			nCur = up[iBit * n + (size_t)nCur];
			if (nCur < 0) {
				return -1;
			}
		}
		k >>= 1;
	}

	/* Bits beyond logn were requested but the table has no rows. */
	if (k != 0u) {
		return -1;
	}
	return nCur;
}

int __gj_blift_build(size_t n, const int *parent, int *up, size_t logn)
    __attribute__((alias("gj_blift_build")));
int __gj_blift_kth(const int *up, size_t n, size_t logn, int v, unsigned k)
    __attribute__((alias("gj_blift_kth")));
