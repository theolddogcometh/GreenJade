/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6400: MILESTONE 6400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6400(void);
 *     - Returns the compile-time graph batch number for this TU (6400).
 *   uint32_t gj_graph_milestone_6400(void);
 *     - Returns the current graph milestone revision (6400).
 *   uint32_t __gj_batch_id_6400  (alias)
 *   uint32_t __gj_graph_milestone_6400  (alias)
 *   __libcgj_batch6400_marker = "libcgj-batch6400"
 *
 * MILESTONE 6400 for the exclusive continuum CREATE-ONLY wave
 * (batches 6391-6399: wave_id_6400, export_hint_6400, smoke_green_6400,
 * dyn_green_6400, shell_green_6400, libcgj_green_6400, bar3_ready_6400,
 * product_score_6400, continuum_ready_6400). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6400_marker[] = "libcgj-batch6400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6400_id(void)
{
	return 6400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6400 - report this TU's graph batch number.
 *
 * Always returns 6400.
 */
uint32_t
gj_batch_id_6400(void)
{
	(void)NULL;
	return b6400_id();
}

/*
 * gj_graph_milestone_6400 - report this TU's graph milestone revision.
 *
 * Always returns 6400 (MILESTONE 6400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6400(void)
{
	return b6400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6400(void)
    __attribute__((alias("gj_batch_id_6400")));

uint32_t __gj_graph_milestone_6400(void)
    __attribute__((alias("gj_graph_milestone_6400")));
