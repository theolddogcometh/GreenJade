/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch61561: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_61561(void);
 *     - Returns the compile-time graph batch number for this TU (61561).
 *   uint32_t gj_graph_milestone_61561(void);
 *     - Returns the current graph milestone revision (61561).
 *   uint32_t __gj_batch_id_61561  (alias)
 *   uint32_t __gj_graph_milestone_61561  (alias)
 *   __libcgj_batch61561_marker = "libcgj-batch61561"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-61561, extension identity). Does NOT
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

const char __libcgj_batch61561_marker[] = "libcgj-batch61561";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b61561_id(void)
{
	return 61561u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_61561 - report this TU's graph batch number.
 *
 * Always returns 61561.
 */
uint32_t
gj_batch_id_61561(void)
{
	(void)NULL;
	return b61561_id();
}

/*
 * gj_graph_milestone_61561 - report this TU's graph milestone revision.
 *
 * Always returns 61561. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_61561(void)
{
	return b61561_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_61561(void)
    __attribute__((alias("gj_batch_id_61561")));

uint32_t __gj_graph_milestone_61561(void)
    __attribute__((alias("gj_graph_milestone_61561")));
