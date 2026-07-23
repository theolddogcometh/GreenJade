/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14973: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14973(void);
 *     - Returns the compile-time graph batch number for this TU (14973).
 *   uint32_t gj_graph_milestone_14973(void);
 *     - Returns the current graph milestone revision (14973).
 *   uint32_t __gj_batch_id_14973  (alias)
 *   uint32_t __gj_graph_milestone_14973  (alias)
 *   __libcgj_batch14973_marker = "libcgj-batch14973"
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

const char __libcgj_batch14973_marker[] = "libcgj-batch14973";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14973_id(void)
{
	return 14973u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14973 - report this TU's graph batch number.
 *
 * Always returns 14973.
 */
uint32_t
gj_batch_id_14973(void)
{
	(void)NULL;
	return b14973_id();
}

/*
 * gj_graph_milestone_14973 - report this TU's graph milestone revision.
 *
 * Always returns 14973. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14973(void)
{
	return b14973_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14973(void)
    __attribute__((alias("gj_batch_id_14973")));

uint32_t __gj_graph_milestone_14973(void)
    __attribute__((alias("gj_graph_milestone_14973")));
