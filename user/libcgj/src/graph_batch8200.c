/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8200: MILESTONE 8200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8200(void);
 *     - Returns the compile-time graph batch number for this TU (8200).
 *   uint32_t gj_graph_milestone_8200(void);
 *     - Returns the current graph milestone revision (8200).
 *   uint32_t __gj_batch_id_8200  (alias)
 *   uint32_t __gj_graph_milestone_8200  (alias)
 *   __libcgj_batch8200_marker = "libcgj-batch8200"
 *
 * MILESTONE 8200 for the exclusive continuum CREATE-ONLY wave
 * (batches 8191-8199: wave_id_8200, export_hint_8200, smoke_green_8200,
 * dyn_green_8200, shell_green_8200, libcgj_green_8200, bar3_ready_8200,
 * product_score_8200, continuum_ready_8200). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8200_marker[] = "libcgj-batch8200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8200_id(void)
{
	return 8200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8200 - report this TU's graph batch number.
 *
 * Always returns 8200.
 */
uint32_t
gj_batch_id_8200(void)
{
	(void)NULL;
	return b8200_id();
}

/*
 * gj_graph_milestone_8200 - report this TU's graph milestone revision.
 *
 * Always returns 8200 (MILESTONE 8200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8200(void)
{
	return b8200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8200(void)
    __attribute__((alias("gj_batch_id_8200")));

uint32_t __gj_graph_milestone_8200(void)
    __attribute__((alias("gj_graph_milestone_8200")));
