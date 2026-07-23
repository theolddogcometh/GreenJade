/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4400: MILESTONE 4400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4400(void);
 *     - Returns the compile-time graph batch number for this TU (4400).
 *   uint32_t gj_graph_milestone_4400(void);
 *     - Returns the current graph milestone revision (4400).
 *   uint32_t __gj_batch_id_4400  (alias)
 *   uint32_t __gj_graph_milestone_4400  (alias)
 *   __libcgj_batch4400_marker = "libcgj-batch4400"
 *
 * MILESTONE 4400 for the exclusive continuum CREATE-ONLY wave
 * (batches 4391-4399: wave_id_4400, export_hint_4400, smoke_green_4400,
 * dyn_green_4400, shell_green_4400, libcgj_green_4400, bar3_ready_4400,
 * product_score_4400, continuum_ready_4400). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4400_marker[] = "libcgj-batch4400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4400_id(void)
{
	return 4400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4400 - report this TU's graph batch number.
 *
 * Always returns 4400.
 */
uint32_t
gj_batch_id_4400(void)
{
	(void)NULL;
	return b4400_id();
}

/*
 * gj_graph_milestone_4400 - report this TU's graph milestone revision.
 *
 * Always returns 4400 (MILESTONE 4400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4400(void)
{
	return b4400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4400(void)
    __attribute__((alias("gj_batch_id_4400")));

uint32_t __gj_graph_milestone_4400(void)
    __attribute__((alias("gj_graph_milestone_4400")));
