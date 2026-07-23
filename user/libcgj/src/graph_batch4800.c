/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4800: MILESTONE 4800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4800(void);
 *     - Returns the compile-time graph batch number for this TU (4800).
 *   uint32_t gj_graph_milestone_4800(void);
 *     - Returns the current graph milestone revision (4800).
 *   uint32_t __gj_batch_id_4800  (alias)
 *   uint32_t __gj_graph_milestone_4800  (alias)
 *   __libcgj_batch4800_marker = "libcgj-batch4800"
 *
 * MILESTONE 4800 for the exclusive continuum CREATE-ONLY wave
 * (batches 4791-4799: wave_id_4800, export_hint_4800, smoke_green_4800,
 * dyn_green_4800, shell_green_4800, libcgj_green_4800, bar3_ready_4800,
 * product_score_4800, continuum_ready_4800). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4800_marker[] = "libcgj-batch4800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4800_id(void)
{
	return 4800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4800 - report this TU's graph batch number.
 *
 * Always returns 4800.
 */
uint32_t
gj_batch_id_4800(void)
{
	(void)NULL;
	return b4800_id();
}

/*
 * gj_graph_milestone_4800 - report this TU's graph milestone revision.
 *
 * Always returns 4800 (MILESTONE 4800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4800(void)
{
	return b4800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4800(void)
    __attribute__((alias("gj_batch_id_4800")));

uint32_t __gj_graph_milestone_4800(void)
    __attribute__((alias("gj_graph_milestone_4800")));
