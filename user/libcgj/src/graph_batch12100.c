/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12100: MILESTONE 12100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12100(void);
 *     - Returns the compile-time graph batch number for this TU (12100).
 *   uint32_t gj_graph_milestone_12100(void);
 *     - Returns the current graph milestone revision (12100).
 *   uint32_t __gj_batch_id_12100  (alias)
 *   uint32_t __gj_graph_milestone_12100  (alias)
 *   __libcgj_batch12100_marker = "libcgj-batch12100"
 *
 * MILESTONE 12100 for the exclusive continuum CREATE-ONLY wave
 * (batches 12091-12099: shell_green_12100, libcgj_green_12100,
 * bar3_ready_12100, product_score_12100, continuum_ready_12100,
 * smoke_soft_12100, dyn_soft_12100, milestone_tag_12100,
 * continuum_wave_12100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12100_marker[] = "libcgj-batch12100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12100_id(void)
{
	return 12100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12100 - report this TU's graph batch number.
 *
 * Always returns 12100.
 */
uint32_t
gj_batch_id_12100(void)
{
	(void)NULL;
	return b12100_id();
}

/*
 * gj_graph_milestone_12100 - report this TU's graph milestone revision.
 *
 * Always returns 12100 (MILESTONE 12100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12100(void)
{
	return b12100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12100(void)
    __attribute__((alias("gj_batch_id_12100")));

uint32_t __gj_graph_milestone_12100(void)
    __attribute__((alias("gj_graph_milestone_12100")));
