/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4300: MILESTONE 4300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4300(void);
 *     - Returns the compile-time graph batch number for this TU (4300).
 *   uint32_t gj_graph_milestone_4300(void);
 *     - Returns the current graph milestone revision (4300).
 *   uint32_t __gj_batch_id_4300  (alias)
 *   uint32_t __gj_graph_milestone_4300  (alias)
 *   __libcgj_batch4300_marker = "libcgj-batch4300"
 *
 * MILESTONE 4300 for the exclusive continuum CREATE-ONLY wave
 * (batches 4291-4299: wave_id_4300, export_hint_4300, smoke_green_4300,
 * dyn_green_4300, shell_green_4300, libcgj_green_4300, bar3_ready_4300,
 * product_score_4300, continuum_ready_4300). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4300_marker[] = "libcgj-batch4300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4300_id(void)
{
	return 4300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4300 - report this TU's graph batch number.
 *
 * Always returns 4300.
 */
uint32_t
gj_batch_id_4300(void)
{
	(void)NULL;
	return b4300_id();
}

/*
 * gj_graph_milestone_4300 - report this TU's graph milestone revision.
 *
 * Always returns 4300 (MILESTONE 4300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4300(void)
{
	return b4300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4300(void)
    __attribute__((alias("gj_batch_id_4300")));

uint32_t __gj_graph_milestone_4300(void)
    __attribute__((alias("gj_graph_milestone_4300")));
