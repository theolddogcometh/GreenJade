/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2290: milestone 2290 batch identity (closes
 * post-2280 bitops exclusive wave 2281-2290).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2290(void);
 *     - Returns the compile-time graph batch number for this TU (2290).
 *   uint32_t gj_graph_milestone_2290(void);
 *     - Returns the current graph milestone revision (2290).
 *   uint32_t __gj_batch_id_2290  (alias)
 *   uint32_t __gj_graph_milestone_2290  (alias)
 *   __libcgj_batch2290_marker = "libcgj-batch2290"
 *
 * Milestone for the post-2280 bitops exclusive wave (batches 2281-2289:
 * u8_popcount, u16_popcount, u32_popcount_u, u64_popcount_u, u32_ctz_u,
 * u64_ctz_u, u32_clz_u, u64_clz_u, u64_parity_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2290_marker[] = "libcgj-batch2290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2290_id(void)
{
	return 2290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2290 - report this TU's graph batch number.
 *
 * Always returns 2290.
 */
uint32_t
gj_batch_id_2290(void)
{
	(void)NULL;
	return b2290_id();
}

/*
 * gj_graph_milestone_2290 - report this TU's graph milestone revision.
 *
 * Always returns 2290 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2290(void)
{
	return b2290_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2290(void)
    __attribute__((alias("gj_batch_id_2290")));

uint32_t __gj_graph_milestone_2290(void)
    __attribute__((alias("gj_graph_milestone_2290")));
