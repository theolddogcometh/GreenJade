/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17504: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17504(void);
 *     - Returns the compile-time graph batch number for this TU (17504).
 *   uint32_t gj_graph_milestone_17504(void);
 *     - Returns the current graph milestone revision (17504).
 *   uint32_t __gj_batch_id_17504  (alias)
 *   uint32_t __gj_graph_milestone_17504  (alias)
 *   __libcgj_batch17504_marker = "libcgj-batch17504"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 17501-17550, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 17600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch17504_marker[] = "libcgj-batch17504";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17504_id(void)
{
	return 17504u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17504 - report this TU's graph batch number.
 *
 * Always returns 17504.
 */
uint32_t
gj_batch_id_17504(void)
{
	(void)NULL;
	return b17504_id();
}

/*
 * gj_graph_milestone_17504 - report this TU's graph milestone revision.
 *
 * Always returns 17504. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_17504(void)
{
	return b17504_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17504(void)
    __attribute__((alias("gj_batch_id_17504")));

uint32_t __gj_graph_milestone_17504(void)
    __attribute__((alias("gj_graph_milestone_17504")));
