/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14200: MILESTONE 14200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14200(void);
 *     - Returns the compile-time graph batch number for this TU (14200).
 *   uint32_t gj_graph_milestone_14200(void);
 *     - Returns the current graph milestone revision (14200).
 *   uint32_t __gj_batch_id_14200  (alias)
 *   uint32_t __gj_graph_milestone_14200  (alias)
 *   __libcgj_batch14200_marker = "libcgj-batch14200"
 *
 * MILESTONE 14200 for the exclusive continuum CREATE-ONLY wave
 * (batches 14191-14199: shell_green_14200, libcgj_green_14200,
 * bar3_ready_14200, product_score_14200, continuum_ready_14200,
 * smoke_soft_14200, dyn_soft_14200, milestone_tag_14200,
 * continuum_wave_14200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14200_marker[] = "libcgj-batch14200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14200_id(void)
{
	return 14200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14200 - report this TU's graph batch number.
 *
 * Always returns 14200.
 */
uint32_t
gj_batch_id_14200(void)
{
	(void)NULL;
	return b14200_id();
}

/*
 * gj_graph_milestone_14200 - report this TU's graph milestone revision.
 *
 * Always returns 14200 (MILESTONE 14200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14200(void)
{
	return b14200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14200(void)
    __attribute__((alias("gj_batch_id_14200")));

uint32_t __gj_graph_milestone_14200(void)
    __attribute__((alias("gj_graph_milestone_14200")));
