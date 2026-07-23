/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10300: MILESTONE 10300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10300(void);
 *     - Returns the compile-time graph batch number for this TU (10300).
 *   uint32_t gj_graph_milestone_10300(void);
 *     - Returns the current graph milestone revision (10300).
 *   uint32_t __gj_batch_id_10300  (alias)
 *   uint32_t __gj_graph_milestone_10300  (alias)
 *   __libcgj_batch10300_marker = "libcgj-batch10300"
 *
 * MILESTONE 10300 for the exclusive continuum CREATE-ONLY wave
 * (batches 10291-10299: shell_green_10300, libcgj_green_10300,
 * bar3_ready_10300, product_score_10300, continuum_ready_10300,
 * smoke_soft_10300, dyn_soft_10300, milestone_tag_10300,
 * continuum_wave_10300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10300_marker[] = "libcgj-batch10300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10300_id(void)
{
	return 10300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10300 - report this TU's graph batch number.
 *
 * Always returns 10300.
 */
uint32_t
gj_batch_id_10300(void)
{
	(void)NULL;
	return b10300_id();
}

/*
 * gj_graph_milestone_10300 - report this TU's graph milestone revision.
 *
 * Always returns 10300 (MILESTONE 10300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10300(void)
{
	return b10300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10300(void)
    __attribute__((alias("gj_batch_id_10300")));

uint32_t __gj_graph_milestone_10300(void)
    __attribute__((alias("gj_graph_milestone_10300")));
