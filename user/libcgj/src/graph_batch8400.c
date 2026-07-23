/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8400: MILESTONE 8400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8400(void);
 *     - Returns the compile-time graph batch number for this TU (8400).
 *   uint32_t gj_graph_milestone_8400(void);
 *     - Returns the current graph milestone revision (8400).
 *   uint32_t __gj_batch_id_8400  (alias)
 *   uint32_t __gj_graph_milestone_8400  (alias)
 *   __libcgj_batch8400_marker = "libcgj-batch8400"
 *
 * MILESTONE 8400 for the exclusive continuum CREATE-ONLY wave
 * (batches 8391-8399: shell_green_8400, libcgj_green_8400,
 * bar3_ready_8400, product_score_8400, continuum_ready_8400,
 * smoke_soft_8400, dyn_soft_8400, milestone_tag_8400,
 * continuum_wave_8400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8400_marker[] = "libcgj-batch8400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8400_id(void)
{
	return 8400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8400 - report this TU's graph batch number.
 *
 * Always returns 8400.
 */
uint32_t
gj_batch_id_8400(void)
{
	(void)NULL;
	return b8400_id();
}

/*
 * gj_graph_milestone_8400 - report this TU's graph milestone revision.
 *
 * Always returns 8400 (MILESTONE 8400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8400(void)
{
	return b8400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8400(void)
    __attribute__((alias("gj_batch_id_8400")));

uint32_t __gj_graph_milestone_8400(void)
    __attribute__((alias("gj_graph_milestone_8400")));
