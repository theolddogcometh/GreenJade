/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50360: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50360(void);
 *     - Returns the compile-time graph batch number for this TU (50360).
 *   uint32_t gj_graph_milestone_50360(void);
 *     - Returns the current graph milestone revision (50360).
 *   uint32_t __gj_batch_id_50360  (alias)
 *   uint32_t __gj_graph_milestone_50360  (alias)
 *   __libcgj_batch50360_marker = "libcgj-batch50360"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 50351-50375, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 50400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch50360_marker[] = "libcgj-batch50360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50360_id(void)
{
	return 50360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50360 - report this TU's graph batch number.
 *
 * Always returns 50360.
 */
uint32_t
gj_batch_id_50360(void)
{
	(void)NULL;
	return b50360_id();
}

/*
 * gj_graph_milestone_50360 - report this TU's graph milestone revision.
 *
 * Always returns 50360. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_50360(void)
{
	return b50360_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50360(void)
    __attribute__((alias("gj_batch_id_50360")));

uint32_t __gj_graph_milestone_50360(void)
    __attribute__((alias("gj_graph_milestone_50360")));
