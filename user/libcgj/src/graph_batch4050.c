/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4050: MILESTONE 4050 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4050(void);
 *     - Returns the compile-time graph batch number for this TU (4050).
 *   uint32_t gj_graph_milestone_4050(void);
 *     - Returns the current graph milestone revision (4050).
 *   uint32_t __gj_batch_id_4050  (alias)
 *   uint32_t __gj_graph_milestone_4050  (alias)
 *   __libcgj_batch4050_marker = "libcgj-batch4050"
 *
 * MILESTONE 4050 for the exclusive continuum CREATE-ONLY wave
 * (batches 4041-4049: wave_id_4050, export_hint_4050, smoke_green_4050,
 * dyn_green_4050, shell_green_4050, libcgj_green_4050, bar3_ready_4050,
 * product_score_4050, continuum_ready_4050). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4050_marker[] = "libcgj-batch4050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4050_id(void)
{
	return 4050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4050 - report this TU's graph batch number.
 *
 * Always returns 4050.
 */
uint32_t
gj_batch_id_4050(void)
{
	(void)NULL;
	return b4050_id();
}

/*
 * gj_graph_milestone_4050 - report this TU's graph milestone revision.
 *
 * Always returns 4050 (MILESTONE 4050). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4050(void)
{
	return b4050_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4050(void)
    __attribute__((alias("gj_batch_id_4050")));

uint32_t __gj_graph_milestone_4050(void)
    __attribute__((alias("gj_graph_milestone_4050")));
