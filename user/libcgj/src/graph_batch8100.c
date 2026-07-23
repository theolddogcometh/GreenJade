/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8100: MILESTONE 8100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8100(void);
 *     - Returns the compile-time graph batch number for this TU (8100).
 *   uint32_t gj_graph_milestone_8100(void);
 *     - Returns the current graph milestone revision (8100).
 *   uint32_t __gj_batch_id_8100  (alias)
 *   uint32_t __gj_graph_milestone_8100  (alias)
 *   __libcgj_batch8100_marker = "libcgj-batch8100"
 *
 * MILESTONE 8100 for the exclusive continuum CREATE-ONLY wave
 * (batches 8091-8099: wave_id_8100, export_hint_8100, smoke_green_8100,
 * dyn_green_8100, shell_green_8100, libcgj_green_8100, bar3_ready_8100,
 * product_score_8100, continuum_ready_8100). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8100_marker[] = "libcgj-batch8100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8100_id(void)
{
	return 8100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8100 - report this TU's graph batch number.
 *
 * Always returns 8100.
 */
uint32_t
gj_batch_id_8100(void)
{
	(void)NULL;
	return b8100_id();
}

/*
 * gj_graph_milestone_8100 - report this TU's graph milestone revision.
 *
 * Always returns 8100 (MILESTONE 8100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8100(void)
{
	return b8100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8100(void)
    __attribute__((alias("gj_batch_id_8100")));

uint32_t __gj_graph_milestone_8100(void)
    __attribute__((alias("gj_graph_milestone_8100")));
