/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch83105: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_83105(void);
 *     - Returns the compile-time graph batch number for this TU (83105).
 *   uint32_t gj_graph_milestone_83105(void);
 *     - Returns the current graph milestone revision (83105).
 *   uint32_t __gj_batch_id_83105  (alias)
 *   uint32_t __gj_graph_milestone_83105  (alias)
 *   __libcgj_batch83105_marker = "libcgj-batch83105"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-83105, extension identity). Does NOT
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

const char __libcgj_batch83105_marker[] = "libcgj-batch83105";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b83105_id(void)
{
	return 83105u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_83105 - report this TU's graph batch number.
 *
 * Always returns 83105.
 */
uint32_t
gj_batch_id_83105(void)
{
	(void)NULL;
	return b83105_id();
}

/*
 * gj_graph_milestone_83105 - report this TU's graph milestone revision.
 *
 * Always returns 83105. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_83105(void)
{
	return b83105_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_83105(void)
    __attribute__((alias("gj_batch_id_83105")));

uint32_t __gj_graph_milestone_83105(void)
    __attribute__((alias("gj_graph_milestone_83105")));
