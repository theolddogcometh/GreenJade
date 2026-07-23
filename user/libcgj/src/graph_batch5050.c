/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5050: MILESTONE 5050 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5050(void);
 *     - Returns the compile-time graph batch number for this TU (5050).
 *   uint32_t gj_graph_milestone_5050(void);
 *     - Returns the current graph milestone revision (5050).
 *   uint32_t __gj_batch_id_5050  (alias)
 *   uint32_t __gj_graph_milestone_5050  (alias)
 *   __libcgj_batch5050_marker = "libcgj-batch5050"
 *
 * MILESTONE 5050 for the exclusive continuum CREATE-ONLY wave
 * (batches 5041-5049: wave_id_5050, export_hint_5050, smoke_green_5050,
 * dyn_green_5050, shell_green_5050, libcgj_green_5050, bar3_ready_5050,
 * product_score_5050, continuum_ready_5050). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5050_marker[] = "libcgj-batch5050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5050_id(void)
{
	return 5050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5050 - report this TU's graph batch number.
 *
 * Always returns 5050.
 */
uint32_t
gj_batch_id_5050(void)
{
	(void)NULL;
	return b5050_id();
}

/*
 * gj_graph_milestone_5050 - report this TU's graph milestone revision.
 *
 * Always returns 5050 (MILESTONE 5050). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5050(void)
{
	return b5050_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5050(void)
    __attribute__((alias("gj_batch_id_5050")));

uint32_t __gj_graph_milestone_5050(void)
    __attribute__((alias("gj_graph_milestone_5050")));
