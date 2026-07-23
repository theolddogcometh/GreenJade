/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2340: milestone 2340 batch identity (closes
 * post-2330 list exclusive wave 2331-2340).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2340(void);
 *     - Returns the compile-time graph batch number for this TU (2340).
 *   uint32_t gj_graph_milestone_2340(void);
 *     - Returns the current graph milestone revision (2340).
 *   uint32_t __gj_batch_id_2340  (alias)
 *   uint32_t __gj_graph_milestone_2340  (alias)
 *   __libcgj_batch2340_marker = "libcgj-batch2340"
 *
 * Milestone for the post-2330 list exclusive wave (batches 2331-2339:
 * list_node_init, list_empty_p, list_push_front, list_push_back,
 * list_pop_front, list_pop_back, list_len_n, list_contains_p,
 * list_unlink). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2340_marker[] = "libcgj-batch2340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2340_id(void)
{
	return 2340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2340 - report this TU's graph batch number.
 *
 * Always returns 2340.
 */
uint32_t
gj_batch_id_2340(void)
{
	(void)NULL;
	return b2340_id();
}

/*
 * gj_graph_milestone_2340 - report this TU's graph milestone revision.
 *
 * Always returns 2340 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2340(void)
{
	return b2340_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2340(void)
    __attribute__((alias("gj_batch_id_2340")));

uint32_t __gj_graph_milestone_2340(void)
    __attribute__((alias("gj_graph_milestone_2340")));
