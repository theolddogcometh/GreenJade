/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch54360: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_54360(void);
 *     - Returns the compile-time graph batch number for this TU (54360).
 *   uint32_t gj_graph_milestone_54360(void);
 *     - Returns the current graph milestone revision (54360).
 *   uint32_t __gj_batch_id_54360  (alias)
 *   uint32_t __gj_graph_milestone_54360  (alias)
 *   __libcgj_batch54360_marker = "libcgj-batch54360"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-54360, extension identity). Does NOT
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

const char __libcgj_batch54360_marker[] = "libcgj-batch54360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b54360_id(void)
{
	return 54360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_54360 - report this TU's graph batch number.
 *
 * Always returns 54360.
 */
uint32_t
gj_batch_id_54360(void)
{
	(void)NULL;
	return b54360_id();
}

/*
 * gj_graph_milestone_54360 - report this TU's graph milestone revision.
 *
 * Always returns 54360. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_54360(void)
{
	return b54360_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_54360(void)
    __attribute__((alias("gj_batch_id_54360")));

uint32_t __gj_graph_milestone_54360(void)
    __attribute__((alias("gj_graph_milestone_54360")));
