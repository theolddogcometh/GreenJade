/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13000: MILESTONE 13000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13000(void);
 *     - Returns the compile-time graph batch number for this TU (13000).
 *   uint32_t gj_graph_milestone_13000(void);
 *     - Returns the current graph milestone revision (13000).
 *   uint32_t __gj_batch_id_13000  (alias)
 *   uint32_t __gj_graph_milestone_13000  (alias)
 *   __libcgj_batch13000_marker = "libcgj-batch13000"
 *
 * MILESTONE 13000 for the exclusive continuum CREATE-ONLY wave
 * (batches 12991-12999: shell_green_13000, libcgj_green_13000,
 * bar3_ready_13000, product_score_13000, continuum_ready_13000,
 * smoke_soft_13000, dyn_soft_13000, milestone_tag_13000,
 * continuum_wave_13000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13000_marker[] = "libcgj-batch13000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13000_id(void)
{
	return 13000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13000 - report this TU's graph batch number.
 *
 * Always returns 13000.
 */
uint32_t
gj_batch_id_13000(void)
{
	(void)NULL;
	return b13000_id();
}

/*
 * gj_graph_milestone_13000 - report this TU's graph milestone revision.
 *
 * Always returns 13000 (MILESTONE 13000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13000(void)
{
	return b13000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13000(void)
    __attribute__((alias("gj_batch_id_13000")));

uint32_t __gj_graph_milestone_13000(void)
    __attribute__((alias("gj_graph_milestone_13000")));
