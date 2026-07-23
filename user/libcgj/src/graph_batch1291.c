/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1291: binary-heap parent index helper.
 *
 * Surface (unique symbols):
 *   size_t gj_heap_parent_idx(size_t i);
 *     — Parent index of child i in a 0-based binary heap: (i - 1) / 2.
 *       Root (i == 0) returns 0 (parent of root is itself by convention).
 *   size_t __gj_heap_parent_idx  (alias)
 *   __libcgj_batch1291_marker = "libcgj-batch1291"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1291_marker[] = "libcgj-batch1291";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_parent_idx — parent of heap index i (root → 0).
 */
size_t
gj_heap_parent_idx(size_t iIdx)
{
	if (iIdx == 0u) {
		return 0u;
	}
	return (iIdx - 1u) / 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_heap_parent_idx(size_t iIdx)
    __attribute__((alias("gj_heap_parent_idx")));
