/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12400: MILESTONE 12400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12400(void);
 *     - Returns the compile-time graph batch number for this TU (12400).
 *   uint32_t gj_graph_milestone_12400(void);
 *     - Returns the current graph milestone revision (12400).
 *   uint32_t __gj_batch_id_12400  (alias)
 *   uint32_t __gj_graph_milestone_12400  (alias)
 *   __libcgj_batch12400_marker = "libcgj-batch12400"
 *
 * MILESTONE 12400 for the exclusive continuum CREATE-ONLY wave
 * (batches 12391-12399: shell_green_12400, libcgj_green_12400,
 * bar3_ready_12400, product_score_12400, continuum_ready_12400,
 * smoke_soft_12400, dyn_soft_12400, milestone_tag_12400,
 * continuum_wave_12400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12400_marker[] = "libcgj-batch12400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12400_id(void)
{
	return 12400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12400 - report this TU's graph batch number.
 *
 * Always returns 12400.
 */
uint32_t
gj_batch_id_12400(void)
{
	(void)NULL;
	return b12400_id();
}

/*
 * gj_graph_milestone_12400 - report this TU's graph milestone revision.
 *
 * Always returns 12400 (MILESTONE 12400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12400(void)
{
	return b12400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12400(void)
    __attribute__((alias("gj_batch_id_12400")));

uint32_t __gj_graph_milestone_12400(void)
    __attribute__((alias("gj_graph_milestone_12400")));
