/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2240: milestone 2240 batch identity (closes
 * post-2230 sort exclusive wave 2231-2240).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2240(void);
 *     - Returns the compile-time graph batch number for this TU (2240).
 *   uint32_t gj_graph_milestone_2240(void);
 *     - Returns the current graph milestone revision (2240).
 *   uint32_t __gj_batch_id_2240  (alias)
 *   uint32_t __gj_graph_milestone_2240  (alias)
 *   __libcgj_batch2240_marker = "libcgj-batch2240"
 *
 * Milestone for the exclusive sort helpers wave (batches 2231-2239:
 * u32/u64/i32 insertion_sort_n, selection_sort_n, bubble_sort_n,
 * is_sorted_asc_n, is_sorted_desc_n, partition_lomuto, swap_idx).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2240_marker[] = "libcgj-batch2240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2240_id(void)
{
	return 2240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2240 - report this TU's graph batch number.
 *
 * Always returns 2240.
 */
uint32_t
gj_batch_id_2240(void)
{
	(void)NULL;
	return b2240_id();
}

/*
 * gj_graph_milestone_2240 - report this TU's graph milestone revision.
 *
 * Always returns 2240 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2240(void)
{
	return b2240_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2240(void)
    __attribute__((alias("gj_batch_id_2240")));

uint32_t __gj_graph_milestone_2240(void)
    __attribute__((alias("gj_graph_milestone_2240")));
