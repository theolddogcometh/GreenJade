/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3300: MILESTONE 3300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3300(void);
 *     - Returns the compile-time graph batch number for this TU (3300).
 *   uint32_t gj_graph_milestone_3300(void);
 *     - Returns the current graph milestone revision (3300).
 *   uint32_t __gj_batch_id_3300  (alias)
 *   uint32_t __gj_graph_milestone_3300  (alias)
 *   __libcgj_batch3300_marker = "libcgj-batch3300"
 *
 * MILESTONE 3300 for the exclusive continuum CREATE-ONLY wave
 * (batches 3291-3299: wave_id_3300, export_hint_3300, smoke_green_3300,
 * dyn_green_3300, shell_green_3300, libcgj_green_3300, bar3_ready_3300,
 * product_score_3300, continuum_ready_3300). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3300_marker[] = "libcgj-batch3300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3300_id(void)
{
	return 3300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3300 - report this TU's graph batch number.
 *
 * Always returns 3300.
 */
uint32_t
gj_batch_id_3300(void)
{
	(void)NULL;
	return b3300_id();
}

/*
 * gj_graph_milestone_3300 - report this TU's graph milestone revision.
 *
 * Always returns 3300 (MILESTONE 3300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3300(void)
{
	return b3300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3300(void)
    __attribute__((alias("gj_batch_id_3300")));

uint32_t __gj_graph_milestone_3300(void)
    __attribute__((alias("gj_graph_milestone_3300")));
