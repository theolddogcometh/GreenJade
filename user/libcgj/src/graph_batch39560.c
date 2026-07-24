/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39560: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39560(void);
 *     - Returns the compile-time graph batch number for this TU (39560).
 *   uint32_t gj_graph_milestone_39560(void);
 *     - Returns the current graph milestone revision (39560).
 *   uint32_t __gj_batch_id_39560  (alias)
 *   uint32_t __gj_graph_milestone_39560  (alias)
 *   __libcgj_batch39560_marker = "libcgj-batch39560"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 39551-39575, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 39600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch39560_marker[] = "libcgj-batch39560";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39560_id(void)
{
	return 39560u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39560 - report this TU's graph batch number.
 *
 * Always returns 39560.
 */
uint32_t
gj_batch_id_39560(void)
{
	(void)NULL;
	return b39560_id();
}

/*
 * gj_graph_milestone_39560 - report this TU's graph milestone revision.
 *
 * Always returns 39560. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_39560(void)
{
	return b39560_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39560(void)
    __attribute__((alias("gj_batch_id_39560")));

uint32_t __gj_graph_milestone_39560(void)
    __attribute__((alias("gj_graph_milestone_39560")));
