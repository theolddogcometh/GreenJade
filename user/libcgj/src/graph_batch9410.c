/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9410: MILESTONE 9410 batch identity for the
 * post-9400 continuum product deepen continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9410(void);
 *     - Returns the compile-time graph batch number for this TU (9410).
 *   uint32_t gj_graph_milestone_9410(void);
 *     - Returns milestone revision 9410 (end of 9401-9410 wave).
 *   uint32_t __gj_batch_id_9410  (alias)
 *   uint32_t __gj_graph_milestone_9410  (alias)
 *   __libcgj_batch9410_marker = "libcgj-batch9410"
 *
 * MILESTONE 9410 for the exclusive continuum CREATE-ONLY wave
 * (batches 9401-9409: continuum_product_deepen_root_9401,
 * continuum_product_deepen_gate_9402, continuum_product_deepen_path_9403,
 * continuum_product_deepen_score_9404, continuum_shell_ready_9405,
 * continuum_libcgj_ready_9406, continuum_bar3_stub_9407,
 * continuum_smoke_soft_9408, continuum_dyn_soft_9409). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9410_marker[] = "libcgj-batch9410";

#define B9410_BATCH_ID  9410u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9410_id(void)
{
	return B9410_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9410 - report this TU's graph batch number.
 *
 * Always returns 9410.
 */
uint32_t
gj_batch_id_9410(void)
{
	(void)NULL;
	return b9410_id();
}

/*
 * gj_graph_milestone_9410 - report this TU's graph milestone revision.
 *
 * Always returns 9410 (MILESTONE 9410). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9410(void)
{
	return b9410_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9410(void)
    __attribute__((alias("gj_batch_id_9410")));

uint32_t __gj_graph_milestone_9410(void)
    __attribute__((alias("gj_graph_milestone_9410")));
