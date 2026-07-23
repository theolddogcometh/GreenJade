/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13500: MILESTONE 13500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13500(void);
 *     - Returns the compile-time graph batch number for this TU (13500).
 *   uint32_t gj_graph_milestone_13500(void);
 *     - Returns the current graph milestone revision (13500).
 *   uint32_t __gj_batch_id_13500  (alias)
 *   uint32_t __gj_graph_milestone_13500  (alias)
 *   __libcgj_batch13500_marker = "libcgj-batch13500"
 *
 * MILESTONE 13500 for the exclusive continuum CREATE-ONLY wave
 * (batches 13491-13499: shell_green_13500, libcgj_green_13500,
 * bar3_ready_13500, product_score_13500, continuum_ready_13500,
 * smoke_soft_13500, dyn_soft_13500, milestone_tag_13500,
 * continuum_wave_13500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13500_marker[] = "libcgj-batch13500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13500_id(void)
{
	return 13500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13500 - report this TU's graph batch number.
 *
 * Always returns 13500.
 */
uint32_t
gj_batch_id_13500(void)
{
	(void)NULL;
	return b13500_id();
}

/*
 * gj_graph_milestone_13500 - report this TU's graph milestone revision.
 *
 * Always returns 13500 (MILESTONE 13500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13500(void)
{
	return b13500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13500(void)
    __attribute__((alias("gj_batch_id_13500")));

uint32_t __gj_graph_milestone_13500(void)
    __attribute__((alias("gj_graph_milestone_13500")));
