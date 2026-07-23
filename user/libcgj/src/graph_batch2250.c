/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2250: milestone 2250 batch identity (closes
 * post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2250(void);
 *     - Returns the compile-time graph batch number for this TU (2250).
 *   uint32_t gj_graph_milestone_2250(void);
 *     - Returns the current graph milestone revision (2250).
 *   uint32_t __gj_batch_id_2250  (alias)
 *   uint32_t __gj_graph_milestone_2250  (alias)
 *   __libcgj_batch2250_marker = "libcgj-batch2250"
 *
 * Milestone for the exclusive math helpers wave (batches 2241-2249:
 * u32/u64 log2_floor, log2_ceil, next_pow2, prev_pow2, is_pow2_p).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2250_marker[] = "libcgj-batch2250";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2250_id(void)
{
	return 2250u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2250 - report this TU's graph batch number.
 *
 * Always returns 2250.
 */
uint32_t
gj_batch_id_2250(void)
{
	(void)NULL;
	return b2250_id();
}

/*
 * gj_graph_milestone_2250 - report this TU's graph milestone revision.
 *
 * Always returns 2250 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2250(void)
{
	return b2250_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2250(void)
    __attribute__((alias("gj_batch_id_2250")));

uint32_t __gj_graph_milestone_2250(void)
    __attribute__((alias("gj_graph_milestone_2250")));
