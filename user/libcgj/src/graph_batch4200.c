/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4200: MILESTONE 4200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4200(void);
 *     - Returns the compile-time graph batch number for this TU (4200).
 *   uint32_t gj_graph_milestone_4200(void);
 *     - Returns the current graph milestone revision (4200).
 *   uint32_t __gj_batch_id_4200  (alias)
 *   uint32_t __gj_graph_milestone_4200  (alias)
 *   __libcgj_batch4200_marker = "libcgj-batch4200"
 *
 * MILESTONE 4200 for the exclusive continuum CREATE-ONLY wave
 * (batches 4191-4199: wave_id_4200, export_hint_4200, smoke_green_4200,
 * dyn_green_4200, shell_green_4200, libcgj_green_4200, bar3_ready_4200,
 * product_score_4200, continuum_ready_4200). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4200_marker[] = "libcgj-batch4200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4200_id(void)
{
	return 4200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4200 - report this TU's graph batch number.
 *
 * Always returns 4200.
 */
uint32_t
gj_batch_id_4200(void)
{
	(void)NULL;
	return b4200_id();
}

/*
 * gj_graph_milestone_4200 - report this TU's graph milestone revision.
 *
 * Always returns 4200 (MILESTONE 4200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4200(void)
{
	return b4200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4200(void)
    __attribute__((alias("gj_batch_id_4200")));

uint32_t __gj_graph_milestone_4200(void)
    __attribute__((alias("gj_graph_milestone_4200")));
