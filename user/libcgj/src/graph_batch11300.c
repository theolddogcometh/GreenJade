/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11300: MILESTONE 11300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11300(void);
 *     - Returns the compile-time graph batch number for this TU (11300).
 *   uint32_t gj_graph_milestone_11300(void);
 *     - Returns the current graph milestone revision (11300).
 *   uint32_t __gj_batch_id_11300  (alias)
 *   uint32_t __gj_graph_milestone_11300  (alias)
 *   __libcgj_batch11300_marker = "libcgj-batch11300"
 *
 * MILESTONE 11300 for the exclusive continuum CREATE-ONLY wave
 * (batches 11291-11299: shell_green_11300, libcgj_green_11300,
 * bar3_ready_11300, product_score_11300, continuum_ready_11300,
 * smoke_soft_11300, dyn_soft_11300, milestone_tag_11300,
 * continuum_wave_11300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11300_marker[] = "libcgj-batch11300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11300_id(void)
{
	return 11300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11300 - report this TU's graph batch number.
 *
 * Always returns 11300.
 */
uint32_t
gj_batch_id_11300(void)
{
	(void)NULL;
	return b11300_id();
}

/*
 * gj_graph_milestone_11300 - report this TU's graph milestone revision.
 *
 * Always returns 11300 (MILESTONE 11300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_11300(void)
{
	return b11300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_11300(void)
    __attribute__((alias("gj_batch_id_11300")));

uint32_t __gj_graph_milestone_11300(void)
    __attribute__((alias("gj_graph_milestone_11300")));
