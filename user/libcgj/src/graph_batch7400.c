/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7400: MILESTONE 7400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7400(void);
 *     - Returns the compile-time graph batch number for this TU (7400).
 *   uint32_t gj_graph_milestone_7400(void);
 *     - Returns the current graph milestone revision (7400).
 *   uint32_t __gj_batch_id_7400  (alias)
 *   uint32_t __gj_graph_milestone_7400  (alias)
 *   __libcgj_batch7400_marker = "libcgj-batch7400"
 *
 * MILESTONE 7400 for the exclusive continuum CREATE-ONLY wave
 * (batches 7391-7399: wave_id_7400, export_hint_7400, smoke_green_7400,
 * dyn_green_7400, shell_green_7400, libcgj_green_7400, bar3_ready_7400,
 * product_score_7400, continuum_ready_7400). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7400_marker[] = "libcgj-batch7400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7400_id(void)
{
	return 7400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7400 - report this TU's graph batch number.
 *
 * Always returns 7400.
 */
uint32_t
gj_batch_id_7400(void)
{
	(void)NULL;
	return b7400_id();
}

/*
 * gj_graph_milestone_7400 - report this TU's graph milestone revision.
 *
 * Always returns 7400 (MILESTONE 7400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7400(void)
{
	return b7400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7400(void)
    __attribute__((alias("gj_batch_id_7400")));

uint32_t __gj_graph_milestone_7400(void)
    __attribute__((alias("gj_graph_milestone_7400")));
