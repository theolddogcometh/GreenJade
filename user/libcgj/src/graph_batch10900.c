/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10900: MILESTONE 10900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10900(void);
 *     - Returns the compile-time graph batch number for this TU (10900).
 *   uint32_t gj_graph_milestone_10900(void);
 *     - Returns the current graph milestone revision (10900).
 *   uint32_t __gj_batch_id_10900  (alias)
 *   uint32_t __gj_graph_milestone_10900  (alias)
 *   __libcgj_batch10900_marker = "libcgj-batch10900"
 *
 * MILESTONE 10900 for the exclusive continuum CREATE-ONLY wave
 * (batches 10891-10899: shell_green_10900, libcgj_green_10900,
 * bar3_ready_10900, product_score_10900, continuum_ready_10900,
 * smoke_soft_10900, dyn_soft_10900, milestone_tag_10900,
 * continuum_wave_10900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10900_marker[] = "libcgj-batch10900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10900_id(void)
{
	return 10900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10900 - report this TU's graph batch number.
 *
 * Always returns 10900.
 */
uint32_t
gj_batch_id_10900(void)
{
	(void)NULL;
	return b10900_id();
}

/*
 * gj_graph_milestone_10900 - report this TU's graph milestone revision.
 *
 * Always returns 10900 (MILESTONE 10900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10900(void)
{
	return b10900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10900(void)
    __attribute__((alias("gj_batch_id_10900")));

uint32_t __gj_graph_milestone_10900(void)
    __attribute__((alias("gj_graph_milestone_10900")));
