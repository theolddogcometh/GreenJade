/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1296: array-BST parent index hint.
 *
 * Surface (unique symbols — distinct from gj_heap_parent_idx (batch1291)):
 *   size_t gj_bst_parent_hint(size_t i);
 *     — Parent index for a complete binary tree / array-laid BST at
 *       0-based index i: (i - 1) / 2. Root (i == 0) returns 0.
 *       "Hint" — pure arithmetic; does not validate tree shape.
 *   size_t __gj_bst_parent_hint  (alias)
 *   __libcgj_batch1296_marker = "libcgj-batch1296"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1296_marker[] = "libcgj-batch1296";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bst_parent_hint — parent index of array-BST node i (root → 0).
 */
size_t
gj_bst_parent_hint(size_t iIdx)
{
	if (iIdx == 0u) {
		return 0u;
	}
	return (iIdx - 1u) / 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bst_parent_hint(size_t iIdx)
    __attribute__((alias("gj_bst_parent_hint")));
