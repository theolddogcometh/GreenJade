/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5200: MILESTONE 5200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5200(void);
 *     - Returns the compile-time graph batch number for this TU (5200).
 *   uint32_t gj_graph_milestone_5200(void);
 *     - Returns the current graph milestone revision (5200).
 *   uint32_t __gj_batch_id_5200  (alias)
 *   uint32_t __gj_graph_milestone_5200  (alias)
 *   __libcgj_batch5200_marker = "libcgj-batch5200"
 *
 * MILESTONE 5200 for the exclusive continuum CREATE-ONLY wave
 * (batches 5191-5199: wave_id_5200, export_hint_5200, smoke_green_5200,
 * dyn_green_5200, shell_green_5200, libcgj_green_5200, bar3_ready_5200,
 * product_score_5200, continuum_ready_5200). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5200_marker[] = "libcgj-batch5200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5200_id(void)
{
	return 5200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5200 - report this TU's graph batch number.
 *
 * Always returns 5200.
 */
uint32_t
gj_batch_id_5200(void)
{
	(void)NULL;
	return b5200_id();
}

/*
 * gj_graph_milestone_5200 - report this TU's graph milestone revision.
 *
 * Always returns 5200 (MILESTONE 5200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5200(void)
{
	return b5200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5200(void)
    __attribute__((alias("gj_batch_id_5200")));

uint32_t __gj_graph_milestone_5200(void)
    __attribute__((alias("gj_graph_milestone_5200")));
