/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7500: MILESTONE 7500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7500(void);
 *     - Returns the compile-time graph batch number for this TU (7500).
 *   uint32_t gj_graph_milestone_7500(void);
 *     - Returns the current graph milestone revision (7500).
 *   uint32_t __gj_batch_id_7500  (alias)
 *   uint32_t __gj_graph_milestone_7500  (alias)
 *   __libcgj_batch7500_marker = "libcgj-batch7500"
 *
 * MILESTONE 7500 for the exclusive continuum CREATE-ONLY wave
 * (batches 7491-7499: wave_id_7500, export_hint_7500, smoke_green_7500,
 * dyn_green_7500, shell_green_7500, libcgj_green_7500, bar3_ready_7500,
 * product_score_7500, continuum_ready_7500). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7500_marker[] = "libcgj-batch7500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7500_id(void)
{
	return 7500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7500 - report this TU's graph batch number.
 *
 * Always returns 7500.
 */
uint32_t
gj_batch_id_7500(void)
{
	(void)NULL;
	return b7500_id();
}

/*
 * gj_graph_milestone_7500 - report this TU's graph milestone revision.
 *
 * Always returns 7500 (MILESTONE 7500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7500(void)
{
	return b7500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7500(void)
    __attribute__((alias("gj_batch_id_7500")));

uint32_t __gj_graph_milestone_7500(void)
    __attribute__((alias("gj_graph_milestone_7500")));
