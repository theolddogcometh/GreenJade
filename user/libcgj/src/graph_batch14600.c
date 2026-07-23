/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14600: MILESTONE 14600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14600(void);
 *     - Returns the compile-time graph batch number for this TU (14600).
 *   uint32_t gj_graph_milestone_14600(void);
 *     - Returns the current graph milestone revision (14600).
 *   uint32_t __gj_batch_id_14600  (alias)
 *   uint32_t __gj_graph_milestone_14600  (alias)
 *   __libcgj_batch14600_marker = "libcgj-batch14600"
 *
 * MILESTONE 14600 for the exclusive continuum CREATE-ONLY wave
 * (batches 14591-14599: shell_green_14600, libcgj_green_14600,
 * bar3_ready_14600, product_score_14600, continuum_ready_14600,
 * smoke_soft_14600, dyn_soft_14600, milestone_tag_14600,
 * continuum_wave_14600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Soft gates only (no Makefile / map /
 * harness wires in this TU). bar3 remains open
 * (gj_bar3_ready_14600 == 0); product score remains open
 * (gj_product_score_14600 == 0).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14600_marker[] = "libcgj-batch14600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14600_id(void)
{
	return 14600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14600 - soft continuum batch identity lamp for wave 14600
 *
 * Always returns 14600u. Soft pure-data batch number tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14600(void)
{
	(void)NULL;
	return b14600_id();
}

/*
 * gj_graph_milestone_14600 - soft continuum milestone lamp for wave 14600
 *
 * Always returns 14600u (MILESTONE 14600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data revision tag. No parent wires.
 */
uint32_t
gj_graph_milestone_14600(void)
{
	return b14600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14600(void)
    __attribute__((alias("gj_batch_id_14600")));

uint32_t __gj_graph_milestone_14600(void)
    __attribute__((alias("gj_graph_milestone_14600")));
