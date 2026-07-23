/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5800: MILESTONE 5800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5800(void);
 *     - Returns the compile-time graph batch number for this TU (5800).
 *   uint32_t gj_graph_milestone_5800(void);
 *     - Returns the current graph milestone revision (5800).
 *   uint32_t __gj_batch_id_5800  (alias)
 *   uint32_t __gj_graph_milestone_5800  (alias)
 *   __libcgj_batch5800_marker = "libcgj-batch5800"
 *
 * MILESTONE 5800 for the exclusive continuum CREATE-ONLY wave
 * (batches 5791-5799: wave_id_5800, export_hint_5800, smoke_green_5800,
 * dyn_green_5800, shell_green_5800, libcgj_green_5800, bar3_ready_5800,
 * product_score_5800, continuum_ready_5800). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5800_marker[] = "libcgj-batch5800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5800_id(void)
{
	return 5800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5800 - report this TU's graph batch number.
 *
 * Always returns 5800.
 */
uint32_t
gj_batch_id_5800(void)
{
	(void)NULL;
	return b5800_id();
}

/*
 * gj_graph_milestone_5800 - report this TU's graph milestone revision.
 *
 * Always returns 5800 (MILESTONE 5800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5800(void)
{
	return b5800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5800(void)
    __attribute__((alias("gj_batch_id_5800")));

uint32_t __gj_graph_milestone_5800(void)
    __attribute__((alias("gj_graph_milestone_5800")));
