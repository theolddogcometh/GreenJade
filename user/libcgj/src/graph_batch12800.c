/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12800: MILESTONE 12800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12800(void);
 *     - Returns the compile-time graph batch number for this TU (12800).
 *   uint32_t gj_graph_milestone_12800(void);
 *     - Returns the current graph milestone revision (12800).
 *   uint32_t __gj_batch_id_12800  (alias)
 *   uint32_t __gj_graph_milestone_12800  (alias)
 *   __libcgj_batch12800_marker = "libcgj-batch12800"
 *
 * MILESTONE 12800 for the exclusive continuum CREATE-ONLY wave
 * (batches 12791-12799: shell_green_12800, libcgj_green_12800,
 * bar3_ready_12800, product_score_12800, continuum_ready_12800,
 * smoke_soft_12800, dyn_soft_12800, milestone_tag_12800,
 * continuum_wave_12800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12800_marker[] = "libcgj-batch12800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12800_id(void)
{
	return 12800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12800 - report this TU's graph batch number.
 *
 * Always returns 12800.
 */
uint32_t
gj_batch_id_12800(void)
{
	(void)NULL;
	return b12800_id();
}

/*
 * gj_graph_milestone_12800 - report this TU's graph milestone revision.
 *
 * Always returns 12800 (MILESTONE 12800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12800(void)
{
	return b12800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12800(void)
    __attribute__((alias("gj_batch_id_12800")));

uint32_t __gj_graph_milestone_12800(void)
    __attribute__((alias("gj_graph_milestone_12800")));
