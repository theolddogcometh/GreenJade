/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92157: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_92157(void);
 *     - Returns the compile-time graph batch number for this TU (92157).
 *   uint32_t gj_graph_milestone_92157(void);
 *     - Returns the current graph milestone revision (92157).
 *   uint32_t __gj_batch_id_92157  (alias)
 *   uint32_t __gj_graph_milestone_92157  (alias)
 *   __libcgj_batch92157_marker = "libcgj-batch92157"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-92157, extension identity). Does NOT
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

const char __libcgj_batch92157_marker[] = "libcgj-batch92157";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b92157_id(void)
{
	return 92157u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_92157 - report this TU's graph batch number.
 *
 * Always returns 92157.
 */
uint32_t
gj_batch_id_92157(void)
{
	(void)NULL;
	return b92157_id();
}

/*
 * gj_graph_milestone_92157 - report this TU's graph milestone revision.
 *
 * Always returns 92157. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_92157(void)
{
	return b92157_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_92157(void)
    __attribute__((alias("gj_batch_id_92157")));

uint32_t __gj_graph_milestone_92157(void)
    __attribute__((alias("gj_graph_milestone_92157")));
