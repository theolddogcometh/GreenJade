/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3157: round a size up to an alignment (u).
 *
 * Surface (unique symbols):
 *   size_t gj_size_align_up_u(size_t n, size_t align);
 *     - Return the least size >= n that is a multiple of align.
 *       align == 0 or 1 → return n. align must be a power of two;
 *       non-pow2 → return n unchanged. On arithmetic overflow of the
 *       round-up, returns 0.
 *   size_t __gj_size_align_up_u  (alias)
 *   __libcgj_batch3157_marker = "libcgj-batch3157"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_u64_align_up_u (batch2718) / gj_align_up (batch526) — unique
 * gj_size_align_up_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3157_marker[] = "libcgj-batch3157";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (n + align - 1) & ~(align - 1). align <= 1 or non-pow2 → n.
 * Intermediate overflow → 0.
 */
static size_t
b3157_size_align_up(size_t n, size_t align)
{
	size_t uMask;
	size_t uAdj;

	if (align <= 1u || (align & (align - 1u)) != 0u) {
		return n;
	}

	uMask = align - 1u;
	if (n > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (n + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_align_up_u - round size n up to power-of-two alignment.
 *
 * n:     value to align
 * align: power-of-two alignment (0/1/non-pow2 → return n)
 *
 * Overflow of the intermediate sum → 0.
 * No parent wires.
 */
size_t
gj_size_align_up_u(size_t n, size_t align)
{
	(void)NULL;
	return b3157_size_align_up(n, align);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_size_align_up_u(size_t n, size_t align)
    __attribute__((alias("gj_size_align_up_u")));
