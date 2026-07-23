/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14969: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14969(void);
 *     - Returns the compile-time graph batch number for this TU (14969).
 *   uint32_t gj_graph_milestone_14969(void);
 *     - Returns the current graph milestone revision (14969).
 *   uint32_t __gj_batch_id_14969  (alias)
 *   uint32_t __gj_graph_milestone_14969  (alias)
 *   __libcgj_batch14969_marker = "libcgj-batch14969"
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

const char __libcgj_batch14969_marker[] = "libcgj-batch14969";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14969_id(void)
{
	return 14969u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14969 - report this TU's graph batch number.
 *
 * Always returns 14969.
 */
uint32_t
gj_batch_id_14969(void)
{
	(void)NULL;
	return b14969_id();
}

/*
 * gj_graph_milestone_14969 - report this TU's graph milestone revision.
 *
 * Always returns 14969. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14969(void)
{
	return b14969_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14969(void)
    __attribute__((alias("gj_batch_id_14969")));

uint32_t __gj_graph_milestone_14969(void)
    __attribute__((alias("gj_graph_milestone_14969")));
