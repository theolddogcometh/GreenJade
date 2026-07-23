/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8013: malloc size align-up stub.
 *
 * Surface (unique symbols):
 *   size_t gj_malloc_align_up_8013(size_t n, size_t align);
 *     - Round n up to a multiple of align (align must be non-zero power
 *       of two). Invalid align → 0. Overflow-safe: if rounding would
 *       wrap size_t, returns 0.
 *   size_t __gj_malloc_align_up_8013  (alias)
 *   __libcgj_batch8013_marker = "libcgj-batch8013"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_malloc_align_up_8013 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8013_marker[] = "libcgj-batch8013";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8013_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static size_t
b8013_align_up(size_t uN, size_t uAlign)
{
	size_t uMask;
	size_t uSum;

	if (b8013_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uMask = uAlign - 1u;
	/* Overflow if n + mask wraps. */
	uSum = uN + uMask;
	if (uSum < uN) {
		return 0u;
	}
	return uSum & ~uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_up_8013 - soft round-up of size to alignment.
 *
 * n:     size in bytes
 * align: power-of-two alignment (non-zero)
 *
 * Returns rounded size, or 0 on invalid align / overflow.
 * Soft pure-data math; does not allocate. No parent wires.
 */
size_t
gj_malloc_align_up_8013(size_t n, size_t align)
{
	(void)NULL;
	return b8013_align_up(n, align);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_malloc_align_up_8013(size_t n, size_t align)
    __attribute__((alias("gj_malloc_align_up_8013")));
