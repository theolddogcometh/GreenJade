/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3160: MILESTONE 3160 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3160(void);
 *     - Returns the compile-time graph batch number for this TU (3160).
 *   uint32_t gj_graph_milestone_3160(void);
 *     - Returns the current graph milestone revision (3160).
 *   uint32_t __gj_batch_id_3160  (alias)
 *   uint32_t __gj_graph_milestone_3160  (alias)
 *   __libcgj_batch3160_marker = "libcgj-batch3160"
 *
 * MILESTONE 3160 for the exclusive continuum CREATE-ONLY wave
 * (batches 3151-3160: u32_log2_floor_u, u64_log2_floor_u,
 * u32_is_aligned_u, u64_is_aligned_u, ptr_align_up_u, ptr_align_down_u,
 * size_align_up_u, size_align_down_u, range_overlap_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3160_marker[] = "libcgj-batch3160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3160_id(void)
{
	return 3160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3160 - report this TU's graph batch number.
 *
 * Always returns 3160.
 */
uint32_t
gj_batch_id_3160(void)
{
	(void)NULL;
	return b3160_id();
}

/*
 * gj_graph_milestone_3160 - report this TU's graph milestone revision.
 *
 * Always returns 3160 (MILESTONE 3160). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3160(void)
{
	return b3160_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3160(void)
    __attribute__((alias("gj_batch_id_3160")));

uint32_t __gj_graph_milestone_3160(void)
    __attribute__((alias("gj_graph_milestone_3160")));
