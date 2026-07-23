/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3460: MILESTONE 3460 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3460(void);
 *     - Returns the compile-time graph batch number for this TU (3460).
 *   uint32_t gj_graph_milestone_3460(void);
 *     - Returns the current graph milestone revision (3460).
 *   uint32_t __gj_batch_id_3460  (alias)
 *   uint32_t __gj_graph_milestone_3460  (alias)
 *   __libcgj_batch3460_marker = "libcgj-batch3460"
 *
 * MILESTONE 3460 for the exclusive continuum CREATE-ONLY wave
 * (batches 3451-3459: u32/u64 abs_diff, median3, clamp_lo_hi, lerp_q16,
 * u32_smooth). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3460_marker[] = "libcgj-batch3460";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3460_id(void)
{
	return 3460u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3460 - report this TU's graph batch number.
 *
 * Always returns 3460.
 */
uint32_t
gj_batch_id_3460(void)
{
	(void)NULL;
	return b3460_id();
}

/*
 * gj_graph_milestone_3460 - report this TU's graph milestone revision.
 *
 * Always returns 3460 (MILESTONE 3460). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3460(void)
{
	return b3460_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3460(void)
    __attribute__((alias("gj_batch_id_3460")));

uint32_t __gj_graph_milestone_3460(void)
    __attribute__((alias("gj_graph_milestone_3460")));
