/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6600: MILESTONE 6600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6600(void);
 *     - Returns the compile-time graph batch number for this TU (6600).
 *   uint32_t gj_graph_milestone_6600(void);
 *     - Returns the current graph milestone revision (6600).
 *   uint32_t __gj_batch_id_6600  (alias)
 *   uint32_t __gj_graph_milestone_6600  (alias)
 *   __libcgj_batch6600_marker = "libcgj-batch6600"
 *
 * MILESTONE 6600 for the exclusive continuum CREATE-ONLY wave
 * (batches 6591-6599: wave_id_6600, export_hint_6600, smoke_green_6600,
 * dyn_green_6600, shell_green_6600, libcgj_green_6600, bar3_ready_6600,
 * product_score_6600, continuum_ready_6600). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6600_marker[] = "libcgj-batch6600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6600_id(void)
{
	return 6600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6600 - report this TU's graph batch number.
 *
 * Always returns 6600.
 */
uint32_t
gj_batch_id_6600(void)
{
	(void)NULL;
	return b6600_id();
}

/*
 * gj_graph_milestone_6600 - report this TU's graph milestone revision.
 *
 * Always returns 6600 (MILESTONE 6600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6600(void)
{
	return b6600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6600(void)
    __attribute__((alias("gj_batch_id_6600")));

uint32_t __gj_graph_milestone_6600(void)
    __attribute__((alias("gj_graph_milestone_6600")));
