/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9200: MILESTONE 9200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9200(void);
 *     - Returns the compile-time graph batch number for this TU (9200).
 *   uint32_t gj_graph_milestone_9200(void);
 *     - Returns the current graph milestone revision (9200).
 *   uint32_t __gj_batch_id_9200  (alias)
 *   uint32_t __gj_graph_milestone_9200  (alias)
 *   __libcgj_batch9200_marker = "libcgj-batch9200"
 *
 * MILESTONE 9200 for the exclusive continuum CREATE-ONLY wave
 * (batches 9191-9199: shell_green_9200, libcgj_green_9200,
 * bar3_ready_9200, product_score_9200, continuum_ready_9200,
 * smoke_soft_9200, dyn_soft_9200, milestone_tag_9200,
 * continuum_wave_9200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9200_marker[] = "libcgj-batch9200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9200_id(void)
{
	return 9200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9200 - report this TU's graph batch number.
 *
 * Always returns 9200.
 */
uint32_t
gj_batch_id_9200(void)
{
	(void)NULL;
	return b9200_id();
}

/*
 * gj_graph_milestone_9200 - report this TU's graph milestone revision.
 *
 * Always returns 9200 (MILESTONE 9200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9200(void)
{
	return b9200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9200(void)
    __attribute__((alias("gj_batch_id_9200")));

uint32_t __gj_graph_milestone_9200(void)
    __attribute__((alias("gj_graph_milestone_9200")));
