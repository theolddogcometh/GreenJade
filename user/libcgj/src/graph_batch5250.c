/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5250: MILESTONE 5250 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5250(void);
 *     - Returns the compile-time graph batch number for this TU (5250).
 *   uint32_t gj_graph_milestone_5250(void);
 *     - Returns the current graph milestone revision (5250).
 *   uint32_t __gj_batch_id_5250  (alias)
 *   uint32_t __gj_graph_milestone_5250  (alias)
 *   __libcgj_batch5250_marker = "libcgj-batch5250"
 *
 * MILESTONE 5250 for the exclusive continuum CREATE-ONLY wave
 * (batches 5241-5249: wave_id_5250, export_hint_5250, smoke_green_5250,
 * dyn_green_5250, shell_green_5250, libcgj_green_5250, bar3_ready_5250,
 * product_score_5250, continuum_ready_5250). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5250_marker[] = "libcgj-batch5250";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5250_id(void)
{
	return 5250u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5250 - report this TU's graph batch number.
 *
 * Always returns 5250.
 */
uint32_t
gj_batch_id_5250(void)
{
	(void)NULL;
	return b5250_id();
}

/*
 * gj_graph_milestone_5250 - report this TU's graph milestone revision.
 *
 * Always returns 5250 (MILESTONE 5250). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5250(void)
{
	return b5250_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5250(void)
    __attribute__((alias("gj_batch_id_5250")));

uint32_t __gj_graph_milestone_5250(void)
    __attribute__((alias("gj_graph_milestone_5250")));
