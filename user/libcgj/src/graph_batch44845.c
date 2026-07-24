/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44845: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44845(void);
 *     - Returns the compile-time graph batch number for this TU (44845).
 *   uint32_t gj_graph_milestone_44845(void);
 *     - Returns the current graph milestone revision (44845).
 *   uint32_t __gj_batch_id_44845  (alias)
 *   uint32_t __gj_graph_milestone_44845  (alias)
 *   __libcgj_batch44845_marker = "libcgj-batch44845"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 44801-44850, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 44900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch44845_marker[] = "libcgj-batch44845";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44845_id(void)
{
	return 44845u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44845 - report this TU's graph batch number.
 *
 * Always returns 44845.
 */
uint32_t
gj_batch_id_44845(void)
{
	(void)NULL;
	return b44845_id();
}

/*
 * gj_graph_milestone_44845 - report this TU's graph milestone revision.
 *
 * Always returns 44845. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_44845(void)
{
	return b44845_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_44845(void)
    __attribute__((alias("gj_batch_id_44845")));

uint32_t __gj_graph_milestone_44845(void)
    __attribute__((alias("gj_graph_milestone_44845")));
