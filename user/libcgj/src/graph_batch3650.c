/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3650: MILESTONE 3650 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3650(void);
 *     - Returns the compile-time graph batch number for this TU (3650).
 *   uint32_t gj_graph_milestone_3650(void);
 *     - Returns the current graph milestone revision (3650).
 *   uint32_t __gj_batch_id_3650  (alias)
 *   uint32_t __gj_graph_milestone_3650  (alias)
 *   __libcgj_batch3650_marker = "libcgj-batch3650"
 *
 * MILESTONE 3650 for the exclusive continuum CREATE-ONLY wave
 * (batches 3641-3649: wave_id_3650, export_hint_3650, smoke_green_3650,
 * dyn_green_3650, shell_green_3650, libcgj_green_3650, bar3_ready_3650,
 * product_score_3650, continuum_ready_3650). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3650_marker[] = "libcgj-batch3650";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3650_id(void)
{
	return 3650u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3650 - report this TU's graph batch number.
 *
 * Always returns 3650.
 */
uint32_t
gj_batch_id_3650(void)
{
	(void)NULL;
	return b3650_id();
}

/*
 * gj_graph_milestone_3650 - report this TU's graph milestone revision.
 *
 * Always returns 3650 (MILESTONE 3650). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3650(void)
{
	return b3650_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3650(void)
    __attribute__((alias("gj_batch_id_3650")));

uint32_t __gj_graph_milestone_3650(void)
    __attribute__((alias("gj_graph_milestone_3650")));
