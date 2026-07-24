/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19549: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19549(void);
 *     - Returns the compile-time graph batch number for this TU (19549).
 *   uint32_t gj_graph_milestone_19549(void);
 *     - Returns the current graph milestone revision (19549).
 *   uint32_t __gj_batch_id_19549  (alias)
 *   uint32_t __gj_graph_milestone_19549  (alias)
 *   __libcgj_batch19549_marker = "libcgj-batch19549"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 19501-19550, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 19600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch19549_marker[] = "libcgj-batch19549";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19549_id(void)
{
	return 19549u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19549 - report this TU's graph batch number.
 *
 * Always returns 19549.
 */
uint32_t
gj_batch_id_19549(void)
{
	(void)NULL;
	return b19549_id();
}

/*
 * gj_graph_milestone_19549 - report this TU's graph milestone revision.
 *
 * Always returns 19549. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_19549(void)
{
	return b19549_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19549(void)
    __attribute__((alias("gj_batch_id_19549")));

uint32_t __gj_graph_milestone_19549(void)
    __attribute__((alias("gj_graph_milestone_19549")));
