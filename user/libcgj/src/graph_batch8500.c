/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8500: MILESTONE 8500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8500(void);
 *     - Returns the compile-time graph batch number for this TU (8500).
 *   uint32_t gj_graph_milestone_8500(void);
 *     - Returns the current graph milestone revision (8500).
 *   uint32_t __gj_batch_id_8500  (alias)
 *   uint32_t __gj_graph_milestone_8500  (alias)
 *   __libcgj_batch8500_marker = "libcgj-batch8500"
 *
 * MILESTONE 8500 for the exclusive continuum CREATE-ONLY wave
 * (batches 8491-8499: shell_green_8500, libcgj_green_8500,
 * bar3_ready_8500, product_score_8500, continuum_ready_8500,
 * smoke_soft_8500, dyn_soft_8500, milestone_tag_8500,
 * continuum_wave_8500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8500_marker[] = "libcgj-batch8500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8500_id(void)
{
	return 8500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8500 - report this TU's graph batch number.
 *
 * Always returns 8500.
 */
uint32_t
gj_batch_id_8500(void)
{
	(void)NULL;
	return b8500_id();
}

/*
 * gj_graph_milestone_8500 - report this TU's graph milestone revision.
 *
 * Always returns 8500 (MILESTONE 8500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8500(void)
{
	return b8500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8500(void)
    __attribute__((alias("gj_batch_id_8500")));

uint32_t __gj_graph_milestone_8500(void)
    __attribute__((alias("gj_graph_milestone_8500")));
