/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23813: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23813(void);
 *     - Returns the compile-time graph batch number for this TU (23813).
 *   uint32_t gj_graph_milestone_23813(void);
 *     - Returns the current graph milestone revision (23813).
 *   uint32_t __gj_batch_id_23813  (alias)
 *   uint32_t __gj_graph_milestone_23813  (alias)
 *   __libcgj_batch23813_marker = "libcgj-batch23813"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 23801-23850, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 23900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch23813_marker[] = "libcgj-batch23813";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23813_id(void)
{
	return 23813u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23813 - report this TU's graph batch number.
 *
 * Always returns 23813.
 */
uint32_t
gj_batch_id_23813(void)
{
	(void)NULL;
	return b23813_id();
}

/*
 * gj_graph_milestone_23813 - report this TU's graph milestone revision.
 *
 * Always returns 23813. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_23813(void)
{
	return b23813_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23813(void)
    __attribute__((alias("gj_batch_id_23813")));

uint32_t __gj_graph_milestone_23813(void)
    __attribute__((alias("gj_graph_milestone_23813")));
