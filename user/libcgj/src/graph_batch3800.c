/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3800: MILESTONE 3800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3800(void);
 *     - Returns the compile-time graph batch number for this TU (3800).
 *   uint32_t gj_graph_milestone_3800(void);
 *     - Returns the current graph milestone revision (3800).
 *   uint32_t __gj_batch_id_3800  (alias)
 *   uint32_t __gj_graph_milestone_3800  (alias)
 *   __libcgj_batch3800_marker = "libcgj-batch3800"
 *
 * MILESTONE 3800 for the exclusive continuum CREATE-ONLY wave
 * (batches 3791-3799: wave_id_3800, export_hint_3800, smoke_green_3800,
 * dyn_green_3800, shell_green_3800, libcgj_green_3800, bar3_ready_3800,
 * product_score_3800, continuum_ready_3800). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3800_marker[] = "libcgj-batch3800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3800_id(void)
{
	return 3800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3800 - report this TU's graph batch number.
 *
 * Always returns 3800.
 */
uint32_t
gj_batch_id_3800(void)
{
	(void)NULL;
	return b3800_id();
}

/*
 * gj_graph_milestone_3800 - report this TU's graph milestone revision.
 *
 * Always returns 3800 (MILESTONE 3800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3800(void)
{
	return b3800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3800(void)
    __attribute__((alias("gj_batch_id_3800")));

uint32_t __gj_graph_milestone_3800(void)
    __attribute__((alias("gj_graph_milestone_3800")));
