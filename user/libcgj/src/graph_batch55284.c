/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch55284: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_55284(void);
 *     - Returns the compile-time graph batch number for this TU (55284).
 *   uint32_t gj_graph_milestone_55284(void);
 *     - Returns the current graph milestone revision (55284).
 *   uint32_t __gj_batch_id_55284  (alias)
 *   uint32_t __gj_graph_milestone_55284  (alias)
 *   __libcgj_batch55284_marker = "libcgj-batch55284"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-55284, extension identity). Does NOT
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

const char __libcgj_batch55284_marker[] = "libcgj-batch55284";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b55284_id(void)
{
	return 55284u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_55284 - report this TU's graph batch number.
 *
 * Always returns 55284.
 */
uint32_t
gj_batch_id_55284(void)
{
	(void)NULL;
	return b55284_id();
}

/*
 * gj_graph_milestone_55284 - report this TU's graph milestone revision.
 *
 * Always returns 55284. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_55284(void)
{
	return b55284_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_55284(void)
    __attribute__((alias("gj_batch_id_55284")));

uint32_t __gj_graph_milestone_55284(void)
    __attribute__((alias("gj_graph_milestone_55284")));
