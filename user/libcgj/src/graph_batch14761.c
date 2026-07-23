/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14761: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14761(void);
 *     - Returns the compile-time graph batch number for this TU (14761).
 *   uint32_t gj_graph_milestone_14761(void);
 *     - Returns the current graph milestone revision (14761).
 *   uint32_t __gj_batch_id_14761  (alias)
 *   uint32_t __gj_graph_milestone_14761  (alias)
 *   __libcgj_batch14761_marker = "libcgj-batch14761"
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

const char __libcgj_batch14761_marker[] = "libcgj-batch14761";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14761_id(void)
{
	return 14761u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14761 - report this TU's graph batch number.
 *
 * Always returns 14761.
 */
uint32_t
gj_batch_id_14761(void)
{
	(void)NULL;
	return b14761_id();
}

/*
 * gj_graph_milestone_14761 - report this TU's graph milestone revision.
 *
 * Always returns 14761. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14761(void)
{
	return b14761_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14761(void)
    __attribute__((alias("gj_batch_id_14761")));

uint32_t __gj_graph_milestone_14761(void)
    __attribute__((alias("gj_graph_milestone_14761")));
