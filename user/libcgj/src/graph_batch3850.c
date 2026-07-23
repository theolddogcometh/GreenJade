/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3850: MILESTONE 3850 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3850(void);
 *     - Returns the compile-time graph batch number for this TU (3850).
 *   uint32_t gj_graph_milestone_3850(void);
 *     - Returns the current graph milestone revision (3850).
 *   uint32_t __gj_batch_id_3850  (alias)
 *   uint32_t __gj_graph_milestone_3850  (alias)
 *   __libcgj_batch3850_marker = "libcgj-batch3850"
 *
 * MILESTONE 3850 for the exclusive continuum CREATE-ONLY wave
 * (batches 3841-3849: wave_id_3850, export_hint_3850, smoke_green_3850,
 * dyn_green_3850, shell_green_3850, libcgj_green_3850, bar3_ready_3850,
 * product_score_3850, continuum_ready_3850). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3850_marker[] = "libcgj-batch3850";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3850_id(void)
{
	return 3850u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3850 - report this TU's graph batch number.
 *
 * Always returns 3850.
 */
uint32_t
gj_batch_id_3850(void)
{
	(void)NULL;
	return b3850_id();
}

/*
 * gj_graph_milestone_3850 - report this TU's graph milestone revision.
 *
 * Always returns 3850 (MILESTONE 3850). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3850(void)
{
	return b3850_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3850(void)
    __attribute__((alias("gj_batch_id_3850")));

uint32_t __gj_graph_milestone_3850(void)
    __attribute__((alias("gj_graph_milestone_3850")));
