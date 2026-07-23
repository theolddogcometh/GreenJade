/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3500: MILESTONE 3500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3500(void);
 *     - Returns the compile-time graph batch number for this TU (3500).
 *   uint32_t gj_graph_milestone_3500(void);
 *     - Returns the current graph milestone revision (3500).
 *   uint32_t __gj_batch_id_3500  (alias)
 *   uint32_t __gj_graph_milestone_3500  (alias)
 *   __libcgj_batch3500_marker = "libcgj-batch3500"
 *
 * MILESTONE 3500 for the exclusive continuum CREATE-ONLY wave
 * (batches 3491-3499: wave_id_3500, export_hint_3500, smoke_green_3500,
 * dyn_green_3500, shell_green_3500, libcgj_green_3500, bar3_ready_3500,
 * product_score_3500, continuum_ready_3500). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3500_marker[] = "libcgj-batch3500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3500_id(void)
{
	return 3500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3500 - report this TU's graph batch number.
 *
 * Always returns 3500.
 */
uint32_t
gj_batch_id_3500(void)
{
	(void)NULL;
	return b3500_id();
}

/*
 * gj_graph_milestone_3500 - report this TU's graph milestone revision.
 *
 * Always returns 3500 (MILESTONE 3500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3500(void)
{
	return b3500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3500(void)
    __attribute__((alias("gj_batch_id_3500")));

uint32_t __gj_graph_milestone_3500(void)
    __attribute__((alias("gj_graph_milestone_3500")));
