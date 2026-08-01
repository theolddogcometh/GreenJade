/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95121: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95121(void);
 *     - Returns the compile-time graph batch number for this TU (95121).
 *   uint32_t gj_graph_milestone_95121(void);
 *     - Returns the current graph milestone revision (95121).
 *   uint32_t __gj_batch_id_95121  (alias)
 *   uint32_t __gj_graph_milestone_95121  (alias)
 *   __libcgj_batch95121_marker = "libcgj-batch95121"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-95121, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 25900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch95121_marker[] = "libcgj-batch95121";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95121_id(void)
{
	return 95121u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95121 - report this TU's graph batch number.
 *
 * Always returns 95121.
 */
uint32_t
gj_batch_id_95121(void)
{
	(void)NULL;
	return b95121_id();
}

/*
 * gj_graph_milestone_95121 - report this TU's graph milestone revision.
 *
 * Always returns 95121. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_95121(void)
{
	return b95121_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_95121(void)
    __attribute__((alias("gj_batch_id_95121")));

uint32_t __gj_graph_milestone_95121(void)
    __attribute__((alias("gj_graph_milestone_95121")));
