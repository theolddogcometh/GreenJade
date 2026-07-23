/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2700: milestone 2700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2700(void);
 *     - Returns the compile-time graph batch number for this TU (2700).
 *   uint32_t gj_graph_milestone_2700(void);
 *     - Returns the current graph milestone revision (2700).
 *   uint32_t __gj_batch_id_2700  (alias)
 *   uint32_t __gj_graph_milestone_2700  (alias)
 *   __libcgj_batch2700_marker = "libcgj-batch2700"
 *
 * Milestone for the exclusive product helpers wave (batches 2691-2699:
 * wave_id_2700, export_hint_2700, smoke_green_pair_p, dyn_green_pair_p,
 * shell_wave_ok, libcgj_wave_ok, product_path_ok, continuum_ready_p,
 * bar3_score_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2700_marker[] = "libcgj-batch2700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2700_id(void)
{
	return 2700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2700 - report this TU's graph batch number.
 *
 * Always returns 2700.
 */
uint32_t
gj_batch_id_2700(void)
{
	(void)NULL;
	return b2700_id();
}

/*
 * gj_graph_milestone_2700 - report this TU's graph milestone revision.
 *
 * Always returns 2700 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2700(void)
{
	return b2700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2700(void)
    __attribute__((alias("gj_batch_id_2700")));

uint32_t __gj_graph_milestone_2700(void)
    __attribute__((alias("gj_graph_milestone_2700")));
