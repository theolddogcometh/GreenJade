/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch974: slab object stride from need + align.
 *
 * Surface (unique symbols):
 *   size_t gj_slab_obj_size(size_t need, size_t align);
 *     — Round need up to a multiple of align for a slab object size /
 *       slot stride. align == 0 or 1 → return need. Power-of-two align
 *       uses mask form; other aligns use general remainder form.
 *       Overflow of the round-up → 0.
 *   size_t __gj_slab_obj_size  (alias)
 *   __libcgj_batch974_marker = "libcgj-batch974"
 *
 * Distinct from gj_slab_init / gj_slab_alloc / gj_slab_free (batch325
 * index freelist). Unique gj_slab_obj_size surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch974_marker[] = "libcgj-batch974";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_obj_size — object/slot size = need rounded up to align.
 *
 * need:  payload (or payload+header) byte count
 * align: alignment / stride (0 or 1 → identity)
 *
 * Overflow (rounded value wraps below need) → 0.
 */
size_t
gj_slab_obj_size(size_t need, size_t align)
{
	size_t uMask;
	size_t uAdj;
	size_t uRem;

	if (align <= 1u) {
		return need;
	}

	/* Power-of-two fast path. */
	if ((align & (align - 1u)) == 0u) {
		uMask = align - 1u;
		/* Overflow of (need + mask)? */
		if (need > (SIZE_MAX - uMask)) {
			return 0u;
		}
		uAdj = (need + uMask) & ~uMask;
		return uAdj;
	}

	/* General multiple: need + (align - need % align) when unaligned. */
	uRem = need % align;
	if (uRem == 0u) {
		return need;
	}
	uAdj = need + (align - uRem);
	if (uAdj < need) {
		return 0u;
	}
	return uAdj;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_obj_size(size_t need, size_t align)
    __attribute__((alias("gj_slab_obj_size")));
