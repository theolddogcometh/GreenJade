/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14839: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14839(void);
 *     - Returns the compile-time graph batch number for this TU (14839).
 *   uint32_t gj_graph_milestone_14839(void);
 *     - Returns the current graph milestone revision (14839).
 *   uint32_t __gj_batch_id_14839  (alias)
 *   uint32_t __gj_graph_milestone_14839  (alias)
 *   __libcgj_batch14839_marker = "libcgj-batch14839"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 14801-14850). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14839_marker[] = "libcgj-batch14839";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14839_id(void)
{
	return 14839u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14839 - report this TU's graph batch number.
 *
 * Always returns 14839.
 */
uint32_t
gj_batch_id_14839(void)
{
	(void)NULL;
	return b14839_id();
}

/*
 * gj_graph_milestone_14839 - report this TU's graph milestone revision.
 *
 * Always returns 14839. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14839(void)
{
	return b14839_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14839(void)
    __attribute__((alias("gj_batch_id_14839")));

uint32_t __gj_graph_milestone_14839(void)
    __attribute__((alias("gj_graph_milestone_14839")));
