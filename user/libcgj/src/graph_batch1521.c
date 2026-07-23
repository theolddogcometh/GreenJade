/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1521: arena size alignment round-up.
 *
 * Surface (unique symbols):
 *   size_t gj_arena_align_up_size(size_t size, size_t align);
 *     — Round size up to a multiple of align for arena / bump payload
 *       sizing. align == 0 or 1 → return size. Power-of-two align uses
 *       mask form; other aligns use general remainder form.
 *       Overflow of the round-up → 0.
 *   size_t __gj_arena_align_up_size  (alias)
 *   __libcgj_batch1521_marker = "libcgj-batch1521"
 *
 * Distinct from gj_align_up / gj_align_up_u64 / gj_slab_obj_size.
 * Unique gj_arena_align_up_size surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1521_marker[] = "libcgj-batch1521";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_align_up_size — size rounded up to align (arena payload stride).
 *
 * cbSize: raw byte count to align
 * uAlign: alignment (0 or 1 → identity; power-of-two preferred)
 *
 * Overflow (rounded value wraps below size) → 0.
 */
size_t
gj_arena_align_up_size(size_t cbSize, size_t uAlign)
{
	size_t uMask;
	size_t uAdj;
	size_t uRem;

	if (uAlign <= 1u) {
		return cbSize;
	}

	/* Power-of-two fast path. */
	if ((uAlign & (uAlign - 1u)) == 0u) {
		uMask = uAlign - 1u;
		if (cbSize > (SIZE_MAX - uMask)) {
			return 0u;
		}
		uAdj = (cbSize + uMask) & ~uMask;
		return uAdj;
	}

	/* General multiple: size + (align - size % align) when unaligned. */
	uRem = cbSize % uAlign;
	if (uRem == 0u) {
		return cbSize;
	}
	uAdj = cbSize + (uAlign - uRem);
	if (uAdj < cbSize) {
		return 0u;
	}
	return uAdj;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_align_up_size(size_t cbSize, size_t uAlign)
    __attribute__((alias("gj_arena_align_up_size")));
