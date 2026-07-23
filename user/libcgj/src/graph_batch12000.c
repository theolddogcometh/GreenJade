/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12000: MILESTONE 12000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12000(void);
 *     - Returns the compile-time graph batch number for this TU (12000).
 *   uint32_t gj_graph_milestone_12000(void);
 *     - Returns the current graph milestone revision (12000).
 *   uint32_t __gj_batch_id_12000  (alias)
 *   uint32_t __gj_graph_milestone_12000  (alias)
 *   __libcgj_batch12000_marker = "libcgj-batch12000"
 *
 * MILESTONE 12000 for the exclusive continuum CREATE-ONLY wave
 * (batches 11991-11999: shell_green_12000, libcgj_green_12000,
 * bar3_ready_12000, product_score_12000, continuum_ready_12000,
 * smoke_soft_12000, dyn_soft_12000, milestone_tag_12000,
 * continuum_wave_12000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12000_marker[] = "libcgj-batch12000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12000_id(void)
{
	return 12000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12000 - report this TU's graph batch number.
 *
 * Always returns 12000.
 */
uint32_t
gj_batch_id_12000(void)
{
	(void)NULL;
	return b12000_id();
}

/*
 * gj_graph_milestone_12000 - report this TU's graph milestone revision.
 *
 * Always returns 12000 (MILESTONE 12000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12000(void)
{
	return b12000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12000(void)
    __attribute__((alias("gj_batch_id_12000")));

uint32_t __gj_graph_milestone_12000(void)
    __attribute__((alias("gj_graph_milestone_12000")));
