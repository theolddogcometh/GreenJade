/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7100: MILESTONE 7100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7100(void);
 *     - Returns the compile-time graph batch number for this TU (7100).
 *   uint32_t gj_graph_milestone_7100(void);
 *     - Returns the current graph milestone revision (7100).
 *   uint32_t __gj_batch_id_7100  (alias)
 *   uint32_t __gj_graph_milestone_7100  (alias)
 *   __libcgj_batch7100_marker = "libcgj-batch7100"
 *
 * MILESTONE 7100 for the exclusive continuum CREATE-ONLY wave
 * (batches 7091-7099: wave_id_7100, export_hint_7100, smoke_green_7100,
 * dyn_green_7100, shell_green_7100, libcgj_green_7100, bar3_ready_7100,
 * product_score_7100, continuum_ready_7100). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7100_marker[] = "libcgj-batch7100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7100_id(void)
{
	return 7100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7100 - report this TU's graph batch number.
 *
 * Always returns 7100.
 */
uint32_t
gj_batch_id_7100(void)
{
	(void)NULL;
	return b7100_id();
}

/*
 * gj_graph_milestone_7100 - report this TU's graph milestone revision.
 *
 * Always returns 7100 (MILESTONE 7100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7100(void)
{
	return b7100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7100(void)
    __attribute__((alias("gj_batch_id_7100")));

uint32_t __gj_graph_milestone_7100(void)
    __attribute__((alias("gj_graph_milestone_7100")));
