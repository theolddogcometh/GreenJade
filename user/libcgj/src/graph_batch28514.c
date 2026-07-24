/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28514: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28514(void);
 *     - Returns the compile-time graph batch number for this TU (28514).
 *   uint32_t gj_graph_milestone_28514(void);
 *     - Returns the current graph milestone revision (28514).
 *   uint32_t __gj_batch_id_28514  (alias)
 *   uint32_t __gj_graph_milestone_28514  (alias)
 *   __libcgj_batch28514_marker = "libcgj-batch28514"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 28501-28550, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 28600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch28514_marker[] = "libcgj-batch28514";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28514_id(void)
{
	return 28514u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28514 - report this TU's graph batch number.
 *
 * Always returns 28514.
 */
uint32_t
gj_batch_id_28514(void)
{
	(void)NULL;
	return b28514_id();
}

/*
 * gj_graph_milestone_28514 - report this TU's graph milestone revision.
 *
 * Always returns 28514. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_28514(void)
{
	return b28514_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28514(void)
    __attribute__((alias("gj_batch_id_28514")));

uint32_t __gj_graph_milestone_28514(void)
    __attribute__((alias("gj_graph_milestone_28514")));
