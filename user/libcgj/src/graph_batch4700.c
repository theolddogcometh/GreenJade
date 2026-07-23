/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4700: MILESTONE 4700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4700(void);
 *     - Returns the compile-time graph batch number for this TU (4700).
 *   uint32_t gj_graph_milestone_4700(void);
 *     - Returns the current graph milestone revision (4700).
 *   uint32_t __gj_batch_id_4700  (alias)
 *   uint32_t __gj_graph_milestone_4700  (alias)
 *   __libcgj_batch4700_marker = "libcgj-batch4700"
 *
 * MILESTONE 4700 for the exclusive continuum CREATE-ONLY wave
 * (batches 4691-4699: wave_id_4700, export_hint_4700, smoke_green_4700,
 * dyn_green_4700, shell_green_4700, libcgj_green_4700, bar3_ready_4700,
 * product_score_4700, continuum_ready_4700). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4700_marker[] = "libcgj-batch4700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4700_id(void)
{
	return 4700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4700 - report this TU's graph batch number.
 *
 * Always returns 4700.
 */
uint32_t
gj_batch_id_4700(void)
{
	(void)NULL;
	return b4700_id();
}

/*
 * gj_graph_milestone_4700 - report this TU's graph milestone revision.
 *
 * Always returns 4700 (MILESTONE 4700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4700(void)
{
	return b4700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4700(void)
    __attribute__((alias("gj_batch_id_4700")));

uint32_t __gj_graph_milestone_4700(void)
    __attribute__((alias("gj_graph_milestone_4700")));
