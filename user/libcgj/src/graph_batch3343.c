/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3343: slab element index from addresses (u).
 *
 * Surface (unique symbols):
 *   size_t gj_slab_idx_u(uintptr_t base, uintptr_t ptr, size_t stride);
 *     - Return the zero-based element index of ptr within a fixed-stride
 *       slab starting at base: (ptr - base) / stride when ptr >= base,
 *       stride > 0, and (ptr - base) is an exact multiple of stride.
 *       Otherwise return (size_t)-1 (out of slab / bad stride / unaligned).
 *   size_t __gj_slab_idx_u  (alias)
 *   __libcgj_batch3343_marker = "libcgj-batch3343"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_slab_index_for_size (batch1525 size-class map) and
 * gj_slab_alloc (batch325 freelist pop) — unique gj_slab_idx_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3343_marker[] = "libcgj-batch3343";

/* Failure sentinel. */
#define B3343_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3343_idx(uintptr_t uBase, uintptr_t uPtr, size_t uStride)
{
	uintptr_t uDelta;

	if (uStride == 0u) {
		return B3343_FAIL;
	}
	if (uPtr < uBase) {
		return B3343_FAIL;
	}
	uDelta = uPtr - uBase;
	if ((uDelta % (uintptr_t)uStride) != 0u) {
		return B3343_FAIL;
	}
	return (size_t)(uDelta / (uintptr_t)uStride);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_idx_u - element index of ptr in a fixed-stride slab at base.
 *
 * base:   slab base address as integer
 * ptr:    candidate element address as integer
 * stride: element size in bytes (must be > 0)
 *
 * Returns index on exact alignment, else (size_t)-1.
 * No parent wires.
 */
size_t
gj_slab_idx_u(uintptr_t uBase, uintptr_t uPtr, size_t uStride)
{
	(void)NULL;
	return b3343_idx(uBase, uPtr, uStride);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_idx_u(uintptr_t uBase, uintptr_t uPtr, size_t uStride)
    __attribute__((alias("gj_slab_idx_u")));
