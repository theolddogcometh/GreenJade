/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6100: MILESTONE 6100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6100(void);
 *     - Returns the compile-time graph batch number for this TU (6100).
 *   uint32_t gj_graph_milestone_6100(void);
 *     - Returns the current graph milestone revision (6100).
 *   uint32_t __gj_batch_id_6100  (alias)
 *   uint32_t __gj_graph_milestone_6100  (alias)
 *   __libcgj_batch6100_marker = "libcgj-batch6100"
 *
 * MILESTONE 6100 for the exclusive continuum CREATE-ONLY wave
 * (batches 6091-6099: wave_id_6100, export_hint_6100, smoke_green_6100,
 * dyn_green_6100, shell_green_6100, libcgj_green_6100, bar3_ready_6100,
 * product_score_6100, continuum_ready_6100). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6100_marker[] = "libcgj-batch6100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6100_id(void)
{
	return 6100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6100 - report this TU's graph batch number.
 *
 * Always returns 6100.
 */
uint32_t
gj_batch_id_6100(void)
{
	(void)NULL;
	return b6100_id();
}

/*
 * gj_graph_milestone_6100 - report this TU's graph milestone revision.
 *
 * Always returns 6100 (MILESTONE 6100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6100(void)
{
	return b6100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6100(void)
    __attribute__((alias("gj_batch_id_6100")));

uint32_t __gj_graph_milestone_6100(void)
    __attribute__((alias("gj_graph_milestone_6100")));
