/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4150: MILESTONE 4150 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4150(void);
 *     - Returns the compile-time graph batch number for this TU (4150).
 *   uint32_t gj_graph_milestone_4150(void);
 *     - Returns the current graph milestone revision (4150).
 *   uint32_t __gj_batch_id_4150  (alias)
 *   uint32_t __gj_graph_milestone_4150  (alias)
 *   __libcgj_batch4150_marker = "libcgj-batch4150"
 *
 * MILESTONE 4150 for the exclusive continuum CREATE-ONLY wave
 * (batches 4141-4149: wave_id_4150, export_hint_4150, smoke_green_4150,
 * dyn_green_4150, shell_green_4150, libcgj_green_4150, bar3_ready_4150,
 * product_score_4150, continuum_ready_4150). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4150_marker[] = "libcgj-batch4150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4150_id(void)
{
	return 4150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4150 - report this TU's graph batch number.
 *
 * Always returns 4150.
 */
uint32_t
gj_batch_id_4150(void)
{
	(void)NULL;
	return b4150_id();
}

/*
 * gj_graph_milestone_4150 - report this TU's graph milestone revision.
 *
 * Always returns 4150 (MILESTONE 4150). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4150(void)
{
	return b4150_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4150(void)
    __attribute__((alias("gj_batch_id_4150")));

uint32_t __gj_graph_milestone_4150(void)
    __attribute__((alias("gj_graph_milestone_4150")));
