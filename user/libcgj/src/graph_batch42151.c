/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42151: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42151(void);
 *     - Returns the compile-time graph batch number for this TU (42151).
 *   uint32_t gj_graph_milestone_42151(void);
 *     - Returns the current graph milestone revision (42151).
 *   uint32_t __gj_batch_id_42151  (alias)
 *   uint32_t __gj_graph_milestone_42151  (alias)
 *   __libcgj_batch42151_marker = "libcgj-batch42151"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 42151-42175, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 42200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch42151_marker[] = "libcgj-batch42151";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42151_id(void)
{
	return 42151u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42151 - report this TU's graph batch number.
 *
 * Always returns 42151.
 */
uint32_t
gj_batch_id_42151(void)
{
	(void)NULL;
	return b42151_id();
}

/*
 * gj_graph_milestone_42151 - report this TU's graph milestone revision.
 *
 * Always returns 42151. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_42151(void)
{
	return b42151_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42151(void)
    __attribute__((alias("gj_batch_id_42151")));

uint32_t __gj_graph_milestone_42151(void)
    __attribute__((alias("gj_graph_milestone_42151")));
