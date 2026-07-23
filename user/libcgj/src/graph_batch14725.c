/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14725: MILESTONE 14725 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14725(void);
 *     - Returns the compile-time graph batch number for this TU (14725).
 *   uint32_t gj_graph_milestone_14725(void);
 *     - Returns the current graph milestone revision (14725).
 *   uint32_t __gj_batch_id_14725  (alias)
 *   uint32_t __gj_graph_milestone_14725  (alias)
 *   __libcgj_batch14725_marker = "libcgj-batch14725"
 *
 * MILESTONE 14725 for the exclusive continuum CREATE-ONLY wave
 * (batches 14701-14724: shell_green_14701, libcgj_green_14702,
 * bar3_ready_14703, product_score_14704, continuum_ready_14705,
 * smoke_soft_14706, dyn_soft_14707, milestone_tag_14708,
 * continuum_wave_14709, u32_min/max_14710, u32_clamp_14711,
 * u32_absdiff_14712, u32_sat_add_14713, u32_sat_sub_14714,
 * u32_avg_14715, u32_is_pow2_14716, u32_align_up_14717,
 * u32_align_down_14718, be32_load/store_14719, le32_load/store_14720,
 * mem_eq_n_14721, mem_zero_n_14722, ascii_isdigit_span_14723,
 * fnv1a32_14724). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14725_marker[] = "libcgj-batch14725";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14725_id(void)
{
	return 14725u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14725 - report this TU's graph batch number.
 *
 * Always returns 14725.
 */
uint32_t
gj_batch_id_14725(void)
{
	(void)NULL;
	return b14725_id();
}

/*
 * gj_graph_milestone_14725 - report this TU's graph milestone revision.
 *
 * Always returns 14725 (MILESTONE 14725). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14725(void)
{
	return b14725_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14725(void)
    __attribute__((alias("gj_batch_id_14725")));

uint32_t __gj_graph_milestone_14725(void)
    __attribute__((alias("gj_graph_milestone_14725")));
