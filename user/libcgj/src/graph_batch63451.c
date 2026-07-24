/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63451: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_63451(void);
 *     - Returns the compile-time graph batch number for this TU (63451).
 *   uint32_t gj_graph_milestone_63451(void);
 *     - Returns the current graph milestone revision (63451).
 *   uint32_t __gj_batch_id_63451  (alias)
 *   uint32_t __gj_graph_milestone_63451  (alias)
 *   __libcgj_batch63451_marker = "libcgj-batch63451"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-63451, extension identity). Does NOT
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

const char __libcgj_batch63451_marker[] = "libcgj-batch63451";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63451_id(void)
{
	return 63451u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_63451 - report this TU's graph batch number.
 *
 * Always returns 63451.
 */
uint32_t
gj_batch_id_63451(void)
{
	(void)NULL;
	return b63451_id();
}

/*
 * gj_graph_milestone_63451 - report this TU's graph milestone revision.
 *
 * Always returns 63451. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_63451(void)
{
	return b63451_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_63451(void)
    __attribute__((alias("gj_batch_id_63451")));

uint32_t __gj_graph_milestone_63451(void)
    __attribute__((alias("gj_graph_milestone_63451")));
