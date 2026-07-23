/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5600: MILESTONE 5600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5600(void);
 *     - Returns the compile-time graph batch number for this TU (5600).
 *   uint32_t gj_graph_milestone_5600(void);
 *     - Returns the current graph milestone revision (5600).
 *   uint32_t __gj_batch_id_5600  (alias)
 *   uint32_t __gj_graph_milestone_5600  (alias)
 *   __libcgj_batch5600_marker = "libcgj-batch5600"
 *
 * MILESTONE 5600 for the exclusive continuum CREATE-ONLY wave
 * (batches 5591-5599: wave_id_5600, export_hint_5600, smoke_green_5600,
 * dyn_green_5600, shell_green_5600, libcgj_green_5600, bar3_ready_5600,
 * product_score_5600, continuum_ready_5600). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5600_marker[] = "libcgj-batch5600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5600_id(void)
{
	return 5600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5600 - report this TU's graph batch number.
 *
 * Always returns 5600.
 */
uint32_t
gj_batch_id_5600(void)
{
	(void)NULL;
	return b5600_id();
}

/*
 * gj_graph_milestone_5600 - report this TU's graph milestone revision.
 *
 * Always returns 5600 (MILESTONE 5600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5600(void)
{
	return b5600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5600(void)
    __attribute__((alias("gj_batch_id_5600")));

uint32_t __gj_graph_milestone_5600(void)
    __attribute__((alias("gj_graph_milestone_5600")));
