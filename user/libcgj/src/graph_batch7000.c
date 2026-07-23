/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7000: MILESTONE 7000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7000(void);
 *     - Returns the compile-time graph batch number for this TU (7000).
 *   uint32_t gj_graph_milestone_7000(void);
 *     - Returns the current graph milestone revision (7000).
 *   uint32_t __gj_batch_id_7000  (alias)
 *   uint32_t __gj_graph_milestone_7000  (alias)
 *   __libcgj_batch7000_marker = "libcgj-batch7000"
 *
 * MILESTONE 7000 for the exclusive continuum CREATE-ONLY wave
 * (batches 6991-6999: wave_id_7000, export_hint_7000, smoke_green_7000,
 * dyn_green_7000, shell_green_7000, libcgj_green_7000, bar3_ready_7000,
 * product_score_7000, continuum_ready_7000). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7000_marker[] = "libcgj-batch7000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7000_id(void)
{
	return 7000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7000 - report this TU's graph batch number.
 *
 * Always returns 7000.
 */
uint32_t
gj_batch_id_7000(void)
{
	(void)NULL;
	return b7000_id();
}

/*
 * gj_graph_milestone_7000 - report this TU's graph milestone revision.
 *
 * Always returns 7000 (MILESTONE 7000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7000(void)
{
	return b7000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7000(void)
    __attribute__((alias("gj_batch_id_7000")));

uint32_t __gj_graph_milestone_7000(void)
    __attribute__((alias("gj_graph_milestone_7000")));
