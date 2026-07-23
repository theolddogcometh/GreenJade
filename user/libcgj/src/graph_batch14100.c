/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14100: MILESTONE 14100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14100(void);
 *     - Returns the compile-time graph batch number for this TU (14100).
 *   uint32_t gj_graph_milestone_14100(void);
 *     - Returns the current graph milestone revision (14100).
 *   uint32_t __gj_batch_id_14100  (alias)
 *   uint32_t __gj_graph_milestone_14100  (alias)
 *   __libcgj_batch14100_marker = "libcgj-batch14100"
 *
 * MILESTONE 14100 for the exclusive continuum CREATE-ONLY wave
 * (batches 14091-14099: shell_green_14100, libcgj_green_14100,
 * bar3_ready_14100, product_score_14100, continuum_ready_14100,
 * smoke_soft_14100, dyn_soft_14100, milestone_tag_14100,
 * continuum_wave_14100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14100_marker[] = "libcgj-batch14100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14100_id(void)
{
	return 14100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14100 - report this TU's graph batch number.
 *
 * Always returns 14100.
 */
uint32_t
gj_batch_id_14100(void)
{
	(void)NULL;
	return b14100_id();
}

/*
 * gj_graph_milestone_14100 - report this TU's graph milestone revision.
 *
 * Always returns 14100 (MILESTONE 14100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14100(void)
{
	return b14100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14100(void)
    __attribute__((alias("gj_batch_id_14100")));

uint32_t __gj_graph_milestone_14100(void)
    __attribute__((alias("gj_graph_milestone_14100")));
