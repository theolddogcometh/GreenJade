/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5900: MILESTONE 5900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5900(void);
 *     - Returns the compile-time graph batch number for this TU (5900).
 *   uint32_t gj_graph_milestone_5900(void);
 *     - Returns the current graph milestone revision (5900).
 *   uint32_t __gj_batch_id_5900  (alias)
 *   uint32_t __gj_graph_milestone_5900  (alias)
 *   __libcgj_batch5900_marker = "libcgj-batch5900"
 *
 * MILESTONE 5900 for the exclusive continuum CREATE-ONLY wave
 * (batches 5891-5899: wave_id_5900, export_hint_5900, smoke_green_5900,
 * dyn_green_5900, shell_green_5900, libcgj_green_5900, bar3_ready_5900,
 * product_score_5900, continuum_ready_5900). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5900_marker[] = "libcgj-batch5900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5900_id(void)
{
	return 5900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5900 - report this TU's graph batch number.
 *
 * Always returns 5900.
 */
uint32_t
gj_batch_id_5900(void)
{
	(void)NULL;
	return b5900_id();
}

/*
 * gj_graph_milestone_5900 - report this TU's graph milestone revision.
 *
 * Always returns 5900 (MILESTONE 5900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5900(void)
{
	return b5900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5900(void)
    __attribute__((alias("gj_batch_id_5900")));

uint32_t __gj_graph_milestone_5900(void)
    __attribute__((alias("gj_graph_milestone_5900")));
