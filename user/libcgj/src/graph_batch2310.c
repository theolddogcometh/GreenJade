/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2310: milestone 2310 batch identity (closes
 * post-2300 buffer exclusive wave 2301-2310).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2310(void);
 *     - Returns the compile-time graph batch number for this TU (2310).
 *   uint32_t gj_graph_milestone_2310(void);
 *     - Returns the current graph milestone revision (2310).
 *   uint32_t __gj_batch_id_2310  (alias)
 *   uint32_t __gj_graph_milestone_2310  (alias)
 *   __libcgj_batch2310_marker = "libcgj-batch2310"
 *
 * Milestone for the post-2300 buffer exclusive wave (batches 2301-2309:
 * buf_cap_ok, buf_len_ok, buf_room, buf_full_p, buf_empty_p,
 * buf_push_u8, buf_pop_u8, buf_peek_u8, buf_reset). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2310_marker[] = "libcgj-batch2310";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2310_id(void)
{
	return 2310u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2310 - report this TU's graph batch number.
 *
 * Always returns 2310.
 */
uint32_t
gj_batch_id_2310(void)
{
	(void)NULL;
	return b2310_id();
}

/*
 * gj_graph_milestone_2310 - report this TU's graph milestone revision.
 *
 * Always returns 2310 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2310(void)
{
	return b2310_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2310(void)
    __attribute__((alias("gj_batch_id_2310")));

uint32_t __gj_graph_milestone_2310(void)
    __attribute__((alias("gj_graph_milestone_2310")));
