/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11900: MILESTONE 11900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11900(void);
 *     - Returns the compile-time graph batch number for this TU (11900).
 *   uint32_t gj_graph_milestone_11900(void);
 *     - Returns the current graph milestone revision (11900).
 *   uint32_t __gj_batch_id_11900  (alias)
 *   uint32_t __gj_graph_milestone_11900  (alias)
 *   __libcgj_batch11900_marker = "libcgj-batch11900"
 *
 * MILESTONE 11900 for the exclusive continuum CREATE-ONLY wave
 * (batches 11891-11899: shell_green_11900, libcgj_green_11900,
 * bar3_ready_11900, product_score_11900, continuum_ready_11900,
 * smoke_soft_11900, dyn_soft_11900, milestone_tag_11900,
 * continuum_wave_11900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11900_marker[] = "libcgj-batch11900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11900_id(void)
{
	return 11900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11900 - report this TU's graph batch number.
 *
 * Always returns 11900.
 */
uint32_t
gj_batch_id_11900(void)
{
	(void)NULL;
	return b11900_id();
}

/*
 * gj_graph_milestone_11900 - report this TU's graph milestone revision.
 *
 * Always returns 11900 (MILESTONE 11900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_11900(void)
{
	return b11900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_11900(void)
    __attribute__((alias("gj_batch_id_11900")));

uint32_t __gj_graph_milestone_11900(void)
    __attribute__((alias("gj_graph_milestone_11900")));
