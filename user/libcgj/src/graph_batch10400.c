/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10400: MILESTONE 10400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10400(void);
 *     - Returns the compile-time graph batch number for this TU (10400).
 *   uint32_t gj_graph_milestone_10400(void);
 *     - Returns the current graph milestone revision (10400).
 *   uint32_t __gj_batch_id_10400  (alias)
 *   uint32_t __gj_graph_milestone_10400  (alias)
 *   __libcgj_batch10400_marker = "libcgj-batch10400"
 *
 * MILESTONE 10400 for the exclusive continuum CREATE-ONLY wave
 * (batches 10391-10399: shell_green_10400, libcgj_green_10400,
 * bar3_ready_10400, product_score_10400, continuum_ready_10400,
 * smoke_soft_10400, dyn_soft_10400, milestone_tag_10400,
 * continuum_wave_10400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10400_marker[] = "libcgj-batch10400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10400_id(void)
{
	return 10400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10400 - report this TU's graph batch number.
 *
 * Always returns 10400.
 */
uint32_t
gj_batch_id_10400(void)
{
	(void)NULL;
	return b10400_id();
}

/*
 * gj_graph_milestone_10400 - report this TU's graph milestone revision.
 *
 * Always returns 10400 (MILESTONE 10400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10400(void)
{
	return b10400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10400(void)
    __attribute__((alias("gj_batch_id_10400")));

uint32_t __gj_graph_milestone_10400(void)
    __attribute__((alias("gj_graph_milestone_10400")));
