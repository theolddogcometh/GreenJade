/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6200: MILESTONE 6200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6200(void);
 *     - Returns the compile-time graph batch number for this TU (6200).
 *   uint32_t gj_graph_milestone_6200(void);
 *     - Returns the current graph milestone revision (6200).
 *   uint32_t __gj_batch_id_6200  (alias)
 *   uint32_t __gj_graph_milestone_6200  (alias)
 *   __libcgj_batch6200_marker = "libcgj-batch6200"
 *
 * MILESTONE 6200 for the exclusive continuum CREATE-ONLY wave
 * (batches 6191-6199: wave_id_6200, export_hint_6200, smoke_green_6200,
 * dyn_green_6200, shell_green_6200, libcgj_green_6200, bar3_ready_6200,
 * product_score_6200, continuum_ready_6200). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6200_marker[] = "libcgj-batch6200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6200_id(void)
{
	return 6200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6200 - report this TU's graph batch number.
 *
 * Always returns 6200.
 */
uint32_t
gj_batch_id_6200(void)
{
	(void)NULL;
	return b6200_id();
}

/*
 * gj_graph_milestone_6200 - report this TU's graph milestone revision.
 *
 * Always returns 6200 (MILESTONE 6200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6200(void)
{
	return b6200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6200(void)
    __attribute__((alias("gj_batch_id_6200")));

uint32_t __gj_graph_milestone_6200(void)
    __attribute__((alias("gj_graph_milestone_6200")));
