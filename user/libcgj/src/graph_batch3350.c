/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3350: MILESTONE 3350 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3350(void);
 *     - Returns the compile-time graph batch number for this TU (3350).
 *   uint32_t gj_graph_milestone_3350(void);
 *     - Returns the current graph milestone revision (3350).
 *   uint32_t __gj_batch_id_3350  (alias)
 *   uint32_t __gj_graph_milestone_3350  (alias)
 *   __libcgj_batch3350_marker = "libcgj-batch3350"
 *
 * MILESTONE 3350 for the exclusive continuum CREATE-ONLY wave
 * (batches 3341-3350: arena_reset_u, arena_alloc_u, slab_idx_u,
 * pool_free_list_u, refcount_inc_u, refcount_dec_u, spin_hint_u,
 * barrier_phase_u, product_wave_3350). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3350_marker[] = "libcgj-batch3350";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3350_id(void)
{
	return 3350u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3350 - report this TU's graph batch number.
 *
 * Always returns 3350.
 */
uint32_t
gj_batch_id_3350(void)
{
	(void)NULL;
	return b3350_id();
}

/*
 * gj_graph_milestone_3350 - report this TU's graph milestone revision.
 *
 * Always returns 3350 (MILESTONE 3350). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3350(void)
{
	return b3350_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3350(void)
    __attribute__((alias("gj_batch_id_3350")));

uint32_t __gj_graph_milestone_3350(void)
    __attribute__((alias("gj_graph_milestone_3350")));
