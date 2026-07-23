/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12900: MILESTONE 12900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12900(void);
 *     - Returns the compile-time graph batch number for this TU (12900).
 *   uint32_t gj_graph_milestone_12900(void);
 *     - Returns the current graph milestone revision (12900).
 *   uint32_t __gj_batch_id_12900  (alias)
 *   uint32_t __gj_graph_milestone_12900  (alias)
 *   __libcgj_batch12900_marker = "libcgj-batch12900"
 *
 * MILESTONE 12900 for the exclusive continuum CREATE-ONLY wave
 * (batches 12891-12899: shell_green_12900, libcgj_green_12900,
 * bar3_ready_12900, product_score_12900, continuum_ready_12900,
 * smoke_soft_12900, dyn_soft_12900, milestone_tag_12900,
 * continuum_wave_12900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12900_marker[] = "libcgj-batch12900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12900_id(void)
{
	return 12900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12900 - report this TU's graph batch number.
 *
 * Always returns 12900.
 */
uint32_t
gj_batch_id_12900(void)
{
	(void)NULL;
	return b12900_id();
}

/*
 * gj_graph_milestone_12900 - report this TU's graph milestone revision.
 *
 * Always returns 12900 (MILESTONE 12900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12900(void)
{
	return b12900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12900(void)
    __attribute__((alias("gj_batch_id_12900")));

uint32_t __gj_graph_milestone_12900(void)
    __attribute__((alias("gj_graph_milestone_12900")));
