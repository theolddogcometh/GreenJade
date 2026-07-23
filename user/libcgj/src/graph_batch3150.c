/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3150: MILESTONE 3150 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3150(void);
 *     - Returns the compile-time graph batch number for this TU (3150).
 *   uint32_t gj_graph_milestone_3150(void);
 *     - Returns the current graph milestone revision (3150).
 *   uint32_t __gj_batch_id_3150  (alias)
 *   uint32_t __gj_graph_milestone_3150  (alias)
 *   __libcgj_batch3150_marker = "libcgj-batch3150"
 *
 * MILESTONE 3150 for the exclusive continuum CREATE-ONLY wave
 * (batches 3141-3150: period_ns_to_hz_u, align_page_4k_u,
 * align_page_2m_u, page_count_4k_u, page_count_2m_u, kb_to_bytes_u,
 * mb_to_bytes_u, gb_to_bytes_u, product_wave_3150). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3150_marker[] = "libcgj-batch3150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3150_id(void)
{
	return 3150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3150 - report this TU's graph batch number.
 *
 * Always returns 3150.
 */
uint32_t
gj_batch_id_3150(void)
{
	(void)NULL;
	return b3150_id();
}

/*
 * gj_graph_milestone_3150 - report this TU's graph milestone revision.
 *
 * Always returns 3150 (MILESTONE 3150). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3150(void)
{
	return b3150_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3150(void)
    __attribute__((alias("gj_batch_id_3150")));

uint32_t __gj_graph_milestone_3150(void)
    __attribute__((alias("gj_graph_milestone_3150")));
