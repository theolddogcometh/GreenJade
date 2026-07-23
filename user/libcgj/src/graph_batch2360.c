/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2360: milestone 2360 batch identity (closes
 * post-2350 exclusive wave 2351-2360).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2360(void);
 *     - Returns the compile-time graph batch number for this TU (2360).
 *   uint32_t gj_graph_milestone_2360(void);
 *     - Returns the current graph milestone revision (2360).
 *   uint32_t __gj_batch_id_2360  (alias)
 *   uint32_t __gj_graph_milestone_2360  (alias)
 *   __libcgj_batch2360_marker = "libcgj-batch2360"
 *
 * Milestone for the post-2350 exclusive wave (batches 2351-2359:
 * gj_u32_median3, gj_u64_median3, gj_i32_median3, gj_u32_min3,
 * gj_u32_max3, gj_u64_min3, gj_u64_max3, gj_u32_clamp3, gj_u64_clamp3).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2360_marker[] = "libcgj-batch2360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2360_id(void)
{
	return 2360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2360 - report this TU's graph batch number.
 *
 * Always returns 2360.
 */
uint32_t
gj_batch_id_2360(void)
{
	(void)NULL;
	return b2360_id();
}

/*
 * gj_graph_milestone_2360 - report this TU's graph milestone revision.
 *
 * Always returns 2360 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2360(void)
{
	return b2360_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2360(void)
    __attribute__((alias("gj_batch_id_2360")));

uint32_t __gj_graph_milestone_2360(void)
    __attribute__((alias("gj_graph_milestone_2360")));
