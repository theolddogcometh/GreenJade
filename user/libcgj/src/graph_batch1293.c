/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1293: binary-heap right-child index helper.
 *
 * Surface (unique symbols):
 *   size_t gj_heap_right_idx(size_t i);
 *     — Right child index of node i in a 0-based binary heap: 2*i + 2.
 *       Does not bounds-check against a heap size (caller must).
 *   size_t __gj_heap_right_idx  (alias)
 *   __libcgj_batch1293_marker = "libcgj-batch1293"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1293_marker[] = "libcgj-batch1293";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_right_idx — right child of heap index i (2*i + 2).
 */
size_t
gj_heap_right_idx(size_t iIdx)
{
	return iIdx * 2u + 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_heap_right_idx(size_t iIdx)
    __attribute__((alias("gj_heap_right_idx")));
