/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9500: MILESTONE 9500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9500(void);
 *     - Returns the compile-time graph batch number for this TU (9500).
 *   uint32_t gj_graph_milestone_9500(void);
 *     - Returns the current graph milestone revision (9500).
 *   uint32_t __gj_batch_id_9500  (alias)
 *   uint32_t __gj_graph_milestone_9500  (alias)
 *   __libcgj_batch9500_marker = "libcgj-batch9500"
 *
 * MILESTONE 9500 for the exclusive continuum CREATE-ONLY wave
 * (batches 9491-9499: shell_green_9500, libcgj_green_9500,
 * bar3_ready_9500, product_score_9500, continuum_ready_9500,
 * smoke_soft_9500, dyn_soft_9500, milestone_tag_9500,
 * continuum_wave_9500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9500_marker[] = "libcgj-batch9500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9500_id(void)
{
	return 9500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9500 - report this TU's graph batch number.
 *
 * Always returns 9500.
 */
uint32_t
gj_batch_id_9500(void)
{
	(void)NULL;
	return b9500_id();
}

/*
 * gj_graph_milestone_9500 - report this TU's graph milestone revision.
 *
 * Always returns 9500 (MILESTONE 9500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9500(void)
{
	return b9500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9500(void)
    __attribute__((alias("gj_batch_id_9500")));

uint32_t __gj_graph_milestone_9500(void)
    __attribute__((alias("gj_graph_milestone_9500")));
