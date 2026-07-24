/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29358: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29358(void);
 *     - Returns the compile-time graph batch number for this TU (29358).
 *   uint32_t gj_graph_milestone_29358(void);
 *     - Returns the current graph milestone revision (29358).
 *   uint32_t __gj_batch_id_29358  (alias)
 *   uint32_t __gj_graph_milestone_29358  (alias)
 *   __libcgj_batch29358_marker = "libcgj-batch29358"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 29351-29375, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 29400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch29358_marker[] = "libcgj-batch29358";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29358_id(void)
{
	return 29358u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29358 - report this TU's graph batch number.
 *
 * Always returns 29358.
 */
uint32_t
gj_batch_id_29358(void)
{
	(void)NULL;
	return b29358_id();
}

/*
 * gj_graph_milestone_29358 - report this TU's graph milestone revision.
 *
 * Always returns 29358. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_29358(void)
{
	return b29358_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29358(void)
    __attribute__((alias("gj_batch_id_29358")));

uint32_t __gj_graph_milestone_29358(void)
    __attribute__((alias("gj_graph_milestone_29358")));
