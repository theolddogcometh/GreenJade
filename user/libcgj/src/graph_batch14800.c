/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14800: MILESTONE 14800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14800(void);
 *     - Returns the compile-time graph batch number for this TU (14800).
 *   uint32_t gj_graph_milestone_14800(void);
 *     - Returns the current graph milestone revision (14800).
 *   uint32_t __gj_batch_id_14800  (alias)
 *   uint32_t __gj_graph_milestone_14800  (alias)
 *   __libcgj_batch14800_marker = "libcgj-batch14800"
 *
 * MILESTONE 14800 for the exclusive continuum CREATE-ONLY wave
 * (batches 14791-14799: shell_green_14800, libcgj_green_14800,
 * bar3_ready_14800, product_score_14800, continuum_ready_14800,
 * smoke_soft_14800, dyn_soft_14800, milestone_tag_14800,
 * continuum_wave_14800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14800_marker[] = "libcgj-batch14800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14800_id(void)
{
	return 14800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14800 - report this TU's graph batch number.
 *
 * Always returns 14800.
 */
uint32_t
gj_batch_id_14800(void)
{
	(void)NULL;
	return b14800_id();
}

/*
 * gj_graph_milestone_14800 - report this TU's graph milestone revision.
 *
 * Always returns 14800 (MILESTONE 14800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14800(void)
{
	return b14800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14800(void)
    __attribute__((alias("gj_batch_id_14800")));

uint32_t __gj_graph_milestone_14800(void)
    __attribute__((alias("gj_graph_milestone_14800")));
