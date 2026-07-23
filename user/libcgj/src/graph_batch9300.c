/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9300: MILESTONE 9300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9300(void);
 *     - Returns the compile-time graph batch number for this TU (9300).
 *   uint32_t gj_graph_milestone_9300(void);
 *     - Returns the current graph milestone revision (9300).
 *   uint32_t __gj_batch_id_9300  (alias)
 *   uint32_t __gj_graph_milestone_9300  (alias)
 *   __libcgj_batch9300_marker = "libcgj-batch9300"
 *
 * MILESTONE 9300 for the exclusive continuum CREATE-ONLY wave
 * (batches 9291-9299: shell_green_9300, libcgj_green_9300,
 * bar3_ready_9300, product_score_9300, continuum_ready_9300,
 * smoke_soft_9300, dyn_soft_9300, milestone_tag_9300,
 * continuum_wave_9300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9300_marker[] = "libcgj-batch9300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9300_id(void)
{
	return 9300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9300 - report this TU's graph batch number.
 *
 * Always returns 9300.
 */
uint32_t
gj_batch_id_9300(void)
{
	(void)NULL;
	return b9300_id();
}

/*
 * gj_graph_milestone_9300 - report this TU's graph milestone revision.
 *
 * Always returns 9300 (MILESTONE 9300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9300(void)
{
	return b9300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9300(void)
    __attribute__((alias("gj_batch_id_9300")));

uint32_t __gj_graph_milestone_9300(void)
    __attribute__((alias("gj_graph_milestone_9300")));
