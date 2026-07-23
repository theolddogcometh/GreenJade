/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3158: round a size down to an alignment (u).
 *
 * Surface (unique symbols):
 *   size_t gj_size_align_down_u(size_t n, size_t align);
 *     - Return the greatest size <= n that is a multiple of align.
 *       align == 0 or 1 → return n. align must be a power of two;
 *       non-pow2 → return n unchanged.
 *   size_t __gj_size_align_down_u  (alias)
 *   __libcgj_batch3158_marker = "libcgj-batch3158"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_u64_align_down_u (batch2719) / gj_align_down (batch527) —
 * unique gj_size_align_down_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3158_marker[] = "libcgj-batch3158";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * n & ~(align - 1). align <= 1 or non-pow2 → n.
 */
static size_t
b3158_size_align_down(size_t n, size_t align)
{
	size_t uMask;

	if (align <= 1u || (align & (align - 1u)) != 0u) {
		return n;
	}

	uMask = align - 1u;
	return n & ~uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_align_down_u - round size n down to power-of-two alignment.
 *
 * n:     value to align
 * align: power-of-two alignment (0/1/non-pow2 → return n)
 *
 * No parent wires.
 */
size_t
gj_size_align_down_u(size_t n, size_t align)
{
	(void)NULL;
	return b3158_size_align_down(n, align);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_size_align_down_u(size_t n, size_t align)
    __attribute__((alias("gj_size_align_down_u")));
