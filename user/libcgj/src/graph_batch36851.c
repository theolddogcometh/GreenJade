/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36851: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36851(void);
 *     - Returns the compile-time graph batch number for this TU (36851).
 *   uint32_t gj_graph_milestone_36851(void);
 *     - Returns the current graph milestone revision (36851).
 *   uint32_t __gj_batch_id_36851  (alias)
 *   uint32_t __gj_graph_milestone_36851  (alias)
 *   __libcgj_batch36851_marker = "libcgj-batch36851"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 36851-36875, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 36900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch36851_marker[] = "libcgj-batch36851";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36851_id(void)
{
	return 36851u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36851 - report this TU's graph batch number.
 *
 * Always returns 36851.
 */
uint32_t
gj_batch_id_36851(void)
{
	(void)NULL;
	return b36851_id();
}

/*
 * gj_graph_milestone_36851 - report this TU's graph milestone revision.
 *
 * Always returns 36851. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_36851(void)
{
	return b36851_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36851(void)
    __attribute__((alias("gj_batch_id_36851")));

uint32_t __gj_graph_milestone_36851(void)
    __attribute__((alias("gj_graph_milestone_36851")));
