/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8800: MILESTONE 8800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8800(void);
 *     - Returns the compile-time graph batch number for this TU (8800).
 *   uint32_t gj_graph_milestone_8800(void);
 *     - Returns the current graph milestone revision (8800).
 *   uint32_t __gj_batch_id_8800  (alias)
 *   uint32_t __gj_graph_milestone_8800  (alias)
 *   __libcgj_batch8800_marker = "libcgj-batch8800"
 *
 * MILESTONE 8800 for the exclusive continuum CREATE-ONLY wave
 * (batches 8791-8799: shell_green_8800, libcgj_green_8800,
 * bar3_ready_8800, product_score_8800, continuum_ready_8800,
 * smoke_soft_8800, dyn_soft_8800, milestone_tag_8800,
 * continuum_wave_8800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8800_marker[] = "libcgj-batch8800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8800_id(void)
{
	return 8800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8800 - report this TU's graph batch number.
 *
 * Always returns 8800.
 */
uint32_t
gj_batch_id_8800(void)
{
	(void)NULL;
	return b8800_id();
}

/*
 * gj_graph_milestone_8800 - report this TU's graph milestone revision.
 *
 * Always returns 8800 (MILESTONE 8800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8800(void)
{
	return b8800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8800(void)
    __attribute__((alias("gj_batch_id_8800")));

uint32_t __gj_graph_milestone_8800(void)
    __attribute__((alias("gj_graph_milestone_8800")));
