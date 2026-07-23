/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7200: MILESTONE 7200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7200(void);
 *     - Returns the compile-time graph batch number for this TU (7200).
 *   uint32_t gj_graph_milestone_7200(void);
 *     - Returns the current graph milestone revision (7200).
 *   uint32_t __gj_batch_id_7200  (alias)
 *   uint32_t __gj_graph_milestone_7200  (alias)
 *   __libcgj_batch7200_marker = "libcgj-batch7200"
 *
 * MILESTONE 7200 for the exclusive continuum CREATE-ONLY wave
 * (batches 7191-7199: wave_id_7200, export_hint_7200, smoke_green_7200,
 * dyn_green_7200, shell_green_7200, libcgj_green_7200, bar3_ready_7200,
 * product_score_7200, continuum_ready_7200). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7200_marker[] = "libcgj-batch7200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7200_id(void)
{
	return 7200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7200 - report this TU's graph batch number.
 *
 * Always returns 7200.
 */
uint32_t
gj_batch_id_7200(void)
{
	(void)NULL;
	return b7200_id();
}

/*
 * gj_graph_milestone_7200 - report this TU's graph milestone revision.
 *
 * Always returns 7200 (MILESTONE 7200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7200(void)
{
	return b7200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7200(void)
    __attribute__((alias("gj_batch_id_7200")));

uint32_t __gj_graph_milestone_7200(void)
    __attribute__((alias("gj_graph_milestone_7200")));
