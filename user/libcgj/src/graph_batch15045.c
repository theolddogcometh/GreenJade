/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15045: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15045(void);
 *     - Returns the compile-time graph batch number for this TU (15045).
 *   uint32_t gj_graph_milestone_15045(void);
 *     - Returns the current graph milestone revision (15045).
 *   uint32_t __gj_batch_id_15045  (alias)
 *   uint32_t __gj_graph_milestone_15045  (alias)
 *   __libcgj_batch15045_marker = "libcgj-batch15045"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15026-15050). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15100. bar3 still open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15045_marker[] = "libcgj-batch15045";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15045_id(void)
{
	return 15045u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15045 - report this TU's graph batch number.
 *
 * Always returns 15045.
 */
uint32_t
gj_batch_id_15045(void)
{
	(void)NULL;
	return b15045_id();
}

/*
 * gj_graph_milestone_15045 - report this TU's graph milestone revision.
 *
 * Always returns 15045. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15045(void)
{
	return b15045_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15045(void)
    __attribute__((alias("gj_batch_id_15045")));

uint32_t __gj_graph_milestone_15045(void)
    __attribute__((alias("gj_graph_milestone_15045")));
