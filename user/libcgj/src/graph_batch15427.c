/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15427: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15427(void);
 *     - Returns the compile-time graph batch number for this TU (15427).
 *   uint32_t gj_graph_milestone_15427(void);
 *     - Returns the current graph milestone revision (15427).
 *   uint32_t __gj_batch_id_15427  (alias)
 *   uint32_t __gj_graph_milestone_15427  (alias)
 *   __libcgj_batch15427_marker = "libcgj-batch15427"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15426-15450). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15500. bar3 open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15427_marker[] = "libcgj-batch15427";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15427_id(void)
{
	return 15427u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15427 - report this TU's graph batch number.
 *
 * Always returns 15427.
 */
uint32_t
gj_batch_id_15427(void)
{
	(void)NULL;
	return b15427_id();
}

/*
 * gj_graph_milestone_15427 - report this TU's graph milestone revision.
 *
 * Always returns 15427. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15427(void)
{
	return b15427_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15427(void)
    __attribute__((alias("gj_batch_id_15427")));

uint32_t __gj_graph_milestone_15427(void)
    __attribute__((alias("gj_graph_milestone_15427")));
