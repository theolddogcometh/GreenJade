/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13200: MILESTONE 13200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13200(void);
 *     - Returns the compile-time graph batch number for this TU (13200).
 *   uint32_t gj_graph_milestone_13200(void);
 *     - Returns the current graph milestone revision (13200).
 *   uint32_t __gj_batch_id_13200  (alias)
 *   uint32_t __gj_graph_milestone_13200  (alias)
 *   __libcgj_batch13200_marker = "libcgj-batch13200"
 *
 * MILESTONE 13200 for the exclusive continuum CREATE-ONLY wave
 * (batches 13191-13199: shell_green_13200, libcgj_green_13200,
 * bar3_ready_13200, product_score_13200, continuum_ready_13200,
 * smoke_soft_13200, dyn_soft_13200, milestone_tag_13200,
 * continuum_wave_13200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13200_marker[] = "libcgj-batch13200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13200_id(void)
{
	return 13200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13200 - report this TU's graph batch number.
 *
 * Always returns 13200.
 */
uint32_t
gj_batch_id_13200(void)
{
	(void)NULL;
	return b13200_id();
}

/*
 * gj_graph_milestone_13200 - report this TU's graph milestone revision.
 *
 * Always returns 13200 (MILESTONE 13200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13200(void)
{
	return b13200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13200(void)
    __attribute__((alias("gj_batch_id_13200")));

uint32_t __gj_graph_milestone_13200(void)
    __attribute__((alias("gj_graph_milestone_13200")));
