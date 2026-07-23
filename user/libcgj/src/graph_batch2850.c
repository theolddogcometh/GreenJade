/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2850: milestone 2850 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2850(void);
 *     - Returns the compile-time graph batch number for this TU (2850).
 *   uint32_t gj_graph_milestone_2850(void);
 *     - Returns the current graph milestone revision (2850).
 *   uint32_t __gj_batch_id_2850  (alias)
 *   uint32_t __gj_graph_milestone_2850  (alias)
 *   __libcgj_batch2850_marker = "libcgj-batch2850"
 *
 * Milestone for the endian/ring exclusive wave (batches 2841-2849:
 * le32_load_u, be16_store_u, be32_store_u, le16_store_u, le32_store_u,
 * ring_u8_next_u, ring_u8_prev_u, ring_u8_full_p_u, ring_u8_empty_p_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2850_marker[] = "libcgj-batch2850";

/* Milestone batch identity for the endian/ring exclusive wave. */
#define B2850_BATCH_ID  2850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2850_id(void)
{
	return B2850_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2850 - report this TU's graph batch number.
 *
 * Always returns 2850.
 */
uint32_t
gj_batch_id_2850(void)
{
	(void)NULL;
	return b2850_id();
}

/*
 * gj_graph_milestone_2850 - report this TU's graph milestone revision.
 *
 * Always returns 2850 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2850(void)
{
	return b2850_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2850(void)
    __attribute__((alias("gj_batch_id_2850")));

uint32_t __gj_graph_milestone_2850(void)
    __attribute__((alias("gj_graph_milestone_2850")));
