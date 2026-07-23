/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9100: MILESTONE 9100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9100(void);
 *     - Returns the compile-time graph batch number for this TU (9100).
 *   uint32_t gj_graph_milestone_9100(void);
 *     - Returns the current graph milestone revision (9100).
 *   uint32_t __gj_batch_id_9100  (alias)
 *   uint32_t __gj_graph_milestone_9100  (alias)
 *   __libcgj_batch9100_marker = "libcgj-batch9100"
 *
 * MILESTONE 9100 for the exclusive continuum CREATE-ONLY wave
 * (batches 9091-9099: shell_green_9100, libcgj_green_9100,
 * bar3_ready_9100, product_score_9100, continuum_ready_9100,
 * smoke_soft_9100, dyn_soft_9100, milestone_tag_9100,
 * continuum_wave_9100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9100_marker[] = "libcgj-batch9100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9100_id(void)
{
	return 9100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9100 - report this TU's graph batch number.
 *
 * Always returns 9100.
 */
uint32_t
gj_batch_id_9100(void)
{
	(void)NULL;
	return b9100_id();
}

/*
 * gj_graph_milestone_9100 - report this TU's graph milestone revision.
 *
 * Always returns 9100 (MILESTONE 9100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9100(void)
{
	return b9100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9100(void)
    __attribute__((alias("gj_batch_id_9100")));

uint32_t __gj_graph_milestone_9100(void)
    __attribute__((alias("gj_graph_milestone_9100")));
