/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95110: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95110(void);
 *     - Returns the compile-time graph batch number for this TU (95110).
 *   uint32_t gj_graph_milestone_95110(void);
 *     - Returns the current graph milestone revision (95110).
 *   uint32_t __gj_batch_id_95110  (alias)
 *   uint32_t __gj_graph_milestone_95110  (alias)
 *   __libcgj_batch95110_marker = "libcgj-batch95110"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-95110, extension identity). Does NOT
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

const char __libcgj_batch95110_marker[] = "libcgj-batch95110";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95110_id(void)
{
	return 95110u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95110 - report this TU's graph batch number.
 *
 * Always returns 95110.
 */
uint32_t
gj_batch_id_95110(void)
{
	(void)NULL;
	return b95110_id();
}

/*
 * gj_graph_milestone_95110 - report this TU's graph milestone revision.
 *
 * Always returns 95110. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_95110(void)
{
	return b95110_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_95110(void)
    __attribute__((alias("gj_batch_id_95110")));

uint32_t __gj_graph_milestone_95110(void)
    __attribute__((alias("gj_graph_milestone_95110")));
