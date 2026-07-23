/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3550: MILESTONE 3550 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3550(void);
 *     - Returns the compile-time graph batch number for this TU (3550).
 *   uint32_t gj_graph_milestone_3550(void);
 *     - Returns the current graph milestone revision (3550).
 *   uint32_t __gj_batch_id_3550  (alias)
 *   uint32_t __gj_graph_milestone_3550  (alias)
 *   __libcgj_batch3550_marker = "libcgj-batch3550"
 *
 * MILESTONE 3550 for the exclusive continuum CREATE-ONLY wave
 * (batches 3541-3549: wave_id_3550, export_hint_3550, smoke_green_3550,
 * dyn_green_3550, shell_green_3550, libcgj_green_3550, bar3_ready_3550,
 * product_score_3550, continuum_ready_3550). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3550_marker[] = "libcgj-batch3550";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3550_id(void)
{
	return 3550u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3550 - report this TU's graph batch number.
 *
 * Always returns 3550.
 */
uint32_t
gj_batch_id_3550(void)
{
	(void)NULL;
	return b3550_id();
}

/*
 * gj_graph_milestone_3550 - report this TU's graph milestone revision.
 *
 * Always returns 3550 (MILESTONE 3550). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3550(void)
{
	return b3550_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3550(void)
    __attribute__((alias("gj_batch_id_3550")));

uint32_t __gj_graph_milestone_3550(void)
    __attribute__((alias("gj_graph_milestone_3550")));
