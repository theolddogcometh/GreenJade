/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13100: MILESTONE 13100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13100(void);
 *     - Returns the compile-time graph batch number for this TU (13100).
 *   uint32_t gj_graph_milestone_13100(void);
 *     - Returns the current graph milestone revision (13100).
 *   uint32_t __gj_batch_id_13100  (alias)
 *   uint32_t __gj_graph_milestone_13100  (alias)
 *   __libcgj_batch13100_marker = "libcgj-batch13100"
 *
 * MILESTONE 13100 for the exclusive continuum CREATE-ONLY wave
 * (batches 13091-13099: shell_green_13100, libcgj_green_13100,
 * bar3_ready_13100, product_score_13100, continuum_ready_13100,
 * smoke_soft_13100, dyn_soft_13100, milestone_tag_13100,
 * continuum_wave_13100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13100_marker[] = "libcgj-batch13100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13100_id(void)
{
	return 13100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13100 - report this TU's graph batch number.
 *
 * Always returns 13100.
 */
uint32_t
gj_batch_id_13100(void)
{
	(void)NULL;
	return b13100_id();
}

/*
 * gj_graph_milestone_13100 - report this TU's graph milestone revision.
 *
 * Always returns 13100 (MILESTONE 13100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13100(void)
{
	return b13100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13100(void)
    __attribute__((alias("gj_batch_id_13100")));

uint32_t __gj_graph_milestone_13100(void)
    __attribute__((alias("gj_graph_milestone_13100")));
