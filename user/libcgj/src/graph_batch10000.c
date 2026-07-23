/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10000: MILESTONE 10000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10000(void);
 *     - Returns the compile-time graph batch number for this TU (10000).
 *   uint32_t gj_graph_milestone_10000(void);
 *     - Returns the current graph milestone revision (10000).
 *   uint32_t __gj_batch_id_10000  (alias)
 *   uint32_t __gj_graph_milestone_10000  (alias)
 *   __libcgj_batch10000_marker = "libcgj-batch10000"
 *
 * MILESTONE 10000 for the exclusive continuum CREATE-ONLY wave
 * (batches 9991-9999: product_deepen_root_10000, product_deepen_gate_10000,
 * product_deepen_path_10000, product_deepen_score_10000,
 * shell_ready_10000, libcgj_ready_10000, bar3_stub_10000,
 * smoke_soft_10000, dyn_soft_10000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10000_marker[] = "libcgj-batch10000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10000_id(void)
{
	return 10000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10000 - report this TU's graph batch number.
 *
 * Always returns 10000.
 */
uint32_t
gj_batch_id_10000(void)
{
	(void)NULL;
	return b10000_id();
}

/*
 * gj_graph_milestone_10000 - report this TU's graph milestone revision.
 *
 * Always returns 10000 (MILESTONE 10000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10000(void)
{
	return b10000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10000(void)
    __attribute__((alias("gj_batch_id_10000")));

uint32_t __gj_graph_milestone_10000(void)
    __attribute__((alias("gj_graph_milestone_10000")));
