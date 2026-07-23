/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4840: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4840(void);
 *     - Returns the compile-time graph batch number for this TU (4840).
 *   uint32_t __gj_batch_id_4840  (alias)
 *   __libcgj_batch4840_marker = "libcgj-batch4840"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4840_marker[] = "libcgj-batch4840";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4840_id(void)
{
	return 4840u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4840 - report this TU's graph batch number.
 *
 * Always returns 4840. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4840(void)
{
	(void)NULL;
	return b4840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4840(void)
    __attribute__((alias("gj_batch_id_4840")));
