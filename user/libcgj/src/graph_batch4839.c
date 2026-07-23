/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4839: pad a size up to a cache-line multiple.
 *
 * Surface (unique symbols):
 *   size_t gj_pad_to_line_u(size_t size);
 *     - Round size up to the next multiple of 64. size already aligned
 *       → size. Overflow of (size + 63) → 0.
 *   size_t __gj_pad_to_line_u  (alias)
 *   __libcgj_batch4839_marker = "libcgj-batch4839"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Distinct from
 * gj_cacheline_align_u (batch3983) — unique gj_pad_to_line_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4839_marker[] = "libcgj-batch4839";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B4839_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4839_pad_to_line(size_t uSize)
{
	size_t uMask = (size_t)B4839_CACHELINE - 1u;
	size_t uAdj;

	if (uSize > (SIZE_MAX - uMask)) {
		return 0u;
	}
	uAdj = (uSize + uMask) & ~uMask;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pad_to_line_u - pad size up to a multiple of the cache line.
 *
 * size: byte count to pad.
 * Returns (size + 63) & ~63, or 0 on overflow. Already aligned values
 * return unchanged. Distinct from gj_cacheline_align_u (batch3983).
 * No parent wires.
 */
size_t
gj_pad_to_line_u(size_t uSize)
{
	(void)NULL;
	return b4839_pad_to_line(uSize);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_pad_to_line_u(size_t uSize)
    __attribute__((alias("gj_pad_to_line_u")));
