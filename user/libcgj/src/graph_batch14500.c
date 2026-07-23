/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14500: MILESTONE 14500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14500(void);
 *     - Returns the compile-time graph batch number for this TU (14500).
 *   uint32_t gj_graph_milestone_14500(void);
 *     - Returns the current graph milestone revision (14500).
 *   uint32_t __gj_batch_id_14500  (alias)
 *   uint32_t __gj_graph_milestone_14500  (alias)
 *   __libcgj_batch14500_marker = "libcgj-batch14500"
 *
 * MILESTONE 14500 for the exclusive continuum CREATE-ONLY wave
 * (batches 14491-14499: shell_green_14500, libcgj_green_14500,
 * bar3_ready_14500, product_score_14500, continuum_ready_14500,
 * smoke_soft_14500, dyn_soft_14500, milestone_tag_14500,
 * continuum_wave_14500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14500_marker[] = "libcgj-batch14500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14500_id(void)
{
	return 14500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14500 - report this TU's graph batch number.
 *
 * Always returns 14500.
 */
uint32_t
gj_batch_id_14500(void)
{
	(void)NULL;
	return b14500_id();
}

/*
 * gj_graph_milestone_14500 - report this TU's graph milestone revision.
 *
 * Always returns 14500 (MILESTONE 14500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14500(void)
{
	return b14500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14500(void)
    __attribute__((alias("gj_batch_id_14500")));

uint32_t __gj_graph_milestone_14500(void)
    __attribute__((alias("gj_graph_milestone_14500")));
