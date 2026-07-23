/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6900: MILESTONE 6900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6900(void);
 *     - Returns the compile-time graph batch number for this TU (6900).
 *   uint32_t gj_graph_milestone_6900(void);
 *     - Returns the current graph milestone revision (6900).
 *   uint32_t __gj_batch_id_6900  (alias)
 *   uint32_t __gj_graph_milestone_6900  (alias)
 *   __libcgj_batch6900_marker = "libcgj-batch6900"
 *
 * MILESTONE 6900 for the exclusive continuum CREATE-ONLY wave
 * (batches 6891-6899: wave_id_6900, export_hint_6900, smoke_green_6900,
 * dyn_green_6900, shell_green_6900, libcgj_green_6900, bar3_ready_6900,
 * product_score_6900, continuum_ready_6900). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6900_marker[] = "libcgj-batch6900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6900_id(void)
{
	return 6900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6900 - report this TU's graph batch number.
 *
 * Always returns 6900.
 */
uint32_t
gj_batch_id_6900(void)
{
	(void)NULL;
	return b6900_id();
}

/*
 * gj_graph_milestone_6900 - report this TU's graph milestone revision.
 *
 * Always returns 6900 (MILESTONE 6900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6900(void)
{
	return b6900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6900(void)
    __attribute__((alias("gj_batch_id_6900")));

uint32_t __gj_graph_milestone_6900(void)
    __attribute__((alias("gj_graph_milestone_6900")));
