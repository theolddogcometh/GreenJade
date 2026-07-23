/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3983: cache-line align size_t up (64 bytes).
 *
 * Surface (unique symbols):
 *   size_t gj_cacheline_align_u(size_t x);
 *     - Round x up to the next multiple of 64. x already aligned → x.
 *       Overflow of (x + 63) → 0.
 *   size_t __gj_cacheline_align_u  (alias)
 *   __libcgj_batch3983_marker = "libcgj-batch3983"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_cacheline_size (batch987) / gj_page_align_u (batch3982) — unique
 * gj_cacheline_align_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3983_marker[] = "libcgj-batch3983";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B3983_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3983_cacheline_align(size_t uX)
{
	size_t uMask = (size_t)B3983_CACHELINE - 1u;
	size_t uAdj;

	if (uX > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uX + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_align_u - (x + 63) & ~63 with overflow → 0.
 *
 * x: byte count or address-as-size to round up to a cache-line boundary.
 * Already cacheline-aligned values return unchanged. No parent wires.
 */
size_t
gj_cacheline_align_u(size_t uX)
{
	(void)NULL;
	return b3983_cacheline_align(uX);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_cacheline_align_u(size_t uX)
    __attribute__((alias("gj_cacheline_align_u")));
