/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2350: milestone 2350 batch identity (closes
 * post-2340 tree exclusive wave 2341-2350).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2350(void);
 *     - Returns the compile-time graph batch number for this TU (2350).
 *   uint32_t gj_graph_milestone_2350(void);
 *     - Returns the current graph milestone revision (2350).
 *   uint32_t __gj_batch_id_2350  (alias)
 *   uint32_t __gj_graph_milestone_2350  (alias)
 *   __libcgj_batch2350_marker = "libcgj-batch2350"
 *
 * Milestone for the post-2340 tree exclusive wave (batches 2341-2349:
 * rb_is_red_p, rb_is_black_p, rb_set_red, rb_set_black,
 * bst_child_count, bst_is_leaf_p, tree_height_bound, tree_max_nodes,
 * tree_min_height). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2350_marker[] = "libcgj-batch2350";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2350_id(void)
{
	return 2350u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2350 - report this TU's graph batch number.
 *
 * Always returns 2350.
 */
uint32_t
gj_batch_id_2350(void)
{
	(void)NULL;
	return b2350_id();
}

/*
 * gj_graph_milestone_2350 - report this TU's graph milestone revision.
 *
 * Always returns 2350 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2350(void)
{
	return b2350_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2350(void)
    __attribute__((alias("gj_batch_id_2350")));

uint32_t __gj_graph_milestone_2350(void)
    __attribute__((alias("gj_graph_milestone_2350")));
