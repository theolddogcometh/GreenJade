/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13800: MILESTONE 13800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13800(void);
 *     - Returns the compile-time graph batch number for this TU (13800).
 *   uint32_t gj_graph_milestone_13800(void);
 *     - Returns the current graph milestone revision (13800).
 *   uint32_t __gj_batch_id_13800  (alias)
 *   uint32_t __gj_graph_milestone_13800  (alias)
 *   __libcgj_batch13800_marker = "libcgj-batch13800"
 *
 * MILESTONE 13800 for the exclusive continuum CREATE-ONLY wave
 * (batches 13791-13799: shell_green_13800, libcgj_green_13800,
 * bar3_ready_13800, product_score_13800, continuum_ready_13800,
 * smoke_soft_13800, dyn_soft_13800, milestone_tag_13800,
 * continuum_wave_13800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13800_marker[] = "libcgj-batch13800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13800_id(void)
{
	return 13800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13800 - report this TU's graph batch number.
 *
 * Always returns 13800.
 */
uint32_t
gj_batch_id_13800(void)
{
	(void)NULL;
	return b13800_id();
}

/*
 * gj_graph_milestone_13800 - report this TU's graph milestone revision.
 *
 * Always returns 13800 (MILESTONE 13800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13800(void)
{
	return b13800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13800(void)
    __attribute__((alias("gj_batch_id_13800")));

uint32_t __gj_graph_milestone_13800(void)
    __attribute__((alias("gj_graph_milestone_13800")));
