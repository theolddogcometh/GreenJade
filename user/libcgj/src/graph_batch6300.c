/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6300: MILESTONE 6300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6300(void);
 *     - Returns the compile-time graph batch number for this TU (6300).
 *   uint32_t gj_graph_milestone_6300(void);
 *     - Returns the current graph milestone revision (6300).
 *   uint32_t __gj_batch_id_6300  (alias)
 *   uint32_t __gj_graph_milestone_6300  (alias)
 *   __libcgj_batch6300_marker = "libcgj-batch6300"
 *
 * MILESTONE 6300 for the exclusive continuum CREATE-ONLY wave
 * (batches 6291-6299: wave_id_6300, export_hint_6300, smoke_green_6300,
 * dyn_green_6300, shell_green_6300, libcgj_green_6300, bar3_ready_6300,
 * product_score_6300, continuum_ready_6300). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6300_marker[] = "libcgj-batch6300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6300_id(void)
{
	return 6300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6300 - report this TU's graph batch number.
 *
 * Always returns 6300.
 */
uint32_t
gj_batch_id_6300(void)
{
	(void)NULL;
	return b6300_id();
}

/*
 * gj_graph_milestone_6300 - report this TU's graph milestone revision.
 *
 * Always returns 6300 (MILESTONE 6300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6300(void)
{
	return b6300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6300(void)
    __attribute__((alias("gj_batch_id_6300")));

uint32_t __gj_graph_milestone_6300(void)
    __attribute__((alias("gj_graph_milestone_6300")));
