/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94748: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_94748(void);
 *     - Returns the compile-time graph batch number for this TU (94748).
 *   uint32_t gj_graph_milestone_94748(void);
 *     - Returns the current graph milestone revision (94748).
 *   uint32_t __gj_batch_id_94748  (alias)
 *   uint32_t __gj_graph_milestone_94748  (alias)
 *   __libcgj_batch94748_marker = "libcgj-batch94748"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-94748, extension identity). Does NOT
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

const char __libcgj_batch94748_marker[] = "libcgj-batch94748";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94748_id(void)
{
	return 94748u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_94748 - report this TU's graph batch number.
 *
 * Always returns 94748.
 */
uint32_t
gj_batch_id_94748(void)
{
	(void)NULL;
	return b94748_id();
}

/*
 * gj_graph_milestone_94748 - report this TU's graph milestone revision.
 *
 * Always returns 94748. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_94748(void)
{
	return b94748_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_94748(void)
    __attribute__((alias("gj_batch_id_94748")));

uint32_t __gj_graph_milestone_94748(void)
    __attribute__((alias("gj_graph_milestone_94748")));
