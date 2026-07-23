/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7600: MILESTONE 7600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7600(void);
 *     - Returns the compile-time graph batch number for this TU (7600).
 *   uint32_t gj_graph_milestone_7600(void);
 *     - Returns the current graph milestone revision (7600).
 *   uint32_t __gj_batch_id_7600  (alias)
 *   uint32_t __gj_graph_milestone_7600  (alias)
 *   __libcgj_batch7600_marker = "libcgj-batch7600"
 *
 * MILESTONE 7600 for the exclusive continuum CREATE-ONLY wave
 * (batches 7591-7599: wave_id_7600, export_hint_7600, smoke_green_7600,
 * dyn_green_7600, shell_green_7600, libcgj_green_7600, bar3_ready_7600,
 * product_score_7600, continuum_ready_7600). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7600_marker[] = "libcgj-batch7600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7600_id(void)
{
	return 7600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7600 - report this TU's graph batch number.
 *
 * Always returns 7600.
 */
uint32_t
gj_batch_id_7600(void)
{
	(void)NULL;
	return b7600_id();
}

/*
 * gj_graph_milestone_7600 - report this TU's graph milestone revision.
 *
 * Always returns 7600 (MILESTONE 7600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7600(void)
{
	return b7600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7600(void)
    __attribute__((alias("gj_batch_id_7600")));

uint32_t __gj_graph_milestone_7600(void)
    __attribute__((alias("gj_graph_milestone_7600")));
