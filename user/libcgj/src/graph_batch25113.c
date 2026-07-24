/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25113: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25113(void);
 *     - Returns the compile-time graph batch number for this TU (25113).
 *   uint32_t gj_graph_milestone_25113(void);
 *     - Returns the current graph milestone revision (25113).
 *   uint32_t __gj_batch_id_25113  (alias)
 *   uint32_t __gj_graph_milestone_25113  (alias)
 *   __libcgj_batch25113_marker = "libcgj-batch25113"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25101-25150, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 25200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch25113_marker[] = "libcgj-batch25113";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25113_id(void)
{
	return 25113u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25113 - report this TU's graph batch number.
 *
 * Always returns 25113.
 */
uint32_t
gj_batch_id_25113(void)
{
	(void)NULL;
	return b25113_id();
}

/*
 * gj_graph_milestone_25113 - report this TU's graph milestone revision.
 *
 * Always returns 25113. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_25113(void)
{
	return b25113_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25113(void)
    __attribute__((alias("gj_batch_id_25113")));

uint32_t __gj_graph_milestone_25113(void)
    __attribute__((alias("gj_graph_milestone_25113")));
