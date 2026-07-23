/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3765: Kruskal union-find find primitive (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_kruskal_find_u(size_t *parent, size_t x);
 *     - Find root of x in a size_t parent array with iterative full path
 *       compression: walk to the root, then rewrite every node on the
 *       path to point at the root. parent[i] == i is a root. Returns the
 *       root, or x if parent is NULL. Caller must ensure x is in-range
 *       for parent[] and the structure is a valid forest (no cycles
 *       except self-roots).
 *   size_t __gj_kruskal_find_u  (alias)
 *   __libcgj_batch3765_marker = "libcgj-batch3765"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from gj_uf_find
 * (batch222 int parents) — unique size_t Kruskal _u surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3765_marker[] = "libcgj-batch3765";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Iterative find with full path compression (two-pass).
 * Avoids deep recursion; safe for freestanding stack limits.
 */
static size_t
b3765_find(size_t *pParent, size_t uX)
{
	size_t uRoot;
	size_t uCur;

	uRoot = uX;
	while (pParent[uRoot] != uRoot) {
		uRoot = pParent[uRoot];
	}

	/* Compress path. */
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
 * gj_kruskal_find_u — root of x with path compression.
 *
 * parent: forest parent array (parent[i] == i at roots)
 * x:      element index
 *
 * Returns root, or x if parent is NULL. No parent wires.
 */
size_t
gj_kruskal_find_u(size_t *parent, size_t x)
{
	(void)NULL;

	if (parent == NULL) {
		return x;
	}
	return b3765_find(parent, x);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_kruskal_find_u(size_t *parent, size_t x)
    __attribute__((alias("gj_kruskal_find_u")));
