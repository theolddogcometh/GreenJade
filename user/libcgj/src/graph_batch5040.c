/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5040: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5040(void);
 *     - Returns the compile-time graph batch number for this TU (5040).
 *   uint32_t __gj_batch_id_5040  (alias)
 *   __libcgj_batch5040_marker = "libcgj-batch5040"
 *
 * Exclusive continuum CREATE-ONLY (5031-5040: mm page helpers —
 * page_shift_u, page_size_u, page_mask_u, pfn_u, page_u, offset_u,
 * round_up_u, round_down_u, pages_for_u, batch_id_5040).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5040_marker[] = "libcgj-batch5040";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5040_id(void)
{
	return 5040u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5040 - report this TU's graph batch number.
 *
 * Always returns 5040. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5040(void)
{
	(void)NULL;
	return b5040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5040(void)
    __attribute__((alias("gj_batch_id_5040")));
