/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3764: Prim MST next-vertex pick primitive (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_prim_pick_u(const uint64_t *key, const uint8_t *in_tree,
 *                         size_t n);
 *     - Among vertices v in [0, n) with in_tree[v] == 0, pick the one with
 *       minimal key[v]. UINT64_MAX keys are infinite (never chosen unless
 *       no finite candidate exists — then still skipped). Ties break to the
 *       lowest index. Returns the chosen index, or n if none is eligible
 *       (all in tree, all infinite, n == 0, or NULL inputs).
 *   size_t __gj_prim_pick_u  (alias)
 *   __libcgj_batch3764_marker = "libcgj-batch3764"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Unique Prim pick _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3764_marker[] = "libcgj-batch3764";

/* Infinite key sentinel (not a selectable Prim candidate). */
#define B3764_INF ((uint64_t)UINT64_MAX)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Scan [0, n) for unmarked finite min-key vertex.
 * Returns best index, or n if none.
 */
static size_t
b3764_pick(const uint64_t *pKey, const uint8_t *pIn, size_t uN)
{
	size_t uBest;
	uint64_t uBestKey;
	size_t uI;

	uBest = uN;
	uBestKey = B3764_INF;

	for (uI = 0u; uI < uN; uI++) {
		if (pIn[uI] != 0u) {
			continue;
		}
		if (pKey[uI] >= B3764_INF) {
			continue;
		}
		if (pKey[uI] < uBestKey) {
			uBestKey = pKey[uI];
			uBest = uI;
		}
	}
	return uBest;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prim_pick_u — next Prim vertex (min key outside the tree).
 *
 * key:     key[v] = lightest edge into v from the tree (UINT64_MAX = none)
 * in_tree: nonzero iff v already in the MST
 * n:       vertex count
 *
 * Returns index in [0, n), or n if no candidate. No parent wires.
 */
size_t
gj_prim_pick_u(const uint64_t *key, const uint8_t *in_tree, size_t n)
{
	(void)NULL;

	if (key == NULL || in_tree == NULL || n == 0u) {
		return n;
	}
	return b3764_pick(key, in_tree, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_prim_pick_u(const uint64_t *key, const uint8_t *in_tree, size_t n)
    __attribute__((alias("gj_prim_pick_u")));
