/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2560: milestone 2560 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2560(void);
 *     - Returns the compile-time graph batch number for this TU (2560).
 *   uint32_t gj_graph_milestone_2560(void);
 *     - Returns the current graph milestone revision (2560).
 *   uint32_t __gj_batch_id_2560  (alias)
 *   uint32_t __gj_graph_milestone_2560  (alias)
 *   __libcgj_batch2560_marker = "libcgj-batch2560"
 *
 * Milestone for the exclusive sort helpers wave (batches 2551-2559:
 * u64/i64 insertion_sort_asc, u32 shell/cocktail sort_asc, is_heap_max/min_p,
 * heapify_max/min, sort_check_asc). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2560_marker[] = "libcgj-batch2560";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2560_id(void)
{
	return 2560u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2560 - report this TU's graph batch number.
 *
 * Always returns 2560.
 */
uint32_t
gj_batch_id_2560(void)
{
	(void)NULL;
	return b2560_id();
}

/*
 * gj_graph_milestone_2560 - report this TU's graph milestone revision.
 *
 * Always returns 2560 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2560(void)
{
	return b2560_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2560(void)
    __attribute__((alias("gj_batch_id_2560")));

uint32_t __gj_graph_milestone_2560(void)
    __attribute__((alias("gj_graph_milestone_2560")));
