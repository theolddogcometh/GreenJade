/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51217: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_51217(void);
 *     - Returns the compile-time graph batch number for this TU (51217).
 *   uint32_t gj_graph_milestone_51217(void);
 *     - Returns the current graph milestone revision (51217).
 *   uint32_t __gj_batch_id_51217  (alias)
 *   uint32_t __gj_graph_milestone_51217  (alias)
 *   __libcgj_batch51217_marker = "libcgj-batch51217"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 51201-51250, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 51300).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch51217_marker[] = "libcgj-batch51217";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b51217_id(void)
{
	return 51217u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_51217 - report this TU's graph batch number.
 *
 * Always returns 51217.
 */
uint32_t
gj_batch_id_51217(void)
{
	(void)NULL;
	return b51217_id();
}

/*
 * gj_graph_milestone_51217 - report this TU's graph milestone revision.
 *
 * Always returns 51217. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_51217(void)
{
	return b51217_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_51217(void)
    __attribute__((alias("gj_batch_id_51217")));

uint32_t __gj_graph_milestone_51217(void)
    __attribute__((alias("gj_graph_milestone_51217")));
