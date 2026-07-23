/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14769: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14769(void);
 *     - Returns the compile-time graph batch number for this TU (14769).
 *   uint32_t gj_graph_milestone_14769(void);
 *     - Returns the current graph milestone revision (14769).
 *   uint32_t __gj_batch_id_14769  (alias)
 *   uint32_t __gj_graph_milestone_14769  (alias)
 *   __libcgj_batch14769_marker = "libcgj-batch14769"
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

const char __libcgj_batch14769_marker[] = "libcgj-batch14769";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14769_id(void)
{
	return 14769u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14769 - report this TU's graph batch number.
 *
 * Always returns 14769.
 */
uint32_t
gj_batch_id_14769(void)
{
	(void)NULL;
	return b14769_id();
}

/*
 * gj_graph_milestone_14769 - report this TU's graph milestone revision.
 *
 * Always returns 14769. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14769(void)
{
	return b14769_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14769(void)
    __attribute__((alias("gj_batch_id_14769")));

uint32_t __gj_graph_milestone_14769(void)
    __attribute__((alias("gj_graph_milestone_14769")));
