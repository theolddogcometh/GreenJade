/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14400: MILESTONE 14400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14400(void);
 *     - Returns the compile-time graph batch number for this TU (14400).
 *   uint32_t gj_graph_milestone_14400(void);
 *     - Returns the current graph milestone revision (14400).
 *   uint32_t __gj_batch_id_14400  (alias)
 *   uint32_t __gj_graph_milestone_14400  (alias)
 *   __libcgj_batch14400_marker = "libcgj-batch14400"
 *
 * MILESTONE 14400 for the exclusive continuum CREATE-ONLY wave
 * (batches 14391-14399: shell_green_14400, libcgj_green_14400,
 * bar3_ready_14400, product_score_14400, continuum_ready_14400,
 * smoke_soft_14400, dyn_soft_14400, milestone_tag_14400,
 * continuum_wave_14400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14400_marker[] = "libcgj-batch14400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14400_id(void)
{
	return 14400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14400 - report this TU's graph batch number.
 *
 * Always returns 14400.
 */
uint32_t
gj_batch_id_14400(void)
{
	(void)NULL;
	return b14400_id();
}

/*
 * gj_graph_milestone_14400 - report this TU's graph milestone revision.
 *
 * Always returns 14400 (MILESTONE 14400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14400(void)
{
	return b14400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14400(void)
    __attribute__((alias("gj_batch_id_14400")));

uint32_t __gj_graph_milestone_14400(void)
    __attribute__((alias("gj_graph_milestone_14400")));
