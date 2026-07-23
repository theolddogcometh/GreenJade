/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4270: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4270(void);
 *     - Returns the compile-time graph batch number for this TU (4270).
 *   uint32_t __gj_batch_id_4270  (alias)
 *   __libcgj_batch4270_marker = "libcgj-batch4270"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): u64_align_up_u,
 * u64_align_down_u, u64_is_aligned_u, u64_page_floor_u, u64_page_ceil_u,
 * u64_pages_spanned_u, u32_align_up_u, u32_align_down_u,
 * ptr_page_offset_u, batch_id_4270.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4270_marker[] = "libcgj-batch4270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4270_id(void)
{
	return 4270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4270 - report this TU's graph batch number.
 *
 * Always returns 4270.
 */
uint32_t
gj_batch_id_4270(void)
{
	(void)NULL;
	return b4270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4270(void)
    __attribute__((alias("gj_batch_id_4270")));
