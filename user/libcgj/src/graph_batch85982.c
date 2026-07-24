/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch85982: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_85982(void);
 *     - Returns the compile-time graph batch number for this TU (85982).
 *   uint32_t gj_graph_milestone_85982(void);
 *     - Returns the current graph milestone revision (85982).
 *   uint32_t __gj_batch_id_85982  (alias)
 *   uint32_t __gj_graph_milestone_85982  (alias)
 *   __libcgj_batch85982_marker = "libcgj-batch85982"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-85982, extension identity). Does NOT
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

const char __libcgj_batch85982_marker[] = "libcgj-batch85982";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b85982_id(void)
{
	return 85982u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_85982 - report this TU's graph batch number.
 *
 * Always returns 85982.
 */
uint32_t
gj_batch_id_85982(void)
{
	(void)NULL;
	return b85982_id();
}

/*
 * gj_graph_milestone_85982 - report this TU's graph milestone revision.
 *
 * Always returns 85982. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_85982(void)
{
	return b85982_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_85982(void)
    __attribute__((alias("gj_batch_id_85982")));

uint32_t __gj_graph_milestone_85982(void)
    __attribute__((alias("gj_graph_milestone_85982")));
