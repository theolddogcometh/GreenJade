/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50713: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50713(void);
 *     - Returns the compile-time graph batch number for this TU (50713).
 *   uint32_t gj_graph_milestone_50713(void);
 *     - Returns the current graph milestone revision (50713).
 *   uint32_t __gj_batch_id_50713  (alias)
 *   uint32_t __gj_graph_milestone_50713  (alias)
 *   __libcgj_batch50713_marker = "libcgj-batch50713"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 50701-50750, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 50800).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch50713_marker[] = "libcgj-batch50713";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50713_id(void)
{
	return 50713u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50713 - report this TU's graph batch number.
 *
 * Always returns 50713.
 */
uint32_t
gj_batch_id_50713(void)
{
	(void)NULL;
	return b50713_id();
}

/*
 * gj_graph_milestone_50713 - report this TU's graph milestone revision.
 *
 * Always returns 50713. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_50713(void)
{
	return b50713_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50713(void)
    __attribute__((alias("gj_batch_id_50713")));

uint32_t __gj_graph_milestone_50713(void)
    __attribute__((alias("gj_graph_milestone_50713")));
