/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3600: MILESTONE 3600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3600(void);
 *     - Returns the compile-time graph batch number for this TU (3600).
 *   uint32_t gj_graph_milestone_3600(void);
 *     - Returns the current graph milestone revision (3600).
 *   uint32_t __gj_batch_id_3600  (alias)
 *   uint32_t __gj_graph_milestone_3600  (alias)
 *   __libcgj_batch3600_marker = "libcgj-batch3600"
 *
 * MILESTONE 3600 for the exclusive continuum CREATE-ONLY wave
 * (batches 3591-3599: wave_id_3600, export_hint_3600, smoke_green_3600,
 * dyn_green_3600, shell_green_3600, libcgj_green_3600, bar3_ready_3600,
 * product_score_3600, continuum_ready_3600). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3600_marker[] = "libcgj-batch3600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3600_id(void)
{
	return 3600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3600 - report this TU's graph batch number.
 *
 * Always returns 3600.
 */
uint32_t
gj_batch_id_3600(void)
{
	(void)NULL;
	return b3600_id();
}

/*
 * gj_graph_milestone_3600 - report this TU's graph milestone revision.
 *
 * Always returns 3600 (MILESTONE 3600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3600(void)
{
	return b3600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3600(void)
    __attribute__((alias("gj_batch_id_3600")));

uint32_t __gj_graph_milestone_3600(void)
    __attribute__((alias("gj_graph_milestone_3600")));
