/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1696: binary-heap right-child index helper.
 *
 * Surface (unique symbols — distinct from gj_heap_right_idx (batch1293)):
 *   size_t gj_heap_right_child_idx(size_t i);
 *     — Right child index of node i in a 0-based binary heap: 2*i + 2.
 *       Does not bounds-check against a heap size (caller must).
 *   size_t __gj_heap_right_child_idx  (alias)
 *   __libcgj_batch1696_marker = "libcgj-batch1696"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1696_marker[] = "libcgj-batch1696";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1696_right(size_t iIdx)
{
	return iIdx * 2u + 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_right_child_idx — right child of heap index i (2*i + 2).
 *
 * Overflow: modular size_t arithmetic if 2*i + 2 wraps.
 */
size_t
gj_heap_right_child_idx(size_t iIdx)
{
	return b1696_right(iIdx);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_heap_right_child_idx(size_t iIdx)
    __attribute__((alias("gj_heap_right_child_idx")));
