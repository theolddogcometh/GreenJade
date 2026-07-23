/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2130: milestone 2130 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2130(void);
 *     - Returns the compile-time graph batch number for this TU (2130).
 *   uint32_t gj_graph_milestone_2130(void);
 *     - Returns the current graph milestone revision (2130).
 *   uint32_t __gj_batch_id_2130  (alias)
 *   uint32_t __gj_graph_milestone_2130  (alias)
 *   __libcgj_batch2130_marker = "libcgj-batch2130"
 *
 * Milestone for the post-2100 fifo_u32 exclusive wave (batches 2121-2129:
 * push, pop, peek, count, space, clear, full_p, empty_p, discard_n).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2130_marker[] = "libcgj-batch2130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2130_id(void)
{
	return 2130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2130 - report this TU's graph batch number.
 *
 * Always returns 2130.
 */
uint32_t
gj_batch_id_2130(void)
{
	(void)NULL;
	return b2130_id();
}

/*
 * gj_graph_milestone_2130 - report this TU's graph milestone revision.
 *
 * Always returns 2130 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2130(void)
{
	return b2130_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2130(void)
    __attribute__((alias("gj_batch_id_2130")));

uint32_t __gj_graph_milestone_2130(void)
    __attribute__((alias("gj_graph_milestone_2130")));
