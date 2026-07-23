/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14000: MILESTONE 14000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14000(void);
 *     - Returns the compile-time graph batch number for this TU (14000).
 *   uint32_t gj_graph_milestone_14000(void);
 *     - Returns the current graph milestone revision (14000).
 *   uint32_t __gj_batch_id_14000  (alias)
 *   uint32_t __gj_graph_milestone_14000  (alias)
 *   __libcgj_batch14000_marker = "libcgj-batch14000"
 *
 * MILESTONE 14000 for the exclusive continuum CREATE-ONLY wave
 * (batches 13991-13999: shell_green_14000, libcgj_green_14000,
 * bar3_ready_14000, product_score_14000, continuum_ready_14000,
 * smoke_soft_14000, dyn_soft_14000, milestone_tag_14000,
 * continuum_wave_14000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14000_marker[] = "libcgj-batch14000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14000_id(void)
{
	return 14000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14000 - report this TU's graph batch number.
 *
 * Always returns 14000.
 */
uint32_t
gj_batch_id_14000(void)
{
	(void)NULL;
	return b14000_id();
}

/*
 * gj_graph_milestone_14000 - report this TU's graph milestone revision.
 *
 * Always returns 14000 (MILESTONE 14000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14000(void)
{
	return b14000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14000(void)
    __attribute__((alias("gj_batch_id_14000")));

uint32_t __gj_graph_milestone_14000(void)
    __attribute__((alias("gj_graph_milestone_14000")));
