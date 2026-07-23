/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4834: byte offset within a cache line.
 *
 * Surface (unique symbols):
 *   uintptr_t gj_cacheline_offset_u(uintptr_t addr);
 *     - Return addr & 63, the offset of addr inside its cache line.
 *   uintptr_t __gj_cacheline_offset_u  (alias)
 *   __libcgj_batch4834_marker = "libcgj-batch4834"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Unique
 * gj_cacheline_offset_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4834_marker[] = "libcgj-batch4834";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B4834_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uintptr_t
b4834_cacheline_offset(uintptr_t uAddr)
{
	return uAddr & ((uintptr_t)B4834_CACHELINE - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_offset_u - byte offset of addr within its cache line.
 *
 * addr: absolute or relative address as an integer.
 * Returns a value in [0, 63]. No parent wires.
 */
uintptr_t
gj_cacheline_offset_u(uintptr_t uAddr)
{
	(void)NULL;
	return b4834_cacheline_offset(uAddr);
}

/* ---- underscored alias ------------------------------------------------- */

uintptr_t __gj_cacheline_offset_u(uintptr_t uAddr)
    __attribute__((alias("gj_cacheline_offset_u")));
