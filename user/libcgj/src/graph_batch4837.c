/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4837: prefetch write-locality hint code.
 *
 * Surface (unique symbols):
 *   uint32_t gj_prefetch_hint_write_u(void);
 *     - Return the integer tag for a write-locality prefetch hint (2).
 *   uint32_t __gj_prefetch_hint_write_u  (alias)
 *   __libcgj_batch4837_marker = "libcgj-batch4837"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Distinct from
 * gj_prefetch_write (batch986) / gj_prefetch_hint_read_u (batch4836) —
 * unique gj_prefetch_hint_write_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4837_marker[] = "libcgj-batch4837";

/* Soft integer tag for a write-locality prefetch hint. */
#define B4837_HINT_WRITE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4837_prefetch_hint_write(void)
{
	return B4837_HINT_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prefetch_hint_write_u - report the write-prefetch hint code.
 *
 * Always returns 2. Soft compile-time locality tag; does not emit a
 * hardware prefetch. Distinct from gj_prefetch_write (batch986).
 * No parent wires.
 */
uint32_t
gj_prefetch_hint_write_u(void)
{
	(void)NULL;
	return b4837_prefetch_hint_write();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_prefetch_hint_write_u(void)
    __attribute__((alias("gj_prefetch_hint_write_u")));
