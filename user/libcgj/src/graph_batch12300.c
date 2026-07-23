/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12300: MILESTONE 12300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12300(void);
 *     - Returns the compile-time graph batch number for this TU (12300).
 *   uint32_t gj_graph_milestone_12300(void);
 *     - Returns the current graph milestone revision (12300).
 *   uint32_t __gj_batch_id_12300  (alias)
 *   uint32_t __gj_graph_milestone_12300  (alias)
 *   __libcgj_batch12300_marker = "libcgj-batch12300"
 *
 * MILESTONE 12300 for the exclusive continuum CREATE-ONLY wave
 * (batches 12291-12299: shell_green_12300, libcgj_green_12300,
 * bar3_ready_12300, product_score_12300, continuum_ready_12300,
 * smoke_soft_12300, dyn_soft_12300, milestone_tag_12300,
 * continuum_wave_12300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12300_marker[] = "libcgj-batch12300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12300_id(void)
{
	return 12300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12300 - report this TU's graph batch number.
 *
 * Always returns 12300.
 */
uint32_t
gj_batch_id_12300(void)
{
	(void)NULL;
	return b12300_id();
}

/*
 * gj_graph_milestone_12300 - report this TU's graph milestone revision.
 *
 * Always returns 12300 (MILESTONE 12300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12300(void)
{
	return b12300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12300(void)
    __attribute__((alias("gj_batch_id_12300")));

uint32_t __gj_graph_milestone_12300(void)
    __attribute__((alias("gj_graph_milestone_12300")));
