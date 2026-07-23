/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8700: MILESTONE 8700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8700(void);
 *     - Returns the compile-time graph batch number for this TU (8700).
 *   uint32_t gj_graph_milestone_8700(void);
 *     - Returns the current graph milestone revision (8700).
 *   uint32_t __gj_batch_id_8700  (alias)
 *   uint32_t __gj_graph_milestone_8700  (alias)
 *   __libcgj_batch8700_marker = "libcgj-batch8700"
 *
 * MILESTONE 8700 for the exclusive continuum CREATE-ONLY wave
 * (batches 8691-8699: shell_green_8700, libcgj_green_8700,
 * bar3_ready_8700, product_score_8700, continuum_ready_8700,
 * smoke_soft_8700, dyn_soft_8700, milestone_tag_8700,
 * continuum_wave_8700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8700_marker[] = "libcgj-batch8700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8700_id(void)
{
	return 8700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8700 - report this TU's graph batch number.
 *
 * Always returns 8700.
 */
uint32_t
gj_batch_id_8700(void)
{
	(void)NULL;
	return b8700_id();
}

/*
 * gj_graph_milestone_8700 - report this TU's graph milestone revision.
 *
 * Always returns 8700 (MILESTONE 8700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8700(void)
{
	return b8700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8700(void)
    __attribute__((alias("gj_batch_id_8700")));

uint32_t __gj_graph_milestone_8700(void)
    __attribute__((alias("gj_graph_milestone_8700")));
