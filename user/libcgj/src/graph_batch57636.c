/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch57636: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_57636(void);
 *     - Returns the compile-time graph batch number for this TU (57636).
 *   uint32_t gj_graph_milestone_57636(void);
 *     - Returns the current graph milestone revision (57636).
 *   uint32_t __gj_batch_id_57636  (alias)
 *   uint32_t __gj_graph_milestone_57636  (alias)
 *   __libcgj_batch57636_marker = "libcgj-batch57636"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-57636, extension identity). Does NOT
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

const char __libcgj_batch57636_marker[] = "libcgj-batch57636";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b57636_id(void)
{
	return 57636u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_57636 - report this TU's graph batch number.
 *
 * Always returns 57636.
 */
uint32_t
gj_batch_id_57636(void)
{
	(void)NULL;
	return b57636_id();
}

/*
 * gj_graph_milestone_57636 - report this TU's graph milestone revision.
 *
 * Always returns 57636. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_57636(void)
{
	return b57636_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_57636(void)
    __attribute__((alias("gj_batch_id_57636")));

uint32_t __gj_graph_milestone_57636(void)
    __attribute__((alias("gj_graph_milestone_57636")));
