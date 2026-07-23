/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12200: MILESTONE 12200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12200(void);
 *     - Returns the compile-time graph batch number for this TU (12200).
 *   uint32_t gj_graph_milestone_12200(void);
 *     - Returns the current graph milestone revision (12200).
 *   uint32_t __gj_batch_id_12200  (alias)
 *   uint32_t __gj_graph_milestone_12200  (alias)
 *   __libcgj_batch12200_marker = "libcgj-batch12200"
 *
 * MILESTONE 12200 for the exclusive continuum CREATE-ONLY wave
 * (batches 12191-12199: shell_green_12200, libcgj_green_12200,
 * bar3_ready_12200, product_score_12200, continuum_ready_12200,
 * smoke_soft_12200, dyn_soft_12200, milestone_tag_12200,
 * continuum_wave_12200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12200_marker[] = "libcgj-batch12200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12200_id(void)
{
	return 12200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12200 - report this TU's graph batch number.
 *
 * Always returns 12200.
 */
uint32_t
gj_batch_id_12200(void)
{
	(void)NULL;
	return b12200_id();
}

/*
 * gj_graph_milestone_12200 - report this TU's graph milestone revision.
 *
 * Always returns 12200 (MILESTONE 12200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12200(void)
{
	return b12200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12200(void)
    __attribute__((alias("gj_batch_id_12200")));

uint32_t __gj_graph_milestone_12200(void)
    __attribute__((alias("gj_graph_milestone_12200")));
