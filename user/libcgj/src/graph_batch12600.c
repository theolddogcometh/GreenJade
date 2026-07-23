/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12600: MILESTONE 12600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12600(void);
 *     - Returns the compile-time graph batch number for this TU (12600).
 *   uint32_t gj_graph_milestone_12600(void);
 *     - Returns the current graph milestone revision (12600).
 *   uint32_t __gj_batch_id_12600  (alias)
 *   uint32_t __gj_graph_milestone_12600  (alias)
 *   __libcgj_batch12600_marker = "libcgj-batch12600"
 *
 * MILESTONE 12600 for the exclusive continuum CREATE-ONLY wave
 * (batches 12591-12599: shell_green_12600, libcgj_green_12600,
 * bar3_ready_12600, product_score_12600, continuum_ready_12600,
 * smoke_soft_12600, dyn_soft_12600, milestone_tag_12600,
 * continuum_wave_12600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12600_marker[] = "libcgj-batch12600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12600_id(void)
{
	return 12600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12600 - report this TU's graph batch number.
 *
 * Always returns 12600.
 */
uint32_t
gj_batch_id_12600(void)
{
	(void)NULL;
	return b12600_id();
}

/*
 * gj_graph_milestone_12600 - report this TU's graph milestone revision.
 *
 * Always returns 12600 (MILESTONE 12600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12600(void)
{
	return b12600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12600(void)
    __attribute__((alias("gj_batch_id_12600")));

uint32_t __gj_graph_milestone_12600(void)
    __attribute__((alias("gj_graph_milestone_12600")));
