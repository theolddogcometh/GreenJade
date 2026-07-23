/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4550: MILESTONE 4550 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4550(void);
 *     - Returns the compile-time graph batch number for this TU (4550).
 *   uint32_t gj_graph_milestone_4550(void);
 *     - Returns the current graph milestone revision (4550).
 *   uint32_t __gj_batch_id_4550  (alias)
 *   uint32_t __gj_graph_milestone_4550  (alias)
 *   __libcgj_batch4550_marker = "libcgj-batch4550"
 *
 * MILESTONE 4550 for the exclusive continuum CREATE-ONLY wave
 * (batches 4541-4549: wave_id_4550, export_hint_4550, smoke_green_4550,
 * dyn_green_4550, shell_green_4550, libcgj_green_4550, bar3_ready_4550,
 * product_score_4550, continuum_ready_4550). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4550_marker[] = "libcgj-batch4550";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4550_id(void)
{
	return 4550u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4550 - report this TU's graph batch number.
 *
 * Always returns 4550.
 */
uint32_t
gj_batch_id_4550(void)
{
	(void)NULL;
	return b4550_id();
}

/*
 * gj_graph_milestone_4550 - report this TU's graph milestone revision.
 *
 * Always returns 4550 (MILESTONE 4550). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4550(void)
{
	return b4550_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4550(void)
    __attribute__((alias("gj_batch_id_4550")));

uint32_t __gj_graph_milestone_4550(void)
    __attribute__((alias("gj_graph_milestone_4550")));
