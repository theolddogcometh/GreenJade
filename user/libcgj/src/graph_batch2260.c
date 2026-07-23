/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2260: milestone 2260 batch identity (closes
 * post-2250 unicode exclusive lightweight wave 2251-2260).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2260(void);
 *     - Returns the compile-time graph batch number for this TU (2260).
 *   uint32_t gj_graph_milestone_2260(void);
 *     - Returns the current graph milestone revision (2260).
 *   uint32_t __gj_batch_id_2260  (alias)
 *   uint32_t __gj_graph_milestone_2260  (alias)
 *   __libcgj_batch2260_marker = "libcgj-batch2260"
 *
 * Milestone for the exclusive unicode helpers wave (batches 2251-2259:
 * utf8 cont/lead1-4/seq_len, unicode surrogate/scalar/bmp predicates).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2260_marker[] = "libcgj-batch2260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2260_id(void)
{
	return 2260u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2260 - report this TU's graph batch number.
 *
 * Always returns 2260.
 */
uint32_t
gj_batch_id_2260(void)
{
	(void)NULL;
	return b2260_id();
}

/*
 * gj_graph_milestone_2260 - report this TU's graph milestone revision.
 *
 * Always returns 2260 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2260(void)
{
	return b2260_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2260(void)
    __attribute__((alias("gj_batch_id_2260")));

uint32_t __gj_graph_milestone_2260(void)
    __attribute__((alias("gj_graph_milestone_2260")));
