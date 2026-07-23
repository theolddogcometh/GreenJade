/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4832: cache-line align size_t up (64 bytes).
 *
 * Surface (unique symbols):
 *   size_t gj_cacheline_align_up_u(size_t x);
 *     - Round x up to the next multiple of 64. x already aligned → x.
 *       Overflow of (x + 63) → 0.
 *   size_t __gj_cacheline_align_up_u  (alias)
 *   __libcgj_batch4832_marker = "libcgj-batch4832"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840). gj_cacheline_align_u
 * already claimed by batch3983 — this TU exposes the unique
 * gj_cacheline_align_up_u surface (same 64-byte roundup semantics) so
 * the continuum has no multi-def. Distinct from gj_cacheline_align_u
 * (batch3983) / gj_page_align_u (batch3982). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4832_marker[] = "libcgj-batch4832";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B4832_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4832_cacheline_align_up(size_t uX)
{
	size_t uMask = (size_t)B4832_CACHELINE - 1u;
	size_t uAdj;

	if (uX > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uX + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_align_up_u - (x + 63) & ~63 with overflow → 0.
 *
 * x: byte count or address-as-size to round up to a cache-line boundary.
 * Already cacheline-aligned values return unchanged.
 * Unique continuum surface; does not redefine gj_cacheline_align_u
 * (batch3983). No parent wires.
 */
size_t
gj_cacheline_align_up_u(size_t uX)
{
	(void)NULL;
	return b4832_cacheline_align_up(uX);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_cacheline_align_up_u(size_t uX)
    __attribute__((alias("gj_cacheline_align_up_u")));
