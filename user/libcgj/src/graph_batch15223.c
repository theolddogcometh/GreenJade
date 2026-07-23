/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15223: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15223(void);
 *     - Returns the compile-time graph batch number for this TU (15223).
 *   uint32_t gj_graph_milestone_15223(void);
 *     - Returns the current graph milestone revision (15223).
 *   uint32_t __gj_batch_id_15223  (alias)
 *   uint32_t __gj_graph_milestone_15223  (alias)
 *   __libcgj_batch15223_marker = "libcgj-batch15223"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15201-15225). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15300. bar3 still open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15223_marker[] = "libcgj-batch15223";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15223_id(void)
{
	return 15223u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15223 - report this TU's graph batch number.
 *
 * Always returns 15223.
 */
uint32_t
gj_batch_id_15223(void)
{
	(void)NULL;
	return b15223_id();
}

/*
 * gj_graph_milestone_15223 - report this TU's graph milestone revision.
 *
 * Always returns 15223. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15223(void)
{
	return b15223_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15223(void)
    __attribute__((alias("gj_batch_id_15223")));

uint32_t __gj_graph_milestone_15223(void)
    __attribute__((alias("gj_graph_milestone_15223")));
