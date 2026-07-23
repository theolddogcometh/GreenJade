/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4831: cache-line size constant query (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cacheline_size_u(void);
 *     - Return the assumed CPU cache-line size in bytes (64).
 *   uint32_t __gj_cacheline_size_u  (alias)
 *   __libcgj_batch4831_marker = "libcgj-batch4831"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Distinct from
 * gj_cacheline_size (batch987) — unique gj_cacheline_size_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4831_marker[] = "libcgj-batch4831";

/* Assumed CPU cache-line width (x86_64 / common modern default). */
#define B4831_CACHELINE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4831_cacheline_size(void)
{
	return B4831_CACHELINE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_size_u - report assumed cache-line width.
 *
 * Always returns 64 (x86_64 / common modern default). Distinct from
 * gj_cacheline_size (batch987). No parent wires.
 */
uint32_t
gj_cacheline_size_u(void)
{
	(void)NULL;
	return b4831_cacheline_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cacheline_size_u(void)
    __attribute__((alias("gj_cacheline_size_u")));
