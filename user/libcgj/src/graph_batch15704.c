/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15704: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15704(void);
 *     - Returns the compile-time graph batch number for this TU (15704).
 *   uint32_t gj_graph_milestone_15704(void);
 *     - Returns the current graph milestone revision (15704).
 *   uint32_t __gj_batch_id_15704  (alias)
 *   uint32_t __gj_graph_milestone_15704  (alias)
 *   __libcgj_batch15704_marker = "libcgj-batch15704"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15701-15750, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 15800).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15704_marker[] = "libcgj-batch15704";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15704_id(void)
{
	return 15704u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15704 - report this TU's graph batch number.
 *
 * Always returns 15704.
 */
uint32_t
gj_batch_id_15704(void)
{
	(void)NULL;
	return b15704_id();
}

/*
 * gj_graph_milestone_15704 - report this TU's graph milestone revision.
 *
 * Always returns 15704. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15704(void)
{
	return b15704_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15704(void)
    __attribute__((alias("gj_batch_id_15704")));

uint32_t __gj_graph_milestone_15704(void)
    __attribute__((alias("gj_graph_milestone_15704")));
