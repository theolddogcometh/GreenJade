/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9010: MILESTONE 9010 batch identity for the
 * post-9000 continuum product deepen continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9010(void);
 *     - Returns the compile-time graph batch number for this TU (9010).
 *   uint32_t gj_graph_milestone_9010(void);
 *     - Returns milestone revision 9010 (end of 9001-9010 wave).
 *   uint32_t __gj_batch_id_9010  (alias)
 *   uint32_t __gj_graph_milestone_9010  (alias)
 *   __libcgj_batch9010_marker = "libcgj-batch9010"
 *
 * MILESTONE 9010 for the exclusive continuum CREATE-ONLY wave
 * (batches 9001-9009: continuum_product_deepen_root_9001,
 * continuum_product_deepen_gate_9002, continuum_product_deepen_path_9003,
 * continuum_product_deepen_score_9004, continuum_shell_ready_9005,
 * continuum_libcgj_ready_9006, continuum_bar3_stub_9007,
 * continuum_smoke_soft_9008, continuum_dyn_soft_9009). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9010_marker[] = "libcgj-batch9010";

#define B9010_BATCH_ID  9010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9010_id(void)
{
	return B9010_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9010 - report this TU's graph batch number.
 *
 * Always returns 9010.
 */
uint32_t
gj_batch_id_9010(void)
{
	(void)NULL;
	return b9010_id();
}

/*
 * gj_graph_milestone_9010 - report this TU's graph milestone revision.
 *
 * Always returns 9010 (MILESTONE 9010). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9010(void)
{
	return b9010_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9010(void)
    __attribute__((alias("gj_batch_id_9010")));

uint32_t __gj_graph_milestone_9010(void)
    __attribute__((alias("gj_graph_milestone_9010")));
