/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13300: MILESTONE 13300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13300(void);
 *     - Returns the compile-time graph batch number for this TU (13300).
 *   uint32_t gj_graph_milestone_13300(void);
 *     - Returns the current graph milestone revision (13300).
 *   uint32_t __gj_batch_id_13300  (alias)
 *   uint32_t __gj_graph_milestone_13300  (alias)
 *   __libcgj_batch13300_marker = "libcgj-batch13300"
 *
 * MILESTONE 13300 for the exclusive continuum CREATE-ONLY wave
 * (batches 13291-13299: shell_green_13300, libcgj_green_13300,
 * bar3_ready_13300, product_score_13300, continuum_ready_13300,
 * smoke_soft_13300, dyn_soft_13300, milestone_tag_13300,
 * continuum_wave_13300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13300_marker[] = "libcgj-batch13300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13300_id(void)
{
	return 13300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13300 - report this TU's graph batch number.
 *
 * Always returns 13300.
 */
uint32_t
gj_batch_id_13300(void)
{
	(void)NULL;
	return b13300_id();
}

/*
 * gj_graph_milestone_13300 - report this TU's graph milestone revision.
 *
 * Always returns 13300 (MILESTONE 13300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13300(void)
{
	return b13300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13300(void)
    __attribute__((alias("gj_batch_id_13300")));

uint32_t __gj_graph_milestone_13300(void)
    __attribute__((alias("gj_graph_milestone_13300")));
