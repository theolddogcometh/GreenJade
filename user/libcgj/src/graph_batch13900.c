/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13900: MILESTONE 13900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13900(void);
 *     - Returns the compile-time graph batch number for this TU (13900).
 *   uint32_t gj_graph_milestone_13900(void);
 *     - Returns the current graph milestone revision (13900).
 *   uint32_t __gj_batch_id_13900  (alias)
 *   uint32_t __gj_graph_milestone_13900  (alias)
 *   __libcgj_batch13900_marker = "libcgj-batch13900"
 *
 * MILESTONE 13900 for the exclusive continuum CREATE-ONLY wave
 * (batches 13891-13899: shell_green_13900, libcgj_green_13900,
 * bar3_ready_13900, product_score_13900, continuum_ready_13900,
 * smoke_soft_13900, dyn_soft_13900, milestone_tag_13900,
 * continuum_wave_13900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13900_marker[] = "libcgj-batch13900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13900_id(void)
{
	return 13900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13900 - report this TU's graph batch number.
 *
 * Always returns 13900.
 */
uint32_t
gj_batch_id_13900(void)
{
	(void)NULL;
	return b13900_id();
}

/*
 * gj_graph_milestone_13900 - report this TU's graph milestone revision.
 *
 * Always returns 13900 (MILESTONE 13900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13900(void)
{
	return b13900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13900(void)
    __attribute__((alias("gj_batch_id_13900")));

uint32_t __gj_graph_milestone_13900(void)
    __attribute__((alias("gj_graph_milestone_13900")));
