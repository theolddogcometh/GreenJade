/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4750: MILESTONE 4750 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4750(void);
 *     - Returns the compile-time graph batch number for this TU (4750).
 *   uint32_t gj_graph_milestone_4750(void);
 *     - Returns the current graph milestone revision (4750).
 *   uint32_t __gj_batch_id_4750  (alias)
 *   uint32_t __gj_graph_milestone_4750  (alias)
 *   __libcgj_batch4750_marker = "libcgj-batch4750"
 *
 * MILESTONE 4750 for the exclusive continuum CREATE-ONLY wave
 * (batches 4741-4749: wave_id_4750, export_hint_4750, smoke_green_4750,
 * dyn_green_4750, shell_green_4750, libcgj_green_4750, bar3_ready_4750,
 * product_score_4750, continuum_ready_4750). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4750_marker[] = "libcgj-batch4750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4750_id(void)
{
	return 4750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4750 - report this TU's graph batch number.
 *
 * Always returns 4750.
 */
uint32_t
gj_batch_id_4750(void)
{
	(void)NULL;
	return b4750_id();
}

/*
 * gj_graph_milestone_4750 - report this TU's graph milestone revision.
 *
 * Always returns 4750 (MILESTONE 4750). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4750(void)
{
	return b4750_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4750(void)
    __attribute__((alias("gj_batch_id_4750")));

uint32_t __gj_graph_milestone_4750(void)
    __attribute__((alias("gj_graph_milestone_4750")));
