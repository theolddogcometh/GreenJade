/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5100: MILESTONE 5100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5100(void);
 *     - Returns the compile-time graph batch number for this TU (5100).
 *   uint32_t gj_graph_milestone_5100(void);
 *     - Returns the current graph milestone revision (5100).
 *   uint32_t __gj_batch_id_5100  (alias)
 *   uint32_t __gj_graph_milestone_5100  (alias)
 *   __libcgj_batch5100_marker = "libcgj-batch5100"
 *
 * MILESTONE 5100 for the exclusive continuum CREATE-ONLY wave
 * (batches 5091-5099: wave_id_5100, export_hint_5100, smoke_green_5100,
 * dyn_green_5100, shell_green_5100, libcgj_green_5100, bar3_ready_5100,
 * product_score_5100, continuum_ready_5100). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5100_marker[] = "libcgj-batch5100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5100_id(void)
{
	return 5100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5100 - report this TU's graph batch number.
 *
 * Always returns 5100.
 */
uint32_t
gj_batch_id_5100(void)
{
	(void)NULL;
	return b5100_id();
}

/*
 * gj_graph_milestone_5100 - report this TU's graph milestone revision.
 *
 * Always returns 5100 (MILESTONE 5100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5100(void)
{
	return b5100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5100(void)
    __attribute__((alias("gj_batch_id_5100")));

uint32_t __gj_graph_milestone_5100(void)
    __attribute__((alias("gj_graph_milestone_5100")));
