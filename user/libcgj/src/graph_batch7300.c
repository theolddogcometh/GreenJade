/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7300: MILESTONE 7300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7300(void);
 *     - Returns the compile-time graph batch number for this TU (7300).
 *   uint32_t gj_graph_milestone_7300(void);
 *     - Returns the current graph milestone revision (7300).
 *   uint32_t __gj_batch_id_7300  (alias)
 *   uint32_t __gj_graph_milestone_7300  (alias)
 *   __libcgj_batch7300_marker = "libcgj-batch7300"
 *
 * MILESTONE 7300 for the exclusive continuum CREATE-ONLY wave
 * (batches 7291-7299: wave_id_7300, export_hint_7300, smoke_green_7300,
 * dyn_green_7300, shell_green_7300, libcgj_green_7300, bar3_ready_7300,
 * product_score_7300, continuum_ready_7300). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7300_marker[] = "libcgj-batch7300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7300_id(void)
{
	return 7300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7300 - report this TU's graph batch number.
 *
 * Always returns 7300.
 */
uint32_t
gj_batch_id_7300(void)
{
	(void)NULL;
	return b7300_id();
}

/*
 * gj_graph_milestone_7300 - report this TU's graph milestone revision.
 *
 * Always returns 7300 (MILESTONE 7300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7300(void)
{
	return b7300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7300(void)
    __attribute__((alias("gj_batch_id_7300")));

uint32_t __gj_graph_milestone_7300(void)
    __attribute__((alias("gj_graph_milestone_7300")));
