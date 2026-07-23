/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5000: MILESTONE 5000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5000(void);
 *     - Returns the compile-time graph batch number for this TU (5000).
 *   uint32_t gj_graph_milestone_5000(void);
 *     - Returns the current graph milestone revision (5000).
 *   uint32_t __gj_batch_id_5000  (alias)
 *   uint32_t __gj_graph_milestone_5000  (alias)
 *   __libcgj_batch5000_marker = "libcgj-batch5000"
 *
 * MILESTONE 5000 for the exclusive continuum CREATE-ONLY wave
 * (batches 4991-4999: wave_id_5000, export_hint_5000, smoke_green_5000,
 * dyn_green_5000, shell_green_5000, libcgj_green_5000, bar3_ready_5000,
 * product_score_5000, continuum_ready_5000). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5000_marker[] = "libcgj-batch5000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5000_id(void)
{
	return 5000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5000 - report this TU's graph batch number.
 *
 * Always returns 5000.
 */
uint32_t
gj_batch_id_5000(void)
{
	(void)NULL;
	return b5000_id();
}

/*
 * gj_graph_milestone_5000 - report this TU's graph milestone revision.
 *
 * Always returns 5000 (MILESTONE 5000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5000(void)
{
	return b5000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5000(void)
    __attribute__((alias("gj_batch_id_5000")));

uint32_t __gj_graph_milestone_5000(void)
    __attribute__((alias("gj_graph_milestone_5000")));
