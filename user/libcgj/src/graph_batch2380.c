/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2380: milestone 2380 batch identity (closes
 * post-2370 unsigned saturating arith exclusive wave 2371-2380).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2380(void);
 *     - Returns the compile-time graph batch number for this TU (2380).
 *   uint32_t gj_graph_milestone_2380(void);
 *     - Returns the current graph milestone revision (2380).
 *   uint32_t __gj_batch_id_2380  (alias)
 *   uint32_t __gj_graph_milestone_2380  (alias)
 *   __libcgj_batch2380_marker = "libcgj-batch2380"
 *
 * Milestone for the post-2370 exclusive wave (batches 2371-2379:
 * gj_u8_sat_add, gj_u8_sat_sub, gj_u16_sat_add, gj_u16_sat_sub,
 * gj_u32_sat_add_u, gj_u32_sat_sub_u, gj_u8_sat_mul, gj_u16_sat_mul,
 * gj_u32_sat_mul_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2380_marker[] = "libcgj-batch2380";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2380_id(void)
{
	return 2380u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2380 - report this TU's graph batch number.
 *
 * Always returns 2380.
 */
uint32_t
gj_batch_id_2380(void)
{
	(void)NULL;
	return b2380_id();
}

/*
 * gj_graph_milestone_2380 - report this TU's graph milestone revision.
 *
 * Always returns 2380 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2380(void)
{
	return b2380_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2380(void)
    __attribute__((alias("gj_batch_id_2380")));

uint32_t __gj_graph_milestone_2380(void)
    __attribute__((alias("gj_graph_milestone_2380")));
