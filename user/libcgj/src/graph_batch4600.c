/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4600: MILESTONE 4600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4600(void);
 *     - Returns the compile-time graph batch number for this TU (4600).
 *   uint32_t gj_graph_milestone_4600(void);
 *     - Returns the current graph milestone revision (4600).
 *   uint32_t __gj_batch_id_4600  (alias)
 *   uint32_t __gj_graph_milestone_4600  (alias)
 *   __libcgj_batch4600_marker = "libcgj-batch4600"
 *
 * MILESTONE 4600 for the exclusive continuum CREATE-ONLY wave
 * (batches 4591-4599: wave_id_4600, export_hint_4600, smoke_green_4600,
 * dyn_green_4600, shell_green_4600, libcgj_green_4600, bar3_ready_4600,
 * product_score_4600, continuum_ready_4600). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4600_marker[] = "libcgj-batch4600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4600_id(void)
{
	return 4600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4600 - report this TU's graph batch number.
 *
 * Always returns 4600.
 */
uint32_t
gj_batch_id_4600(void)
{
	(void)NULL;
	return b4600_id();
}

/*
 * gj_graph_milestone_4600 - report this TU's graph milestone revision.
 *
 * Always returns 4600 (MILESTONE 4600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4600(void)
{
	return b4600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4600(void)
    __attribute__((alias("gj_batch_id_4600")));

uint32_t __gj_graph_milestone_4600(void)
    __attribute__((alias("gj_graph_milestone_4600")));
