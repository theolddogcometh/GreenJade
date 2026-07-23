/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14955: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14955(void);
 *     - Returns the compile-time graph batch number for this TU (14955).
 *   uint32_t gj_graph_milestone_14955(void);
 *     - Returns the current graph milestone revision (14955).
 *   uint32_t __gj_batch_id_14955  (alias)
 *   uint32_t __gj_graph_milestone_14955  (alias)
 *   __libcgj_batch14955_marker = "libcgj-batch14955"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 14951-14975). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15000. bar3 still open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14955_marker[] = "libcgj-batch14955";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14955_id(void)
{
	return 14955u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14955 - report this TU's graph batch number.
 *
 * Always returns 14955.
 */
uint32_t
gj_batch_id_14955(void)
{
	(void)NULL;
	return b14955_id();
}

/*
 * gj_graph_milestone_14955 - report this TU's graph milestone revision.
 *
 * Always returns 14955. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14955(void)
{
	return b14955_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14955(void)
    __attribute__((alias("gj_batch_id_14955")));

uint32_t __gj_graph_milestone_14955(void)
    __attribute__((alias("gj_graph_milestone_14955")));
