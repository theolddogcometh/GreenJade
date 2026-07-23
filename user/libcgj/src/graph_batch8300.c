/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8300: MILESTONE 8300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8300(void);
 *     - Returns the compile-time graph batch number for this TU (8300).
 *   uint32_t gj_graph_milestone_8300(void);
 *     - Returns the current graph milestone revision (8300).
 *   uint32_t __gj_batch_id_8300  (alias)
 *   uint32_t __gj_graph_milestone_8300  (alias)
 *   __libcgj_batch8300_marker = "libcgj-batch8300"
 *
 * MILESTONE 8300 for the exclusive continuum CREATE-ONLY wave
 * (batches 8291-8299: shell_green_8300, libcgj_green_8300,
 * bar3_ready_8300, product_score_8300, continuum_ready_8300,
 * smoke_soft_8300, dyn_soft_8300, milestone_tag_8300,
 * continuum_wave_8300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8300_marker[] = "libcgj-batch8300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8300_id(void)
{
	return 8300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8300 - report this TU's graph batch number.
 *
 * Always returns 8300.
 */
uint32_t
gj_batch_id_8300(void)
{
	(void)NULL;
	return b8300_id();
}

/*
 * gj_graph_milestone_8300 - report this TU's graph milestone revision.
 *
 * Always returns 8300 (MILESTONE 8300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8300(void)
{
	return b8300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8300(void)
    __attribute__((alias("gj_batch_id_8300")));

uint32_t __gj_graph_milestone_8300(void)
    __attribute__((alias("gj_graph_milestone_8300")));
