/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9400: MILESTONE 9400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9400(void);
 *     - Returns the compile-time graph batch number for this TU (9400).
 *   uint32_t gj_graph_milestone_9400(void);
 *     - Returns the current graph milestone revision (9400).
 *   uint32_t __gj_batch_id_9400  (alias)
 *   uint32_t __gj_graph_milestone_9400  (alias)
 *   __libcgj_batch9400_marker = "libcgj-batch9400"
 *
 * MILESTONE 9400 for the exclusive continuum CREATE-ONLY wave
 * (batches 9391-9399: shell_green_9400, libcgj_green_9400,
 * bar3_ready_9400, product_score_9400, continuum_ready_9400,
 * smoke_soft_9400, dyn_soft_9400, milestone_tag_9400,
 * continuum_wave_9400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9400_marker[] = "libcgj-batch9400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9400_id(void)
{
	return 9400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9400 - report this TU's graph batch number.
 *
 * Always returns 9400.
 */
uint32_t
gj_batch_id_9400(void)
{
	(void)NULL;
	return b9400_id();
}

/*
 * gj_graph_milestone_9400 - report this TU's graph milestone revision.
 *
 * Always returns 9400 (MILESTONE 9400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9400(void)
{
	return b9400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9400(void)
    __attribute__((alias("gj_batch_id_9400")));

uint32_t __gj_graph_milestone_9400(void)
    __attribute__((alias("gj_graph_milestone_9400")));
