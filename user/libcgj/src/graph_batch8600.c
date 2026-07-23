/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8600: MILESTONE 8600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8600(void);
 *     - Returns the compile-time graph batch number for this TU (8600).
 *   uint32_t gj_graph_milestone_8600(void);
 *     - Returns the current graph milestone revision (8600).
 *   uint32_t __gj_batch_id_8600  (alias)
 *   uint32_t __gj_graph_milestone_8600  (alias)
 *   __libcgj_batch8600_marker = "libcgj-batch8600"
 *
 * MILESTONE 8600 for the exclusive continuum CREATE-ONLY wave
 * (batches 8591-8599: shell_green_8600, libcgj_green_8600,
 * bar3_ready_8600, product_score_8600, continuum_ready_8600,
 * smoke_soft_8600, dyn_soft_8600, milestone_tag_8600,
 * continuum_wave_8600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8600_marker[] = "libcgj-batch8600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8600_id(void)
{
	return 8600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8600 - report this TU's graph batch number.
 *
 * Always returns 8600.
 */
uint32_t
gj_batch_id_8600(void)
{
	(void)NULL;
	return b8600_id();
}

/*
 * gj_graph_milestone_8600 - report this TU's graph milestone revision.
 *
 * Always returns 8600 (MILESTONE 8600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_8600(void)
{
	return b8600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8600(void)
    __attribute__((alias("gj_batch_id_8600")));

uint32_t __gj_graph_milestone_8600(void)
    __attribute__((alias("gj_graph_milestone_8600")));
