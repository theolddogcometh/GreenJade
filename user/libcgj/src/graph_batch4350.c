/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4350: MILESTONE 4350 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4350(void);
 *     - Returns the compile-time graph batch number for this TU (4350).
 *   uint32_t gj_graph_milestone_4350(void);
 *     - Returns the current graph milestone revision (4350).
 *   uint32_t __gj_batch_id_4350  (alias)
 *   uint32_t __gj_graph_milestone_4350  (alias)
 *   __libcgj_batch4350_marker = "libcgj-batch4350"
 *
 * MILESTONE 4350 for the exclusive continuum CREATE-ONLY wave
 * (batches 4341-4349: wave_id_4350, export_hint_4350, smoke_green_4350,
 * dyn_green_4350, shell_green_4350, libcgj_green_4350, bar3_ready_4350,
 * product_score_4350, continuum_ready_4350). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4350_marker[] = "libcgj-batch4350";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4350_id(void)
{
	return 4350u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4350 - report this TU's graph batch number.
 *
 * Always returns 4350.
 */
uint32_t
gj_batch_id_4350(void)
{
	(void)NULL;
	return b4350_id();
}

/*
 * gj_graph_milestone_4350 - report this TU's graph milestone revision.
 *
 * Always returns 4350 (MILESTONE 4350). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4350(void)
{
	return b4350_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4350(void)
    __attribute__((alias("gj_batch_id_4350")));

uint32_t __gj_graph_milestone_4350(void)
    __attribute__((alias("gj_graph_milestone_4350")));
