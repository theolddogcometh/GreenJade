/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9800: MILESTONE 9800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9800(void);
 *     - Returns the compile-time graph batch number for this TU (9800).
 *   uint32_t gj_graph_milestone_9800(void);
 *     - Returns the current graph milestone revision (9800).
 *   uint32_t __gj_batch_id_9800  (alias)
 *   uint32_t __gj_graph_milestone_9800  (alias)
 *   __libcgj_batch9800_marker = "libcgj-batch9800"
 *
 * MILESTONE 9800 for the exclusive continuum CREATE-ONLY wave
 * (batches 9791-9799: shell_green_9800, libcgj_green_9800,
 * bar3_ready_9800, product_score_9800, continuum_ready_9800,
 * smoke_soft_9800, dyn_soft_9800, milestone_tag_9800,
 * continuum_wave_9800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9800_marker[] = "libcgj-batch9800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9800_id(void)
{
	return 9800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9800 - report this TU's graph batch number.
 *
 * Always returns 9800.
 */
uint32_t
gj_batch_id_9800(void)
{
	(void)NULL;
	return b9800_id();
}

/*
 * gj_graph_milestone_9800 - report this TU's graph milestone revision.
 *
 * Always returns 9800 (MILESTONE 9800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9800(void)
{
	return b9800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9800(void)
    __attribute__((alias("gj_batch_id_9800")));

uint32_t __gj_graph_milestone_9800(void)
    __attribute__((alias("gj_graph_milestone_9800")));
