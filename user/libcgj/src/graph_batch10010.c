/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10010: post-10000 continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10010(void);
 *     - Returns the compile-time graph batch number for this TU (10010).
 *   uint32_t gj_graph_milestone_10010(void);
 *     - Returns the current graph milestone revision (10010).
 *   uint32_t __gj_batch_id_10010  (alias)
 *   uint32_t __gj_graph_milestone_10010  (alias)
 *   __libcgj_batch10010_marker = "libcgj-batch10010"
 *
 * Milestone marker for the exclusive continuum CREATE-ONLY wave
 * (batches 10001-10009: product_deepen_root_10001, product_deepen_gate_10002,
 * product_deepen_path_10003, product_deepen_score_10004,
 * shell_ready_10005, libcgj_ready_10006, bar3_stub_10007,
 * smoke_soft_10008, dyn_soft_10009). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10010_marker[] = "libcgj-batch10010";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10010_id(void)
{
	return 10010u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10010 - report this TU's graph batch number.
 *
 * Always returns 10010.
 */
uint32_t
gj_batch_id_10010(void)
{
	(void)NULL;
	return b10010_id();
}

/*
 * gj_graph_milestone_10010 - report this TU's graph milestone revision.
 *
 * Always returns 10010 (post-10000 continuum product deepen wave).
 * Distinct from earlier gj_graph_milestone / gj_graph_milestone_N so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_10010(void)
{
	return b10010_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10010(void)
    __attribute__((alias("gj_batch_id_10010")));

uint32_t __gj_graph_milestone_10010(void)
    __attribute__((alias("gj_graph_milestone_10010")));
