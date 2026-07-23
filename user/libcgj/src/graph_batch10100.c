/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10100: MILESTONE 10100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10100(void);
 *     - Returns the compile-time graph batch number for this TU (10100).
 *   uint32_t gj_graph_milestone_10100(void);
 *     - Returns the current graph milestone revision (10100).
 *   uint32_t __gj_batch_id_10100  (alias)
 *   uint32_t __gj_graph_milestone_10100  (alias)
 *   __libcgj_batch10100_marker = "libcgj-batch10100"
 *
 * MILESTONE 10100 for the exclusive continuum CREATE-ONLY wave
 * (batches 10091-10099: shell_green_10100, libcgj_green_10100,
 * bar3_ready_10100, product_score_10100, continuum_ready_10100,
 * smoke_soft_10100, dyn_soft_10100, milestone_tag_10100,
 * continuum_wave_10100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10100_marker[] = "libcgj-batch10100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10100_id(void)
{
	return 10100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10100 - report this TU's graph batch number.
 *
 * Always returns 10100.
 */
uint32_t
gj_batch_id_10100(void)
{
	(void)NULL;
	return b10100_id();
}

/*
 * gj_graph_milestone_10100 - report this TU's graph milestone revision.
 *
 * Always returns 10100 (MILESTONE 10100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10100(void)
{
	return b10100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10100(void)
    __attribute__((alias("gj_batch_id_10100")));

uint32_t __gj_graph_milestone_10100(void)
    __attribute__((alias("gj_graph_milestone_10100")));
