/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3400: MILESTONE 3400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3400(void);
 *     - Returns the compile-time graph batch number for this TU (3400).
 *   uint32_t gj_graph_milestone_3400(void);
 *     - Returns the current graph milestone revision (3400).
 *   uint32_t __gj_batch_id_3400  (alias)
 *   uint32_t __gj_graph_milestone_3400  (alias)
 *   __libcgj_batch3400_marker = "libcgj-batch3400"
 *
 * MILESTONE 3400 for the exclusive continuum CREATE-ONLY wave
 * (batches 3391-3399: wave_id_3400, export_hint_3400, smoke_green_3400,
 * dyn_green_3400, shell_green_3400, libcgj_green_3400, bar3_ready_3400,
 * product_score_3400, continuum_ready_3400). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3400_marker[] = "libcgj-batch3400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3400_id(void)
{
	return 3400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3400 - report this TU's graph batch number.
 *
 * Always returns 3400.
 */
uint32_t
gj_batch_id_3400(void)
{
	(void)NULL;
	return b3400_id();
}

/*
 * gj_graph_milestone_3400 - report this TU's graph milestone revision.
 *
 * Always returns 3400 (MILESTONE 3400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3400(void)
{
	return b3400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3400(void)
    __attribute__((alias("gj_batch_id_3400")));

uint32_t __gj_graph_milestone_3400(void)
    __attribute__((alias("gj_graph_milestone_3400")));
