/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4950: MILESTONE 4950 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4950(void);
 *     - Returns the compile-time graph batch number for this TU (4950).
 *   uint32_t gj_graph_milestone_4950(void);
 *     - Returns the current graph milestone revision (4950).
 *   uint32_t __gj_batch_id_4950  (alias)
 *   uint32_t __gj_graph_milestone_4950  (alias)
 *   __libcgj_batch4950_marker = "libcgj-batch4950"
 *
 * MILESTONE 4950 for the exclusive continuum CREATE-ONLY wave
 * (batches 4941-4949: wave_id_4950, export_hint_4950, smoke_green_4950,
 * dyn_green_4950, shell_green_4950, libcgj_green_4950, bar3_ready_4950,
 * product_score_4950, continuum_ready_4950). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4950_marker[] = "libcgj-batch4950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4950_id(void)
{
	return 4950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4950 - report this TU's graph batch number.
 *
 * Always returns 4950.
 */
uint32_t
gj_batch_id_4950(void)
{
	(void)NULL;
	return b4950_id();
}

/*
 * gj_graph_milestone_4950 - report this TU's graph milestone revision.
 *
 * Always returns 4950 (MILESTONE 4950). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_4950(void)
{
	return b4950_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_4950(void)
    __attribute__((alias("gj_batch_id_4950")));

uint32_t __gj_graph_milestone_4950(void)
    __attribute__((alias("gj_graph_milestone_4950")));
