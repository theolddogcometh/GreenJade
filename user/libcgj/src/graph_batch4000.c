/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4000: MILESTONE 4000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4000(void);
 *     - Returns the compile-time graph batch number for this TU (4000).
 *   uint32_t gj_graph_milestone_4000(void);
 *     - Returns the current graph milestone revision (4000).
 *   uint32_t __gj_batch_id_4000  (alias)
 *   uint32_t __gj_graph_milestone_4000  (alias)
 *   __libcgj_batch4000_marker = "libcgj-batch4000"
 *
 * MILESTONE 4000 for the exclusive continuum CREATE-ONLY wave
 * (batches 3991-3999: wave_id_4000, export_hint_4000, smoke_green_4000,
 * dyn_green_4000, shell_green_4000, libcgj_green_4000, bar3_ready_4000,
 * product_score_4000, continuum_ready_4000). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4000_marker[] = "libcgj-batch4000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4000_id(void)
{
	return 4000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4000 - report this TU's graph batch number.
 *
 * Always returns 4000.
 */
uint32_t
gj_batch_id_4000(void)
{
	(void)NULL;
	return b4000_id();
}

/*
 * gj_graph_milestone_4000 - report this TU's graph milestone revision.
 *
 * Always returns 4000 (MILESTONE 4000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4000(void)
{
	return b4000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4000(void)
    __attribute__((alias("gj_batch_id_4000")));

uint32_t __gj_graph_milestone_4000(void)
    __attribute__((alias("gj_graph_milestone_4000")));
