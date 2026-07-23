/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1695: binary-heap left-child index helper.
 *
 * Surface (unique symbols — distinct from gj_heap_left_idx (batch1292)):
 *   size_t gj_heap_left_child_idx(size_t i);
 *     — Left child index of node i in a 0-based binary heap: 2*i + 1.
 *       Does not bounds-check against a heap size (caller must).
 *   size_t __gj_heap_left_child_idx  (alias)
 *   __libcgj_batch1695_marker = "libcgj-batch1695"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1695_marker[] = "libcgj-batch1695";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1695_left(size_t iIdx)
{
	return iIdx * 2u + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_left_child_idx — left child of heap index i (2*i + 1).
 *
 * Overflow: if 2*i + 1 would wrap size_t, result is well-defined modular
 * arithmetic; callers with huge i should not rely on a valid child.
 */
size_t
gj_heap_left_child_idx(size_t iIdx)
{
	return b1695_left(iIdx);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_heap_left_child_idx(size_t iIdx)
    __attribute__((alias("gj_heap_left_child_idx")));
