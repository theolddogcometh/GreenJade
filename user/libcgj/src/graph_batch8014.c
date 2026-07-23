/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8014: malloc size align-down stub.
 *
 * Surface (unique symbols):
 *   size_t gj_malloc_align_down_8014(size_t n, size_t align);
 *     - Round n down to a multiple of align (align must be non-zero
 *       power of two). Invalid align → 0.
 *   size_t __gj_malloc_align_down_8014  (alias)
 *   __libcgj_batch8014_marker = "libcgj-batch8014"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_malloc_align_down_8014 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8014_marker[] = "libcgj-batch8014";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8014_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static size_t
b8014_align_down(size_t uN, size_t uAlign)
{
	size_t uMask;

	if (b8014_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uMask = uAlign - 1u;
	return uN & ~uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_down_8014 - soft round-down of size to alignment.
 *
 * n:     size in bytes
 * align: power-of-two alignment (non-zero)
 *
 * Returns floored size, or 0 on invalid align.
 * Soft pure-data math; does not allocate. No parent wires.
 */
size_t
gj_malloc_align_down_8014(size_t n, size_t align)
{
	(void)NULL;
	return b8014_align_down(n, align);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_malloc_align_down_8014(size_t n, size_t align)
    __attribute__((alias("gj_malloc_align_down_8014")));
