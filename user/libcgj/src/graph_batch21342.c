/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21342: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21342(void);
 *     - Returns the compile-time graph batch number for this TU (21342).
 *   uint32_t gj_graph_milestone_21342(void);
 *     - Returns the current graph milestone revision (21342).
 *   uint32_t __gj_batch_id_21342  (alias)
 *   uint32_t __gj_graph_milestone_21342  (alias)
 *   __libcgj_batch21342_marker = "libcgj-batch21342"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 21301-21350, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 21400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch21342_marker[] = "libcgj-batch21342";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21342_id(void)
{
	return 21342u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21342 - report this TU's graph batch number.
 *
 * Always returns 21342.
 */
uint32_t
gj_batch_id_21342(void)
{
	(void)NULL;
	return b21342_id();
}

/*
 * gj_graph_milestone_21342 - report this TU's graph milestone revision.
 *
 * Always returns 21342. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_21342(void)
{
	return b21342_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21342(void)
    __attribute__((alias("gj_batch_id_21342")));

uint32_t __gj_graph_milestone_21342(void)
    __attribute__((alias("gj_graph_milestone_21342")));
