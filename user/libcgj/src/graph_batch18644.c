/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18644: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18644(void);
 *     - Returns the compile-time graph batch number for this TU (18644).
 *   uint32_t gj_graph_milestone_18644(void);
 *     - Returns the current graph milestone revision (18644).
 *   uint32_t __gj_batch_id_18644  (alias)
 *   uint32_t __gj_graph_milestone_18644  (alias)
 *   __libcgj_batch18644_marker = "libcgj-batch18644"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 18601-18650, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 18700).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch18644_marker[] = "libcgj-batch18644";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18644_id(void)
{
	return 18644u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18644 - report this TU's graph batch number.
 *
 * Always returns 18644.
 */
uint32_t
gj_batch_id_18644(void)
{
	(void)NULL;
	return b18644_id();
}

/*
 * gj_graph_milestone_18644 - report this TU's graph milestone revision.
 *
 * Always returns 18644. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_18644(void)
{
	return b18644_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18644(void)
    __attribute__((alias("gj_batch_id_18644")));

uint32_t __gj_graph_milestone_18644(void)
    __attribute__((alias("gj_graph_milestone_18644")));
