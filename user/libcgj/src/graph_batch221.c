/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch221: min segment tree over int64_t arrays.
 *
 * Surface (unique symbols):
 *   void gj_segtree_build(int64_t *t, const int64_t *a, size_t n);
 *     — Build a 1-indexed min segment tree for a[0..n) into t.
 *       Caller provides t with at least 4*n int64_t slots (4n buffer).
 *       n==0 or NULL t/a (when n>0) → no-op.
 *   int64_t gj_segtree_query(const int64_t *t, size_t n, size_t l, size_t r);
 *     — Minimum of a[l..r] inclusive after build. Requires a prior
 *       gj_segtree_build on the same t/n. Empty / invalid ranges
 *       (l>r, n==0, NULL t, l>=n) return INT64_MAX (min identity).
 *       r is clamped to n-1 when r>=n.
 *   __gj_segtree_build / __gj_segtree_query  (aliases)
 *   __libcgj_batch221_marker = "libcgj-batch221"
 *
 * Layout (classic recursive segment tree):
 *   node 1 covers [0, n)
 *   left child of v is 2v, right is 2v+1
 *   t[v] = min of the segment stored at node v
 *   Leaves hold a[i]; internal nodes min(left, right).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch221_marker[] = "libcgj-batch221";

/* Min identity: larger than any int64_t value. */
#define B221_INF ((int64_t)0x7fffffffffffffffLL)

/* ---- helpers ----------------------------------------------------------- */

static int64_t
b221_min2(int64_t x, int64_t y)
{
	return (x < y) ? x : y;
}

/*
 * Recursively build node v covering half-open [tl, tr) from a.
 * t must have room for indices up through ~4n (1-indexed tree).
 */
static void
b221_build(int64_t *t, const int64_t *a, size_t v, size_t tl, size_t tr)
{
	size_t tm;

	if (tr - tl == 1u) {
		t[v] = a[tl];
		return;
	}
	tm = tl + ((tr - tl) >> 1);
	b221_build(t, a, v << 1, tl, tm);
	b221_build(t, a, (v << 1) | 1u, tm, tr);
	t[v] = b221_min2(t[v << 1], t[(v << 1) | 1u]);
}

/*
 * Min on intersection of node segment [tl, tr) with query [ql, qr)
 * (half-open internally; public API is inclusive [l, r]).
 */
static int64_t
b221_query(const int64_t *t, size_t v, size_t tl, size_t tr,
           size_t ql, size_t qr)
{
	size_t tm;
	int64_t left;
	int64_t right;

	if (ql >= tr || qr <= tl) {
		return B221_INF;
	}
	if (ql <= tl && tr <= qr) {
		return t[v];
	}
	tm = tl + ((tr - tl) >> 1);
	left = b221_query(t, v << 1, tl, tm, ql, qr);
	right = b221_query(t, (v << 1) | 1u, tm, tr, ql, qr);
	return b221_min2(left, right);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_segtree_build — fill t[1..] as a min segment tree over a[0..n).
 *
 * t: tree buffer, capacity >= 4*n elements (unused slots may be left unset).
 * a: source array of n int64_t values.
 * n: element count; 0 → no-op.
 */
void
gj_segtree_build(int64_t *t, const int64_t *a, size_t n)
{
	if (t == NULL || a == NULL || n == 0u) {
		return;
	}
	b221_build(t, a, 1u, 0u, n);
}

void __gj_segtree_build(int64_t *t, const int64_t *a, size_t n)
    __attribute__((alias("gj_segtree_build")));

/*
 * gj_segtree_query — range minimum on inclusive indices [l, r].
 *
 * t/n must match a prior gj_segtree_build. Returns B221_INF (INT64_MAX)
 * when the effective query range is empty or arguments are invalid.
 */
int64_t
gj_segtree_query(const int64_t *t, size_t n, size_t l, size_t r)
{
	size_t qr;

	if (t == NULL || n == 0u || l >= n || l > r) {
		return B221_INF;
	}
	/* Clamp r to last valid index; convert inclusive → half-open end. */
	qr = (r >= n) ? n : (r + 1u);
	return b221_query(t, 1u, 0u, n, l, qr);
}

int64_t __gj_segtree_query(const int64_t *t, size_t n, size_t l, size_t r)
    __attribute__((alias("gj_segtree_query")));
