/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4100: MILESTONE 4100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4100(void);
 *     - Returns the compile-time graph batch number for this TU (4100).
 *   uint32_t gj_graph_milestone_4100(void);
 *     - Returns the current graph milestone revision (4100).
 *   uint32_t __gj_batch_id_4100  (alias)
 *   uint32_t __gj_graph_milestone_4100  (alias)
 *   __libcgj_batch4100_marker = "libcgj-batch4100"
 *
 * MILESTONE 4100 for the exclusive continuum CREATE-ONLY wave
 * (batches 4091-4099: wave_id_4100, export_hint_4100, smoke_green_4100,
 * dyn_green_4100, shell_green_4100, libcgj_green_4100, bar3_ready_4100,
 * product_score_4100, continuum_ready_4100). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4100_marker[] = "libcgj-batch4100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4100_id(void)
{
	return 4100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4100 - report this TU's graph batch number.
 *
 * Always returns 4100.
 */
uint32_t
gj_batch_id_4100(void)
{
	(void)NULL;
	return b4100_id();
}

/*
 * gj_graph_milestone_4100 - report this TU's graph milestone revision.
 *
 * Always returns 4100 (MILESTONE 4100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4100(void)
{
	return b4100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4100(void)
    __attribute__((alias("gj_batch_id_4100")));

uint32_t __gj_graph_milestone_4100(void)
    __attribute__((alias("gj_graph_milestone_4100")));
