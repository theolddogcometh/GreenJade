/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11000: MILESTONE 11000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11000(void);
 *     - Returns the compile-time graph batch number for this TU (11000).
 *   uint32_t gj_graph_milestone_11000(void);
 *     - Returns the current graph milestone revision (11000).
 *   uint32_t __gj_batch_id_11000  (alias)
 *   uint32_t __gj_graph_milestone_11000  (alias)
 *   __libcgj_batch11000_marker = "libcgj-batch11000"
 *
 * MILESTONE 11000 for the exclusive continuum CREATE-ONLY wave
 * (batches 10991-10999: shell_green_11000, libcgj_green_11000,
 * bar3_ready_11000, product_score_11000, continuum_ready_11000,
 * smoke_soft_11000, dyn_soft_11000, milestone_tag_11000,
 * continuum_wave_11000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11000_marker[] = "libcgj-batch11000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11000_id(void)
{
	return 11000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11000 - report this TU's graph batch number.
 *
 * Always returns 11000.
 */
uint32_t
gj_batch_id_11000(void)
{
	(void)NULL;
	return b11000_id();
}

/*
 * gj_graph_milestone_11000 - report this TU's graph milestone revision.
 *
 * Always returns 11000 (MILESTONE 11000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_11000(void)
{
	return b11000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_11000(void)
    __attribute__((alias("gj_batch_id_11000")));

uint32_t __gj_graph_milestone_11000(void)
    __attribute__((alias("gj_graph_milestone_11000")));
