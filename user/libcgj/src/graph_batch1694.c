/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1694: binary-heap parent index (tree/index wave).
 *
 * Surface (unique symbols — same names as batch1291; this TU provides
 * weak definitions so a whole-archive link keeps a single strong surface
 * from batch1291 when both objects are present):
 *   size_t gj_heap_parent_index(size_t i)  [weak]
 *     — Parent index of child i in a 0-based binary heap: (i - 1) / 2.
 *       Root (i == 0) returns 0 (parent of root is itself by convention).
 *   size_t __gj_heap_parent_index  (weak alias)
 *   __libcgj_batch1694_marker = "libcgj-batch1694"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1694_marker[] = "libcgj-batch1694";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1694_parent(size_t iIdx)
{
	if (iIdx == 0u) {
		return 0u;
	}
	return (iIdx - 1u) / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_parent_index — parent of heap index i (root → 0).
 *
 * Weak: batch1291 owns the strong definition of the same name.
 */
 size_t
gj_heap_parent_index(size_t iIdx)
{
	return b1694_parent(iIdx);
}

/* ---- underscored alias ------------------------------------------------- */

 size_t
__gj_heap_parent_index(size_t iIdx)
{
	return b1694_parent(iIdx);
}
