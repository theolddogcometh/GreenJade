/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3100: MILESTONE 3100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3100(void);
 *     - Returns the compile-time graph batch number for this TU (3100).
 *   uint32_t gj_graph_milestone_3100(void);
 *     - Returns the current graph milestone revision (3100).
 *   uint32_t __gj_batch_id_3100  (alias)
 *   uint32_t __gj_graph_milestone_3100  (alias)
 *   __libcgj_batch3100_marker = "libcgj-batch3100"
 *
 * MILESTONE 3100 for the exclusive continuum CREATE-ONLY wave
 * (batches 3091-3099: sub_vec_u32_n_u, sub_vec_u64_n_u, wave_id_3100,
 * export_hint_3100, smoke_green_u, dyn_green_u, shell_green_u,
 * libcgj_green_u, bar3_partial_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3100_marker[] = "libcgj-batch3100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3100_id(void)
{
	return 3100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3100 - report this TU's graph batch number.
 *
 * Always returns 3100.
 */
uint32_t
gj_batch_id_3100(void)
{
	(void)NULL;
	return b3100_id();
}

/*
 * gj_graph_milestone_3100 - report this TU's graph milestone revision.
 *
 * Always returns 3100 (MILESTONE 3100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3100(void)
{
	return b3100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3100(void)
    __attribute__((alias("gj_batch_id_3100")));

uint32_t __gj_graph_milestone_3100(void)
    __attribute__((alias("gj_graph_milestone_3100")));
