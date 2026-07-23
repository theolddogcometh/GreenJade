/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8900: MILESTONE 8900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8900(void);
 *     - Returns the compile-time graph batch number for this TU (8900).
 *   uint32_t gj_graph_milestone_8900(void);
 *     - Returns the current graph milestone revision (8900).
 *   uint32_t __gj_batch_id_8900  (alias)
 *   uint32_t __gj_graph_milestone_8900  (alias)
 *   __libcgj_batch8900_marker = "libcgj-batch8900"
 *
 * MILESTONE 8900 for the exclusive continuum CREATE-ONLY wave
 * (batches 8891-8899: shell_green_8900, libcgj_green_8900,
 * bar3_ready_8900, product_score_8900, continuum_ready_8900,
 * smoke_soft_8900, dyn_soft_8900, milestone_tag_8900,
 * continuum_wave_8900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8900_marker[] = "libcgj-batch8900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8900_id(void)
{
	return 8900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8900 - report this TU's graph batch number.
 *
 * Always returns 8900.
 */
uint32_t
gj_batch_id_8900(void)
{
	(void)NULL;
	return b8900_id();
}

/*
 * gj_graph_milestone_8900 - report this TU's graph milestone revision.
 *
 * Always returns 8900 (MILESTONE 8900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8900(void)
{
	return b8900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8900(void)
    __attribute__((alias("gj_batch_id_8900")));

uint32_t __gj_graph_milestone_8900(void)
    __attribute__((alias("gj_graph_milestone_8900")));
