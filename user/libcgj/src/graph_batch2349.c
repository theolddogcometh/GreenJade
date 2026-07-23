/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2349: freestanding binary-tree minimum height
 * for n nodes (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_tree_min_height(size_t n);
 *     - Minimum height of a binary tree with n nodes (perfect / complete
 *       shape). Height of a single node is 0, so result is floor(log2(n))
 *       for n >= 1. Empty (n == 0) returns 0.
 *   size_t __gj_tree_min_height  (alias)
 *   __libcgj_batch2349_marker = "libcgj-batch2349"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_tree_height_bound (batch2347) / gj_tree_max_nodes (batch2348) /
 * gj_ilog2_u64 (batch372) - unique gj_tree_min_height surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2349_marker[] = "libcgj-batch2349";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(log2(n)) for n >= 1; 0 when n == 0. */
static size_t
b2349_min_height(size_t cNodes)
{
	size_t uH;
	size_t uV;

	if (cNodes == 0u) {
		return 0u;
	}
	uH = 0u;
	uV = cNodes;
	while (uV > 1u) {
		uV = uV >> 1;
		uH = uH + 1u;
	}
	return uH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tree_min_height - minimum binary-tree height for n nodes.
 *
 * n: node count
 *
 * Returns floor(log2(n)) when n >= 1 (complete-tree height with root
 * height 0), else 0. Does not call libc.
 */
size_t
gj_tree_min_height(size_t cNodes)
{
	(void)NULL;
	return b2349_min_height(cNodes);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tree_min_height(size_t cNodes)
    __attribute__((alias("gj_tree_min_height")));
