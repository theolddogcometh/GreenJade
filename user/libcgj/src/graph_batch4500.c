/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4500: MILESTONE 4500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4500(void);
 *     - Returns the compile-time graph batch number for this TU (4500).
 *   uint32_t gj_graph_milestone_4500(void);
 *     - Returns the current graph milestone revision (4500).
 *   uint32_t __gj_batch_id_4500  (alias)
 *   uint32_t __gj_graph_milestone_4500  (alias)
 *   __libcgj_batch4500_marker = "libcgj-batch4500"
 *
 * MILESTONE 4500 for the exclusive continuum CREATE-ONLY wave
 * (batches 4491-4499: wave_id_4500, export_hint_4500, smoke_green_4500,
 * dyn_green_4500, shell_green_4500, libcgj_green_4500, bar3_ready_4500,
 * product_score_4500, continuum_ready_4500). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4500_marker[] = "libcgj-batch4500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4500_id(void)
{
	return 4500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4500 - report this TU's graph batch number.
 *
 * Always returns 4500.
 */
uint32_t
gj_batch_id_4500(void)
{
	(void)NULL;
	return b4500_id();
}

/*
 * gj_graph_milestone_4500 - report this TU's graph milestone revision.
 *
 * Always returns 4500 (MILESTONE 4500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4500(void)
{
	return b4500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4500(void)
    __attribute__((alias("gj_batch_id_4500")));

uint32_t __gj_graph_milestone_4500(void)
    __attribute__((alias("gj_graph_milestone_4500")));
