/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3766: Kruskal union-find union-by-rank (_u).
 *
 * Surface (unique symbols):
 *   int gj_kruskal_union_u(size_t *parent, size_t *rank, size_t a, size_t b);
 *     - Unite the sets containing a and b by rank. parent[] and rank[]
 *       are size_t arrays; fresh singletons have parent[i] == i and
 *       rank[i] == 0. Internal iterative find with path compression (no
 *       call into batch3765 — no parent wires). If rank is NULL, attaches
 *       root(b) under root(a) without rank balance.
 *     Returns 1 if two distinct sets were united, 0 if already same set
 *       or parent is NULL.
 *   int __gj_kruskal_union_u  (alias)
 *   __libcgj_batch3766_marker = "libcgj-batch3766"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from gj_uf_union
 * (batch222 int parents) — unique size_t Kruskal _u surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3766_marker[] = "libcgj-batch3766";

/* ---- freestanding helpers ---------------------------------------------- */

/* Iterative find + full path compression (self-contained). */
static size_t
b3766_find(size_t *pParent, size_t uX)
{
	size_t uRoot;
	size_t uCur;

	uRoot = uX;
	while (pParent[uRoot] != uRoot) {
		uRoot = pParent[uRoot];
	}
	uCur = uX;
	while (uCur != uRoot) {
		size_t uNext;

		uNext = pParent[uCur];
		pParent[uCur] = uRoot;
		uCur = uNext;
	}
	return uRoot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kruskal_union_u — link roots of a and b by rank.
 *
 * parent: forest parent array
 * rank:   rank array (may be NULL → attach rb under ra)
 * a / b:  elements to unite
 *
 * Returns 1 if united, 0 if same set or parent NULL. No parent wires.
 */
int
gj_kruskal_union_u(size_t *parent, size_t *rank, size_t a, size_t b)
{
	size_t uRa;
	size_t uRb;

	(void)NULL;

	if (parent == NULL) {
		return 0;
	}

	uRa = b3766_find(parent, a);
	uRb = b3766_find(parent, b);
	if (uRa == uRb) {
		return 0;
	}

	if (rank == NULL) {
		parent[uRb] = uRa;
		return 1;
	}

	if (rank[uRa] < rank[uRb]) {
		parent[uRa] = uRb;
	} else if (rank[uRa] > rank[uRb]) {
		parent[uRb] = uRa;
	} else {
		parent[uRb] = uRa;
		rank[uRa]++;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_kruskal_union_u(size_t *parent, size_t *rank, size_t a, size_t b)
    __attribute__((alias("gj_kruskal_union_u")));
