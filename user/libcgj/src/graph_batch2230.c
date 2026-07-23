/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2230: milestone 2230 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2230(void);
 *     - Returns the compile-time graph batch number for this TU (2230).
 *   uint32_t gj_graph_milestone_2230(void);
 *     - Returns the current graph milestone revision (2230).
 *   uint32_t __gj_batch_id_2230  (alias)
 *   uint32_t __gj_graph_milestone_2230  (alias)
 *   __libcgj_batch2230_marker = "libcgj-batch2230"
 *
 * Milestone for the post-2220 table exclusive wave (batches 2221-2229:
 * table_u32_lookup, table_u32_insert_hint, table_u32_delete_hint,
 * table_u64_lookup, table_u64_insert_hint, slot_empty_u32_p,
 * slot_tomb_u32_p, load_factor_milli, probe_linear_next). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2230_marker[] = "libcgj-batch2230";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2230_id(void)
{
	return 2230u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2230 - report this TU's graph batch number.
 *
 * Always returns 2230.
 */
uint32_t
gj_batch_id_2230(void)
{
	(void)NULL;
	return b2230_id();
}

/*
 * gj_graph_milestone_2230 - report this TU's graph milestone revision.
 *
 * Always returns 2230 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2230(void)
{
	return b2230_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2230(void)
    __attribute__((alias("gj_batch_id_2230")));

uint32_t __gj_graph_milestone_2230(void)
    __attribute__((alias("gj_graph_milestone_2230")));
