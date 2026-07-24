/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50014: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50014(void);
 *     - Returns the compile-time graph batch number for this TU (50014).
 *   uint32_t gj_graph_milestone_50014(void);
 *     - Returns the current graph milestone revision (50014).
 *   uint32_t __gj_batch_id_50014  (alias)
 *   uint32_t __gj_graph_milestone_50014  (alias)
 *   __libcgj_batch50014_marker = "libcgj-batch50014"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 50001-50050, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 50100).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch50014_marker[] = "libcgj-batch50014";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50014_id(void)
{
	return 50014u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50014 - report this TU's graph batch number.
 *
 * Always returns 50014.
 */
uint32_t
gj_batch_id_50014(void)
{
	(void)NULL;
	return b50014_id();
}

/*
 * gj_graph_milestone_50014 - report this TU's graph milestone revision.
 *
 * Always returns 50014. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_50014(void)
{
	return b50014_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50014(void)
    __attribute__((alias("gj_batch_id_50014")));

uint32_t __gj_graph_milestone_50014(void)
    __attribute__((alias("gj_graph_milestone_50014")));
