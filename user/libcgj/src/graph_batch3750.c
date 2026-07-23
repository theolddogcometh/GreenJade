/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3750: MILESTONE 3750 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3750(void);
 *     - Returns the compile-time graph batch number for this TU (3750).
 *   uint32_t gj_graph_milestone_3750(void);
 *     - Returns the current graph milestone revision (3750).
 *   uint32_t __gj_batch_id_3750  (alias)
 *   uint32_t __gj_graph_milestone_3750  (alias)
 *   __libcgj_batch3750_marker = "libcgj-batch3750"
 *
 * MILESTONE 3750 for the exclusive continuum CREATE-ONLY wave
 * (batches 3741-3749: wave_id_3750, export_hint_3750, smoke_green_3750,
 * dyn_green_3750, shell_green_3750, libcgj_green_3750, bar3_ready_3750,
 * product_score_3750, continuum_ready_3750). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3750_marker[] = "libcgj-batch3750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3750_id(void)
{
	return 3750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3750 - report this TU's graph batch number.
 *
 * Always returns 3750.
 */
uint32_t
gj_batch_id_3750(void)
{
	(void)NULL;
	return b3750_id();
}

/*
 * gj_graph_milestone_3750 - report this TU's graph milestone revision.
 *
 * Always returns 3750 (MILESTONE 3750). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3750(void)
{
	return b3750_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3750(void)
    __attribute__((alias("gj_batch_id_3750")));

uint32_t __gj_graph_milestone_3750(void)
    __attribute__((alias("gj_graph_milestone_3750")));
