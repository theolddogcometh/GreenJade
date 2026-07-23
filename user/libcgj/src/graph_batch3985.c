/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3985: cache-line alignment predicate (size_t).
 *
 * Surface (unique symbols):
 *   int gj_is_cacheline_aligned_u(size_t x);
 *     - Return 1 if x is a multiple of 64 (x & 63 == 0), else 0.
 *   int __gj_is_cacheline_aligned_u  (alias)
 *   __libcgj_batch3985_marker = "libcgj-batch3985"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990). Distinct from
 * gj_is_page_aligned_u (batch3984) / gj_cacheline_align_u (batch3983) /
 * gj_cacheline_size (batch987) — unique gj_is_cacheline_aligned_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3985_marker[] = "libcgj-batch3985";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B3985_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3985_is_cacheline_aligned(size_t uX)
{
	size_t uMask = (size_t)B3985_CACHELINE - 1u;

	return ((uX & uMask) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_cacheline_aligned_u - true when uX is aligned to a cache line.
 *
 * Returns 1 if (uX % 64) == 0, else 0. No parent wires.
 */
int
gj_is_cacheline_aligned_u(size_t uX)
{
	(void)NULL;
	return b3985_is_cacheline_aligned(uX);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_cacheline_aligned_u(size_t uX)
    __attribute__((alias("gj_is_cacheline_aligned_u")));
