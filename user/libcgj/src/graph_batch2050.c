/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2050: milestone 2050 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2050(void);
 *     - Returns the compile-time graph batch number for this TU (2050).
 *   uint32_t gj_graph_milestone_2050(void);
 *     - Returns the current graph milestone revision (2050).
 *   uint32_t __gj_batch_id_2050  (alias)
 *   uint32_t __gj_graph_milestone_2050  (alias)
 *   __libcgj_batch2050_marker = "libcgj-batch2050"
 *
 * Milestone for the post-2000 i64 arith exclusive wave (batches 2041-2049:
 * add_sat_u, sub_sat_u, mul_sat_u, neg_sat_u, abs_sat_u, signum, cmp,
 * clamp_range, in_range_p). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2050_marker[] = "libcgj-batch2050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2050_id(void)
{
	return 2050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2050 - report this TU's graph batch number.
 *
 * Always returns 2050.
 */
uint32_t
gj_batch_id_2050(void)
{
	(void)NULL;
	return b2050_id();
}

/*
 * gj_graph_milestone_2050 - report this TU's graph milestone revision.
 *
 * Always returns 2050 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2050(void)
{
	return b2050_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2050(void)
    __attribute__((alias("gj_batch_id_2050")));

uint32_t __gj_graph_milestone_2050(void)
    __attribute__((alias("gj_graph_milestone_2050")));
