/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4250: MILESTONE 4250 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4250(void);
 *     - Returns the compile-time graph batch number for this TU (4250).
 *   uint32_t gj_graph_milestone_4250(void);
 *     - Returns the current graph milestone revision (4250).
 *   uint32_t __gj_batch_id_4250  (alias)
 *   uint32_t __gj_graph_milestone_4250  (alias)
 *   __libcgj_batch4250_marker = "libcgj-batch4250"
 *
 * MILESTONE 4250 for the exclusive continuum CREATE-ONLY wave
 * (batches 4241-4249: wave_id_4250, export_hint_4250, smoke_green_4250,
 * dyn_green_4250, shell_green_4250, libcgj_green_4250, bar3_ready_4250,
 * product_score_4250, continuum_ready_4250). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4250_marker[] = "libcgj-batch4250";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4250_id(void)
{
	return 4250u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4250 - report this TU's graph batch number.
 *
 * Always returns 4250.
 */
uint32_t
gj_batch_id_4250(void)
{
	(void)NULL;
	return b4250_id();
}

/*
 * gj_graph_milestone_4250 - report this TU's graph milestone revision.
 *
 * Always returns 4250 (MILESTONE 4250). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4250(void)
{
	return b4250_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4250(void)
    __attribute__((alias("gj_batch_id_4250")));

uint32_t __gj_graph_milestone_4250(void)
    __attribute__((alias("gj_graph_milestone_4250")));
