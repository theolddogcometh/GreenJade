/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11500: MILESTONE 11500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11500(void);
 *     - Returns the compile-time graph batch number for this TU (11500).
 *   uint32_t gj_graph_milestone_11500(void);
 *     - Returns the current graph milestone revision (11500).
 *   uint32_t __gj_batch_id_11500  (alias)
 *   uint32_t __gj_graph_milestone_11500  (alias)
 *   __libcgj_batch11500_marker = "libcgj-batch11500"
 *
 * MILESTONE 11500 for the exclusive continuum CREATE-ONLY wave
 * (batches 11491-11499: shell_green_11500, libcgj_green_11500,
 * bar3_ready_11500, product_score_11500, continuum_ready_11500,
 * smoke_soft_11500, dyn_soft_11500, milestone_tag_11500,
 * continuum_wave_11500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11500_marker[] = "libcgj-batch11500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11500_id(void)
{
	return 11500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11500 - report this TU's graph batch number.
 *
 * Always returns 11500.
 */
uint32_t
gj_batch_id_11500(void)
{
	(void)NULL;
	return b11500_id();
}

/*
 * gj_graph_milestone_11500 - report this TU's graph milestone revision.
 *
 * Always returns 11500 (MILESTONE 11500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_11500(void)
{
	return b11500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_11500(void)
    __attribute__((alias("gj_batch_id_11500")));

uint32_t __gj_graph_milestone_11500(void)
    __attribute__((alias("gj_graph_milestone_11500")));
