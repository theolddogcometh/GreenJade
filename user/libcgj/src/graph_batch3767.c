/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3767: MST edge admissibility check (_u).
 *
 * Surface (unique symbols):
 *   int gj_mst_edge_ok_u(size_t *parent, size_t u, size_t v);
 *     - Kruskal / generic MST filter: edge {u, v} is admissible iff u and
 *       v lie in different components of the size_t parent forest. Uses
 *       iterative path-compressing find (self-contained; no parent wires
 *       to batch3765). Returns 1 if the edge would not form a cycle
 *       (different roots), 0 if same component or parent is NULL.
 *       Does not mutate the forest beyond path compression on finds.
 *   int __gj_mst_edge_ok_u  (alias)
 *   __libcgj_batch3767_marker = "libcgj-batch3767"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Unique mst_edge_ok_u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3767_marker[] = "libcgj-batch3767";

/* ---- freestanding helpers ---------------------------------------------- */

/* Iterative find + full path compression (self-contained). */
static size_t
b3767_find(size_t *pParent, size_t uX)
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
 * gj_mst_edge_ok_u — true if edge {u,v} joins distinct components.
 *
 * parent: forest parent array (path-compressed on find)
 * u / v:  edge endpoints
 *
 * Returns 1 if OK to add, 0 if cycle / NULL parent. No parent wires.
 */
int
gj_mst_edge_ok_u(size_t *parent, size_t u, size_t v)
{
	size_t uRu;
	size_t uRv;

	(void)NULL;

	if (parent == NULL) {
		return 0;
	}

	uRu = b3767_find(parent, u);
	uRv = b3767_find(parent, v);
	return (uRu != uRv) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mst_edge_ok_u(size_t *parent, size_t u, size_t v)
    __attribute__((alias("gj_mst_edge_ok_u")));
