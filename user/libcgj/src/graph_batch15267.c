/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15267: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15267(void);
 *     - Returns the compile-time graph batch number for this TU (15267).
 *   uint32_t gj_graph_milestone_15267(void);
 *     - Returns the current graph milestone revision (15267).
 *   uint32_t __gj_batch_id_15267  (alias)
 *   uint32_t __gj_graph_milestone_15267  (alias)
 *   __libcgj_batch15267_marker = "libcgj-batch15267"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15251-15275). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15300. bar3 still open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15267_marker[] = "libcgj-batch15267";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15267_id(void)
{
	return 15267u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15267 - report this TU's graph batch number.
 *
 * Always returns 15267.
 */
uint32_t
gj_batch_id_15267(void)
{
	(void)NULL;
	return b15267_id();
}

/*
 * gj_graph_milestone_15267 - report this TU's graph milestone revision.
 *
 * Always returns 15267. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15267(void)
{
	return b15267_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15267(void)
    __attribute__((alias("gj_batch_id_15267")));

uint32_t __gj_graph_milestone_15267(void)
    __attribute__((alias("gj_graph_milestone_15267")));
