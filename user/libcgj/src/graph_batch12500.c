/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12500: MILESTONE 12500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12500(void);
 *     - Returns the compile-time graph batch number for this TU (12500).
 *   uint32_t gj_graph_milestone_12500(void);
 *     - Returns the current graph milestone revision (12500).
 *   uint32_t __gj_batch_id_12500  (alias)
 *   uint32_t __gj_graph_milestone_12500  (alias)
 *   __libcgj_batch12500_marker = "libcgj-batch12500"
 *
 * MILESTONE 12500 for the exclusive continuum CREATE-ONLY wave
 * (batches 12491-12499: shell_green_12500, libcgj_green_12500,
 * bar3_ready_12500, product_score_12500, continuum_ready_12500,
 * smoke_soft_12500, dyn_soft_12500, milestone_tag_12500,
 * continuum_wave_12500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12500_marker[] = "libcgj-batch12500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12500_id(void)
{
	return 12500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12500 - report this TU's graph batch number.
 *
 * Always returns 12500.
 */
uint32_t
gj_batch_id_12500(void)
{
	(void)NULL;
	return b12500_id();
}

/*
 * gj_graph_milestone_12500 - report this TU's graph milestone revision.
 *
 * Always returns 12500 (MILESTONE 12500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12500(void)
{
	return b12500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12500(void)
    __attribute__((alias("gj_batch_id_12500")));

uint32_t __gj_graph_milestone_12500(void)
    __attribute__((alias("gj_graph_milestone_12500")));
