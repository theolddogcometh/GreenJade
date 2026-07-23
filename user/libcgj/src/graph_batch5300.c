/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5300: MILESTONE 5300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5300(void);
 *     - Returns the compile-time graph batch number for this TU (5300).
 *   uint32_t gj_graph_milestone_5300(void);
 *     - Returns the current graph milestone revision (5300).
 *   uint32_t __gj_batch_id_5300  (alias)
 *   uint32_t __gj_graph_milestone_5300  (alias)
 *   __libcgj_batch5300_marker = "libcgj-batch5300"
 *
 * MILESTONE 5300 for the exclusive continuum CREATE-ONLY wave
 * (batches 5291-5299: wave_id_5300, export_hint_5300, smoke_green_5300,
 * dyn_green_5300, shell_green_5300, libcgj_green_5300, bar3_ready_5300,
 * product_score_5300, continuum_ready_5300). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5300_marker[] = "libcgj-batch5300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5300_id(void)
{
	return 5300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5300 - report this TU's graph batch number.
 *
 * Always returns 5300.
 */
uint32_t
gj_batch_id_5300(void)
{
	(void)NULL;
	return b5300_id();
}

/*
 * gj_graph_milestone_5300 - report this TU's graph milestone revision.
 *
 * Always returns 5300 (MILESTONE 5300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5300(void)
{
	return b5300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5300(void)
    __attribute__((alias("gj_batch_id_5300")));

uint32_t __gj_graph_milestone_5300(void)
    __attribute__((alias("gj_graph_milestone_5300")));
