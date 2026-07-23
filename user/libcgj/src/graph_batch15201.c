/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15201: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15201(void);
 *     - Returns the compile-time graph batch number for this TU (15201).
 *   uint32_t gj_graph_milestone_15201(void);
 *     - Returns the current graph milestone revision (15201).
 *   uint32_t __gj_batch_id_15201  (alias)
 *   uint32_t __gj_graph_milestone_15201  (alias)
 *   __libcgj_batch15201_marker = "libcgj-batch15201"
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

const char __libcgj_batch15201_marker[] = "libcgj-batch15201";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15201_id(void)
{
	return 15201u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15201 - report this TU's graph batch number.
 *
 * Always returns 15201.
 */
uint32_t
gj_batch_id_15201(void)
{
	(void)NULL;
	return b15201_id();
}

/*
 * gj_graph_milestone_15201 - report this TU's graph milestone revision.
 *
 * Always returns 15201. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15201(void)
{
	return b15201_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15201(void)
    __attribute__((alias("gj_batch_id_15201")));

uint32_t __gj_graph_milestone_15201(void)
    __attribute__((alias("gj_graph_milestone_15201")));
