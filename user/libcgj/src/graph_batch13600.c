/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13600: MILESTONE 13600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13600(void);
 *     - Returns the compile-time graph batch number for this TU (13600).
 *   uint32_t gj_graph_milestone_13600(void);
 *     - Returns the current graph milestone revision (13600).
 *   uint32_t __gj_batch_id_13600  (alias)
 *   uint32_t __gj_graph_milestone_13600  (alias)
 *   __libcgj_batch13600_marker = "libcgj-batch13600"
 *
 * MILESTONE 13600 for the exclusive continuum CREATE-ONLY wave
 * (batches 13591-13599: shell_green_13600, libcgj_green_13600,
 * bar3_ready_13600, product_score_13600, continuum_ready_13600,
 * smoke_soft_13600, dyn_soft_13600, milestone_tag_13600,
 * continuum_wave_13600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13600_marker[] = "libcgj-batch13600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13600_id(void)
{
	return 13600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13600 - report this TU's graph batch number.
 *
 * Always returns 13600.
 */
uint32_t
gj_batch_id_13600(void)
{
	(void)NULL;
	return b13600_id();
}

/*
 * gj_graph_milestone_13600 - report this TU's graph milestone revision.
 *
 * Always returns 13600 (MILESTONE 13600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_13600(void)
{
	return b13600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_13600(void)
    __attribute__((alias("gj_batch_id_13600")));

uint32_t __gj_graph_milestone_13600(void)
    __attribute__((alias("gj_graph_milestone_13600")));
