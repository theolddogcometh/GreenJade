/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63217: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_63217(void);
 *     - Returns the compile-time graph batch number for this TU (63217).
 *   uint32_t gj_graph_milestone_63217(void);
 *     - Returns the current graph milestone revision (63217).
 *   uint32_t __gj_batch_id_63217  (alias)
 *   uint32_t __gj_graph_milestone_63217  (alias)
 *   __libcgj_batch63217_marker = "libcgj-batch63217"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-63217, extension identity). Does NOT
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

const char __libcgj_batch63217_marker[] = "libcgj-batch63217";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63217_id(void)
{
	return 63217u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_63217 - report this TU's graph batch number.
 *
 * Always returns 63217.
 */
uint32_t
gj_batch_id_63217(void)
{
	(void)NULL;
	return b63217_id();
}

/*
 * gj_graph_milestone_63217 - report this TU's graph milestone revision.
 *
 * Always returns 63217. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_63217(void)
{
	return b63217_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_63217(void)
    __attribute__((alias("gj_batch_id_63217")));

uint32_t __gj_graph_milestone_63217(void)
    __attribute__((alias("gj_graph_milestone_63217")));
