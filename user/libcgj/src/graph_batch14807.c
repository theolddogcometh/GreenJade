/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14807: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14807(void);
 *     - Returns the compile-time graph batch number for this TU (14807).
 *   uint32_t gj_graph_milestone_14807(void);
 *     - Returns the current graph milestone revision (14807).
 *   uint32_t __gj_batch_id_14807  (alias)
 *   uint32_t __gj_graph_milestone_14807  (alias)
 *   __libcgj_batch14807_marker = "libcgj-batch14807"
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

const char __libcgj_batch14807_marker[] = "libcgj-batch14807";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14807_id(void)
{
	return 14807u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14807 - report this TU's graph batch number.
 *
 * Always returns 14807.
 */
uint32_t
gj_batch_id_14807(void)
{
	(void)NULL;
	return b14807_id();
}

/*
 * gj_graph_milestone_14807 - report this TU's graph milestone revision.
 *
 * Always returns 14807. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14807(void)
{
	return b14807_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14807(void)
    __attribute__((alias("gj_batch_id_14807")));

uint32_t __gj_graph_milestone_14807(void)
    __attribute__((alias("gj_graph_milestone_14807")));
