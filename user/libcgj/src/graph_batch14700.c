/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14700: MILESTONE 14700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14700(void);
 *     - Returns the compile-time graph batch number for this TU (14700).
 *   uint32_t gj_graph_milestone_14700(void);
 *     - Returns the current graph milestone revision (14700).
 *   uint32_t __gj_batch_id_14700  (alias)
 *   uint32_t __gj_graph_milestone_14700  (alias)
 *   __libcgj_batch14700_marker = "libcgj-batch14700"
 *
 * MILESTONE 14700 for the exclusive continuum CREATE-ONLY wave
 * (batches 14691-14699: shell_green_14700, libcgj_green_14700,
 * bar3_ready_14700, product_score_14700, continuum_ready_14700,
 * smoke_soft_14700, dyn_soft_14700, milestone_tag_14700,
 * continuum_wave_14700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14700_marker[] = "libcgj-batch14700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14700_id(void)
{
	return 14700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14700 - report this TU's graph batch number.
 *
 * Always returns 14700.
 */
uint32_t
gj_batch_id_14700(void)
{
	(void)NULL;
	return b14700_id();
}

/*
 * gj_graph_milestone_14700 - report this TU's graph milestone revision.
 *
 * Always returns 14700 (MILESTONE 14700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14700(void)
{
	return b14700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14700(void)
    __attribute__((alias("gj_batch_id_14700")));

uint32_t __gj_graph_milestone_14700(void)
    __attribute__((alias("gj_graph_milestone_14700")));
