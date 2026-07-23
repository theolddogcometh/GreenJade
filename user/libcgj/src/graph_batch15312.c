/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15312: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15312(void);
 *     - Returns the compile-time graph batch number for this TU (15312).
 *   uint32_t gj_graph_milestone_15312(void);
 *     - Returns the current graph milestone revision (15312).
 *   uint32_t __gj_batch_id_15312  (alias)
 *   uint32_t __gj_graph_milestone_15312  (alias)
 *   __libcgj_batch15312_marker = "libcgj-batch15312"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15301-15325). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15400. bar3 still open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15312_marker[] = "libcgj-batch15312";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15312_id(void)
{
	return 15312u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15312 - report this TU's graph batch number.
 *
 * Always returns 15312.
 */
uint32_t
gj_batch_id_15312(void)
{
	(void)NULL;
	return b15312_id();
}

/*
 * gj_graph_milestone_15312 - report this TU's graph milestone revision.
 *
 * Always returns 15312. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15312(void)
{
	return b15312_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15312(void)
    __attribute__((alias("gj_batch_id_15312")));

uint32_t __gj_graph_milestone_15312(void)
    __attribute__((alias("gj_graph_milestone_15312")));
