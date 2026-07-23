/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10200: MILESTONE 10200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10200(void);
 *     - Returns the compile-time graph batch number for this TU (10200).
 *   uint32_t gj_graph_milestone_10200(void);
 *     - Returns the current graph milestone revision (10200).
 *   uint32_t __gj_batch_id_10200  (alias)
 *   uint32_t __gj_graph_milestone_10200  (alias)
 *   __libcgj_batch10200_marker = "libcgj-batch10200"
 *
 * MILESTONE 10200 for the exclusive continuum CREATE-ONLY wave
 * (batches 10191-10199: shell_green_10200, libcgj_green_10200,
 * bar3_ready_10200, product_score_10200, continuum_ready_10200,
 * smoke_soft_10200, dyn_soft_10200, milestone_tag_10200,
 * continuum_wave_10200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10200_marker[] = "libcgj-batch10200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10200_id(void)
{
	return 10200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10200 - report this TU's graph batch number.
 *
 * Always returns 10200.
 */
uint32_t
gj_batch_id_10200(void)
{
	(void)NULL;
	return b10200_id();
}

/*
 * gj_graph_milestone_10200 - report this TU's graph milestone revision.
 *
 * Always returns 10200 (MILESTONE 10200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10200(void)
{
	return b10200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10200(void)
    __attribute__((alias("gj_batch_id_10200")));

uint32_t __gj_graph_milestone_10200(void)
    __attribute__((alias("gj_graph_milestone_10200")));
