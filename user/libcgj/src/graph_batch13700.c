/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13700: MILESTONE 13700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13700(void);
 *     - Returns the compile-time graph batch number for this TU (13700).
 *   uint32_t gj_graph_milestone_13700(void);
 *     - Returns the current graph milestone revision (13700).
 *   uint32_t __gj_batch_id_13700  (alias)
 *   uint32_t __gj_graph_milestone_13700  (alias)
 *   __libcgj_batch13700_marker = "libcgj-batch13700"
 *
 * MILESTONE 13700 for the exclusive continuum CREATE-ONLY wave
 * (batches 13691-13699: shell_green_13700, libcgj_green_13700,
 * bar3_ready_13700, product_score_13700, continuum_ready_13700,
 * smoke_soft_13700, dyn_soft_13700, milestone_tag_13700,
 * continuum_wave_13700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13700_marker[] = "libcgj-batch13700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13700_id(void)
{
	return 13700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13700 - report this TU's graph batch number.
 *
 * Always returns 13700.
 */
uint32_t
gj_batch_id_13700(void)
{
	(void)NULL;
	return b13700_id();
}

/*
 * gj_graph_milestone_13700 - report this TU's graph milestone revision.
 *
 * Always returns 13700 (MILESTONE 13700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13700(void)
{
	return b13700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13700(void)
    __attribute__((alias("gj_batch_id_13700")));

uint32_t __gj_graph_milestone_13700(void)
    __attribute__((alias("gj_graph_milestone_13700")));
