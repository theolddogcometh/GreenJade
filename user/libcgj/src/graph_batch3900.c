/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3900: MILESTONE 3900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3900(void);
 *     - Returns the compile-time graph batch number for this TU (3900).
 *   uint32_t gj_graph_milestone_3900(void);
 *     - Returns the current graph milestone revision (3900).
 *   uint32_t __gj_batch_id_3900  (alias)
 *   uint32_t __gj_graph_milestone_3900  (alias)
 *   __libcgj_batch3900_marker = "libcgj-batch3900"
 *
 * MILESTONE 3900 for the exclusive continuum CREATE-ONLY wave
 * (batches 3891-3899: wave_id_3900, export_hint_3900, smoke_green_3900,
 * dyn_green_3900, shell_green_3900, libcgj_green_3900, bar3_ready_3900,
 * product_score_3900, continuum_ready_3900). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3900_marker[] = "libcgj-batch3900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3900_id(void)
{
	return 3900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3900 - report this TU's graph batch number.
 *
 * Always returns 3900.
 */
uint32_t
gj_batch_id_3900(void)
{
	(void)NULL;
	return b3900_id();
}

/*
 * gj_graph_milestone_3900 - report this TU's graph milestone revision.
 *
 * Always returns 3900 (MILESTONE 3900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3900(void)
{
	return b3900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3900(void)
    __attribute__((alias("gj_batch_id_3900")));

uint32_t __gj_graph_milestone_3900(void)
    __attribute__((alias("gj_graph_milestone_3900")));
