/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3700: MILESTONE 3700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3700(void);
 *     - Returns the compile-time graph batch number for this TU (3700).
 *   uint32_t gj_graph_milestone_3700(void);
 *     - Returns the current graph milestone revision (3700).
 *   uint32_t __gj_batch_id_3700  (alias)
 *   uint32_t __gj_graph_milestone_3700  (alias)
 *   __libcgj_batch3700_marker = "libcgj-batch3700"
 *
 * MILESTONE 3700 for the exclusive continuum CREATE-ONLY wave
 * (batches 3691-3699: wave_id_3700, export_hint_3700, smoke_green_3700,
 * dyn_green_3700, shell_green_3700, libcgj_green_3700, bar3_ready_3700,
 * product_score_3700, continuum_ready_3700). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3700_marker[] = "libcgj-batch3700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3700_id(void)
{
	return 3700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3700 - report this TU's graph batch number.
 *
 * Always returns 3700.
 */
uint32_t
gj_batch_id_3700(void)
{
	(void)NULL;
	return b3700_id();
}

/*
 * gj_graph_milestone_3700 - report this TU's graph milestone revision.
 *
 * Always returns 3700 (MILESTONE 3700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3700(void)
{
	return b3700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3700(void)
    __attribute__((alias("gj_batch_id_3700")));

uint32_t __gj_graph_milestone_3700(void)
    __attribute__((alias("gj_graph_milestone_3700")));
