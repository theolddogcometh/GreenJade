/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13400: MILESTONE 13400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13400(void);
 *     - Returns the compile-time graph batch number for this TU (13400).
 *   uint32_t gj_graph_milestone_13400(void);
 *     - Returns the current graph milestone revision (13400).
 *   uint32_t __gj_batch_id_13400  (alias)
 *   uint32_t __gj_graph_milestone_13400  (alias)
 *   __libcgj_batch13400_marker = "libcgj-batch13400"
 *
 * MILESTONE 13400 for the exclusive continuum CREATE-ONLY wave
 * (batches 13391-13399: shell_green_13400, libcgj_green_13400,
 * bar3_ready_13400, product_score_13400, continuum_ready_13400,
 * smoke_soft_13400, dyn_soft_13400, milestone_tag_13400,
 * continuum_wave_13400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13400_marker[] = "libcgj-batch13400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13400_id(void)
{
	return 13400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13400 - report this TU's graph batch number.
 *
 * Always returns 13400.
 */
uint32_t
gj_batch_id_13400(void)
{
	(void)NULL;
	return b13400_id();
}

/*
 * gj_graph_milestone_13400 - report this TU's graph milestone revision.
 *
 * Always returns 13400 (MILESTONE 13400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13400(void)
{
	return b13400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13400(void)
    __attribute__((alias("gj_batch_id_13400")));

uint32_t __gj_graph_milestone_13400(void)
    __attribute__((alias("gj_graph_milestone_13400")));
