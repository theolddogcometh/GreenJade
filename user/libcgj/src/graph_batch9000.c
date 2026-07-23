/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9000: MILESTONE 9000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9000(void);
 *     - Returns the compile-time graph batch number for this TU (9000).
 *   uint32_t gj_graph_milestone_9000(void);
 *     - Returns the current graph milestone revision (9000).
 *   uint32_t __gj_batch_id_9000  (alias)
 *   uint32_t __gj_graph_milestone_9000  (alias)
 *   __libcgj_batch9000_marker = "libcgj-batch9000"
 *
 * MILESTONE 9000 for the exclusive continuum CREATE-ONLY wave
 * (batches 8991-8999: shell_green_9000, libcgj_green_9000,
 * bar3_ready_9000, product_score_9000, continuum_ready_9000,
 * smoke_soft_9000, dyn_soft_9000, milestone_tag_9000,
 * continuum_wave_9000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9000_marker[] = "libcgj-batch9000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9000_id(void)
{
	return 9000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9000 - report this TU's graph batch number.
 *
 * Always returns 9000.
 */
uint32_t
gj_batch_id_9000(void)
{
	(void)NULL;
	return b9000_id();
}

/*
 * gj_graph_milestone_9000 - report this TU's graph milestone revision.
 *
 * Always returns 9000 (MILESTONE 9000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9000(void)
{
	return b9000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9000(void)
    __attribute__((alias("gj_batch_id_9000")));

uint32_t __gj_graph_milestone_9000(void)
    __attribute__((alias("gj_graph_milestone_9000")));
