/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6500: MILESTONE 6500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6500(void);
 *     - Returns the compile-time graph batch number for this TU (6500).
 *   uint32_t gj_graph_milestone_6500(void);
 *     - Returns the current graph milestone revision (6500).
 *   uint32_t __gj_batch_id_6500  (alias)
 *   uint32_t __gj_graph_milestone_6500  (alias)
 *   __libcgj_batch6500_marker = "libcgj-batch6500"
 *
 * MILESTONE 6500 for the exclusive continuum CREATE-ONLY wave
 * (batches 6491-6499: wave_id_6500, export_hint_6500, smoke_green_6500,
 * dyn_green_6500, shell_green_6500, libcgj_green_6500, bar3_ready_6500,
 * product_score_6500, continuum_ready_6500). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6500_marker[] = "libcgj-batch6500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6500_id(void)
{
	return 6500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6500 - report this TU's graph batch number.
 *
 * Always returns 6500.
 */
uint32_t
gj_batch_id_6500(void)
{
	(void)NULL;
	return b6500_id();
}

/*
 * gj_graph_milestone_6500 - report this TU's graph milestone revision.
 *
 * Always returns 6500 (MILESTONE 6500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6500(void)
{
	return b6500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6500(void)
    __attribute__((alias("gj_batch_id_6500")));

uint32_t __gj_graph_milestone_6500(void)
    __attribute__((alias("gj_graph_milestone_6500")));
