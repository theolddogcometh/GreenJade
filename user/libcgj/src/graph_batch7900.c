/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7900: MILESTONE 7900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7900(void);
 *     - Returns the compile-time graph batch number for this TU (7900).
 *   uint32_t gj_graph_milestone_7900(void);
 *     - Returns the current graph milestone revision (7900).
 *   uint32_t __gj_batch_id_7900  (alias)
 *   uint32_t __gj_graph_milestone_7900  (alias)
 *   __libcgj_batch7900_marker = "libcgj-batch7900"
 *
 * MILESTONE 7900 for the exclusive continuum CREATE-ONLY wave
 * (batches 7891-7899: wave_id_7900, export_hint_7900, smoke_green_7900,
 * dyn_green_7900, shell_green_7900, libcgj_green_7900, bar3_ready_7900,
 * product_score_7900, continuum_ready_7900). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7900_marker[] = "libcgj-batch7900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7900_id(void)
{
	return 7900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7900 - report this TU's graph batch number.
 *
 * Always returns 7900.
 */
uint32_t
gj_batch_id_7900(void)
{
	(void)NULL;
	return b7900_id();
}

/*
 * gj_graph_milestone_7900 - report this TU's graph milestone revision.
 *
 * Always returns 7900 (MILESTONE 7900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7900(void)
{
	return b7900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7900(void)
    __attribute__((alias("gj_batch_id_7900")));

uint32_t __gj_graph_milestone_7900(void)
    __attribute__((alias("gj_graph_milestone_7900")));
