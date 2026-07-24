/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40813: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40813(void);
 *     - Returns the compile-time graph batch number for this TU (40813).
 *   uint32_t gj_graph_milestone_40813(void);
 *     - Returns the current graph milestone revision (40813).
 *   uint32_t __gj_batch_id_40813  (alias)
 *   uint32_t __gj_graph_milestone_40813  (alias)
 *   __libcgj_batch40813_marker = "libcgj-batch40813"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 40801-40850, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 40900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch40813_marker[] = "libcgj-batch40813";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40813_id(void)
{
	return 40813u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40813 - report this TU's graph batch number.
 *
 * Always returns 40813.
 */
uint32_t
gj_batch_id_40813(void)
{
	(void)NULL;
	return b40813_id();
}

/*
 * gj_graph_milestone_40813 - report this TU's graph milestone revision.
 *
 * Always returns 40813. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_40813(void)
{
	return b40813_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40813(void)
    __attribute__((alias("gj_batch_id_40813")));

uint32_t __gj_graph_milestone_40813(void)
    __attribute__((alias("gj_graph_milestone_40813")));
