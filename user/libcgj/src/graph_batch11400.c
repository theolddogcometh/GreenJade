/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11400: MILESTONE 11400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11400(void);
 *     - Returns the compile-time graph batch number for this TU (11400).
 *   uint32_t gj_graph_milestone_11400(void);
 *     - Returns the current graph milestone revision (11400).
 *   uint32_t __gj_batch_id_11400  (alias)
 *   uint32_t __gj_graph_milestone_11400  (alias)
 *   __libcgj_batch11400_marker = "libcgj-batch11400"
 *
 * MILESTONE 11400 for the exclusive continuum CREATE-ONLY wave
 * (batches 11391-11399: shell_green_11400, libcgj_green_11400,
 * bar3_ready_11400, product_score_11400, continuum_ready_11400,
 * smoke_soft_11400, dyn_soft_11400, milestone_tag_11400,
 * continuum_wave_11400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11400_marker[] = "libcgj-batch11400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11400_id(void)
{
	return 11400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11400 - report this TU's graph batch number.
 *
 * Always returns 11400.
 */
uint32_t
gj_batch_id_11400(void)
{
	(void)NULL;
	return b11400_id();
}

/*
 * gj_graph_milestone_11400 - report this TU's graph milestone revision.
 *
 * Always returns 11400 (MILESTONE 11400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_11400(void)
{
	return b11400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_11400(void)
    __attribute__((alias("gj_batch_id_11400")));

uint32_t __gj_graph_milestone_11400(void)
    __attribute__((alias("gj_graph_milestone_11400")));
