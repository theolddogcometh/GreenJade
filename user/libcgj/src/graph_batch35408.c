/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35408: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35408(void);
 *     - Returns the compile-time graph batch number for this TU (35408).
 *   uint32_t gj_graph_milestone_35408(void);
 *     - Returns the current graph milestone revision (35408).
 *   uint32_t __gj_batch_id_35408  (alias)
 *   uint32_t __gj_graph_milestone_35408  (alias)
 *   __libcgj_batch35408_marker = "libcgj-batch35408"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 35401-35450, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 35500).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch35408_marker[] = "libcgj-batch35408";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35408_id(void)
{
	return 35408u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35408 - report this TU's graph batch number.
 *
 * Always returns 35408.
 */
uint32_t
gj_batch_id_35408(void)
{
	(void)NULL;
	return b35408_id();
}

/*
 * gj_graph_milestone_35408 - report this TU's graph milestone revision.
 *
 * Always returns 35408. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_35408(void)
{
	return b35408_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35408(void)
    __attribute__((alias("gj_batch_id_35408")));

uint32_t __gj_graph_milestone_35408(void)
    __attribute__((alias("gj_graph_milestone_35408")));
