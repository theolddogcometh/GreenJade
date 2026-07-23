/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3000: MILESTONE 3000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3000(void);
 *     - Returns the compile-time graph batch number for this TU (3000).
 *   uint32_t gj_graph_milestone_3000(void);
 *     - Returns the current graph milestone revision (3000).
 *   uint32_t __gj_batch_id_3000  (alias)
 *   uint32_t __gj_graph_milestone_3000  (alias)
 *   __libcgj_batch3000_marker = "libcgj-batch3000"
 *
 * MILESTONE 3000 for the exclusive continuum CREATE-ONLY wave
 * (batches 2991-2999: continuum_wave_id_3000, export_hint_3000,
 * smoke_gate_pair_u, dyn_gate_pair_u, shell_ready_u, libcgj_ready_u,
 * bar3_progress_u, product_score_u, milestone_prev_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3000_marker[] = "libcgj-batch3000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3000_id(void)
{
	return 3000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3000 - report this TU's graph batch number.
 *
 * Always returns 3000.
 */
uint32_t
gj_batch_id_3000(void)
{
	(void)NULL;
	return b3000_id();
}

/*
 * gj_graph_milestone_3000 - report this TU's graph milestone revision.
 *
 * Always returns 3000 (MILESTONE 3000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 * No parent wires.
 */
uint32_t
gj_graph_milestone_3000(void)
{
	return b3000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3000(void)
    __attribute__((alias("gj_batch_id_3000")));

uint32_t __gj_graph_milestone_3000(void)
    __attribute__((alias("gj_graph_milestone_3000")));
