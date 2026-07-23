/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2720: milestone 2720 batch identity (closes
 * post-2710 continuum exclusive arith/align wave 2711-2720).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2720(void);
 *     - Returns the compile-time graph batch number for this TU (2720).
 *   uint32_t gj_graph_milestone_2720(void);
 *     - Returns the current graph milestone revision (2720).
 *   uint32_t __gj_batch_id_2720  (alias)
 *   uint32_t __gj_graph_milestone_2720  (alias)
 *   __libcgj_batch2720_marker = "libcgj-batch2720"
 *
 * Milestone for the exclusive continuum wave (batches 2711-2719:
 * gj_u64_sat_sub_u, gj_u32_sat_mul_u_soft, gj_u64_sat_mul_u,
 * gj_u32_div_ceil_u, gj_u64_div_ceil_u2, gj_u32_align_up_u,
 * gj_u32_align_down_u, gj_u64_align_up_u, gj_u64_align_down_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2720_marker[] = "libcgj-batch2720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2720_id(void)
{
	return 2720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2720 - report this TU's graph batch number.
 *
 * Always returns 2720.
 */
uint32_t
gj_batch_id_2720(void)
{
	(void)NULL;
	return b2720_id();
}

/*
 * gj_graph_milestone_2720 - report this TU's graph milestone revision.
 *
 * Always returns 2720 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2720(void)
{
	return b2720_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2720(void)
    __attribute__((alias("gj_batch_id_2720")));

uint32_t __gj_graph_milestone_2720(void)
    __attribute__((alias("gj_graph_milestone_2720")));
