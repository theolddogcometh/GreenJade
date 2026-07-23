/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14758: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14758(void);
 *     - Returns the compile-time graph batch number for this TU (14758).
 *   uint32_t gj_graph_milestone_14758(void);
 *     - Returns the current graph milestone revision (14758).
 *   uint32_t __gj_batch_id_14758  (alias)
 *   uint32_t __gj_graph_milestone_14758  (alias)
 *   __libcgj_batch14758_marker = "libcgj-batch14758"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 14751-14775). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14758_marker[] = "libcgj-batch14758";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14758_id(void)
{
	return 14758u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14758 - report this TU's graph batch number.
 *
 * Always returns 14758.
 */
uint32_t
gj_batch_id_14758(void)
{
	(void)NULL;
	return b14758_id();
}

/*
 * gj_graph_milestone_14758 - report this TU's graph milestone revision.
 *
 * Always returns 14758. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14758(void)
{
	return b14758_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14758(void)
    __attribute__((alias("gj_batch_id_14758")));

uint32_t __gj_graph_milestone_14758(void)
    __attribute__((alias("gj_graph_milestone_14758")));
