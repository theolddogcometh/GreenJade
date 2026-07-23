/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4850: MILESTONE 4850 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4850(void);
 *     - Returns the compile-time graph batch number for this TU (4850).
 *   uint32_t gj_graph_milestone_4850(void);
 *     - Returns the current graph milestone revision (4850).
 *   uint32_t __gj_batch_id_4850  (alias)
 *   uint32_t __gj_graph_milestone_4850  (alias)
 *   __libcgj_batch4850_marker = "libcgj-batch4850"
 *
 * MILESTONE 4850 for the exclusive continuum CREATE-ONLY wave
 * (batches 4841-4849: wave_id_4850, export_hint_4850, smoke_green_4850,
 * dyn_green_4850, shell_green_4850, libcgj_green_4850, bar3_ready_4850,
 * product_score_4850, continuum_ready_4850). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4850_marker[] = "libcgj-batch4850";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4850_id(void)
{
	return 4850u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4850 - report this TU's graph batch number.
 *
 * Always returns 4850.
 */
uint32_t
gj_batch_id_4850(void)
{
	(void)NULL;
	return b4850_id();
}

/*
 * gj_graph_milestone_4850 - report this TU's graph milestone revision.
 *
 * Always returns 4850 (MILESTONE 4850). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4850(void)
{
	return b4850_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4850(void)
    __attribute__((alias("gj_batch_id_4850")));

uint32_t __gj_graph_milestone_4850(void)
    __attribute__((alias("gj_graph_milestone_4850")));
