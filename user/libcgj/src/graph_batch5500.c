/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5500: MILESTONE 5500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5500(void);
 *     - Returns the compile-time graph batch number for this TU (5500).
 *   uint32_t gj_graph_milestone_5500(void);
 *     - Returns the current graph milestone revision (5500).
 *   uint32_t __gj_batch_id_5500  (alias)
 *   uint32_t __gj_graph_milestone_5500  (alias)
 *   __libcgj_batch5500_marker = "libcgj-batch5500"
 *
 * MILESTONE 5500 for the exclusive continuum CREATE-ONLY wave
 * (batches 5491-5499: wave_id_5500, export_hint_5500, smoke_green_5500,
 * dyn_green_5500, shell_green_5500, libcgj_green_5500, bar3_ready_5500,
 * product_score_5500, continuum_ready_5500). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5500_marker[] = "libcgj-batch5500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5500_id(void)
{
	return 5500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5500 - report this TU's graph batch number.
 *
 * Always returns 5500.
 */
uint32_t
gj_batch_id_5500(void)
{
	(void)NULL;
	return b5500_id();
}

/*
 * gj_graph_milestone_5500 - report this TU's graph milestone revision.
 *
 * Always returns 5500 (MILESTONE 5500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5500(void)
{
	return b5500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5500(void)
    __attribute__((alias("gj_batch_id_5500")));

uint32_t __gj_graph_milestone_5500(void)
    __attribute__((alias("gj_graph_milestone_5500")));
