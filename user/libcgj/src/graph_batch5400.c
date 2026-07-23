/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5400: MILESTONE 5400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5400(void);
 *     - Returns the compile-time graph batch number for this TU (5400).
 *   uint32_t gj_graph_milestone_5400(void);
 *     - Returns the current graph milestone revision (5400).
 *   uint32_t __gj_batch_id_5400  (alias)
 *   uint32_t __gj_graph_milestone_5400  (alias)
 *   __libcgj_batch5400_marker = "libcgj-batch5400"
 *
 * MILESTONE 5400 for the exclusive continuum CREATE-ONLY wave
 * (batches 5391-5399: wave_id_5400, export_hint_5400, smoke_green_5400,
 * dyn_green_5400, shell_green_5400, libcgj_green_5400, bar3_ready_5400,
 * product_score_5400, continuum_ready_5400). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5400_marker[] = "libcgj-batch5400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5400_id(void)
{
	return 5400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5400 - report this TU's graph batch number.
 *
 * Always returns 5400.
 */
uint32_t
gj_batch_id_5400(void)
{
	(void)NULL;
	return b5400_id();
}

/*
 * gj_graph_milestone_5400 - report this TU's graph milestone revision.
 *
 * Always returns 5400 (MILESTONE 5400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5400(void)
{
	return b5400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5400(void)
    __attribute__((alias("gj_batch_id_5400")));

uint32_t __gj_graph_milestone_5400(void)
    __attribute__((alias("gj_graph_milestone_5400")));
