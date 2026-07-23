/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2348: freestanding binary-tree max node count
 * for a given height (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_tree_max_nodes(size_t h);
 *     - Maximum nodes in a binary tree of height h. Height of a single
 *       node is 0, so max nodes is (2^(h+1) - 1). Saturates at
 *       SIZE_MAX when the power would overflow size_t.
 *   size_t __gj_tree_max_nodes  (alias)
 *   __libcgj_batch2348_marker = "libcgj-batch2348"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_tree_height_bound (batch2347) - unique gj_tree_max_nodes surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2348_marker[] = "libcgj-batch2348";

/* Bits in size_t (shift ceiling for 1u << k). */
#define B2348_SIZE_BITS ((size_t)(sizeof(size_t) * 8u))

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Perfect binary tree of height h has 2^(h+1) - 1 nodes.
 * When h+1 >= bit-width of size_t, result saturates to SIZE_MAX.
 */
static size_t
b2348_max_nodes(size_t uH)
{
	/* Need room for shift amount (h+1) without wrapping size_t. */
	if (uH >= (B2348_SIZE_BITS - 1u)) {
		return (size_t)-1;
	}
	return ((size_t)1 << (uH + 1u)) - (size_t)1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tree_max_nodes - max nodes for a binary tree of height h.
 *
 * h: tree height (single node => 0)
 *
 * Returns 2^(h+1) - 1, or SIZE_MAX on overflow. Does not call libc.
 */
size_t
gj_tree_max_nodes(size_t uH)
{
	(void)NULL;
	return b2348_max_nodes(uH);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tree_max_nodes(size_t uH)
    __attribute__((alias("gj_tree_max_nodes")));
