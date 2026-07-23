/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2347: freestanding binary-tree height bound
 * (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_tree_height_bound(size_t n);
 *     - Worst-case height of a binary tree with n nodes. Height of a
 *       single node is 0, so the bound is n - 1 for n >= 1. Empty
 *       (n == 0) returns 0.
 *   size_t __gj_tree_height_bound  (alias)
 *   __libcgj_batch2347_marker = "libcgj-batch2347"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_tree_diameter (batch317) - unique gj_tree_height_bound surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2347_marker[] = "libcgj-batch2347";

/* ---- freestanding helpers ---------------------------------------------- */

/* Worst-case height: chain of n nodes is n-1 edges. */
static size_t
b2347_height_bound(size_t cNodes)
{
	if (cNodes == 0u) {
		return 0u;
	}
	return cNodes - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tree_height_bound - upper bound on binary-tree height for n nodes.
 *
 * n: node count
 *
 * Returns n - 1 when n >= 1 (degenerate chain), else 0. Height of a
 * one-node tree is 0. Does not call libc.
 */
size_t
gj_tree_height_bound(size_t cNodes)
{
	(void)NULL;
	return b2347_height_bound(cNodes);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tree_height_bound(size_t cNodes)
    __attribute__((alias("gj_tree_height_bound")));
