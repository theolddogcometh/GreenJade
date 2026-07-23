/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9310: MILESTONE 9310 batch identity for the
 * post-9300 continuum product deepen continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9310(void);
 *     - Returns the compile-time graph batch number for this TU (9310).
 *   uint32_t gj_graph_milestone_9310(void);
 *     - Returns milestone revision 9310 (end of 9301-9310 wave).
 *   uint32_t __gj_batch_id_9310  (alias)
 *   uint32_t __gj_graph_milestone_9310  (alias)
 *   __libcgj_batch9310_marker = "libcgj-batch9310"
 *
 * MILESTONE 9310 for the exclusive continuum CREATE-ONLY wave
 * (batches 9301-9309: continuum_product_deepen_root_9301,
 * continuum_product_deepen_gate_9302, continuum_product_deepen_path_9303,
 * continuum_product_deepen_score_9304, continuum_shell_ready_9305,
 * continuum_libcgj_ready_9306, continuum_bar3_stub_9307,
 * continuum_smoke_soft_9308, continuum_dyn_soft_9309). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9310_marker[] = "libcgj-batch9310";

#define B9310_BATCH_ID  9310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9310_id(void)
{
	return B9310_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9310 - report this TU's graph batch number.
 *
 * Always returns 9310.
 */
uint32_t
gj_batch_id_9310(void)
{
	(void)NULL;
	return b9310_id();
}

/*
 * gj_graph_milestone_9310 - report this TU's graph milestone revision.
 *
 * Always returns 9310 (MILESTONE 9310). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9310(void)
{
	return b9310_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9310(void)
    __attribute__((alias("gj_batch_id_9310")));

uint32_t __gj_graph_milestone_9310(void)
    __attribute__((alias("gj_graph_milestone_9310")));
