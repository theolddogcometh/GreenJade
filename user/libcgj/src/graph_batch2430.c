/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2430: milestone 2430 batch identity (closes
 * array reduce exclusive wave 2421-2430).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2430(void);
 *     - Returns the compile-time graph batch number for this TU (2430).
 *   uint32_t gj_graph_milestone_2430(void);
 *     - Returns the current graph milestone revision (2430).
 *   uint32_t __gj_batch_id_2430  (alias)
 *   uint32_t __gj_graph_milestone_2430  (alias)
 *   __libcgj_batch2430_marker = "libcgj-batch2430"
 *
 * Milestone for the array reduce exclusive wave (batches 2421-2429:
 * gj_u32_sum_sat_n, gj_u64_sum_sat_n, gj_u32_avg_floor_n,
 * gj_u64_avg_floor_n, gj_u32_variance_hint_n, gj_u32_dot_n,
 * gj_i32_dot_n, gj_u32_l1_dist_n, gj_u32_l2sq_dist_n). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2430_marker[] = "libcgj-batch2430";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2430_id(void)
{
	return 2430u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2430 - report this TU's graph batch number.
 *
 * Always returns 2430.
 */
uint32_t
gj_batch_id_2430(void)
{
	(void)NULL;
	return b2430_id();
}

/*
 * gj_graph_milestone_2430 - report this TU's graph milestone revision.
 *
 * Always returns 2430 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2430(void)
{
	return b2430_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2430(void)
    __attribute__((alias("gj_batch_id_2430")));

uint32_t __gj_graph_milestone_2430(void)
    __attribute__((alias("gj_graph_milestone_2430")));
