/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5150: MILESTONE 5150 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5150(void);
 *     - Returns the compile-time graph batch number for this TU (5150).
 *   uint32_t gj_graph_milestone_5150(void);
 *     - Returns the current graph milestone revision (5150).
 *   uint32_t __gj_batch_id_5150  (alias)
 *   uint32_t __gj_graph_milestone_5150  (alias)
 *   __libcgj_batch5150_marker = "libcgj-batch5150"
 *
 * MILESTONE 5150 for the exclusive continuum CREATE-ONLY wave
 * (batches 5141-5149: wave_id_5150, export_hint_5150, smoke_green_5150,
 * dyn_green_5150, shell_green_5150, libcgj_green_5150, bar3_ready_5150,
 * product_score_5150, continuum_ready_5150). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5150_marker[] = "libcgj-batch5150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5150_id(void)
{
	return 5150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5150 - report this TU's graph batch number.
 *
 * Always returns 5150.
 */
uint32_t
gj_batch_id_5150(void)
{
	(void)NULL;
	return b5150_id();
}

/*
 * gj_graph_milestone_5150 - report this TU's graph milestone revision.
 *
 * Always returns 5150 (MILESTONE 5150). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5150(void)
{
	return b5150_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5150(void)
    __attribute__((alias("gj_batch_id_5150")));

uint32_t __gj_graph_milestone_5150(void)
    __attribute__((alias("gj_graph_milestone_5150")));
