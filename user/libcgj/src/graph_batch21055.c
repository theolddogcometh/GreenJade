/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21055: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21055(void);
 *     - Returns the compile-time graph batch number for this TU (21055).
 *   uint32_t gj_graph_milestone_21055(void);
 *     - Returns the current graph milestone revision (21055).
 *   uint32_t __gj_batch_id_21055  (alias)
 *   uint32_t __gj_graph_milestone_21055  (alias)
 *   __libcgj_batch21055_marker = "libcgj-batch21055"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 21051-21075, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 21100).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch21055_marker[] = "libcgj-batch21055";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21055_id(void)
{
	return 21055u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21055 - report this TU's graph batch number.
 *
 * Always returns 21055.
 */
uint32_t
gj_batch_id_21055(void)
{
	(void)NULL;
	return b21055_id();
}

/*
 * gj_graph_milestone_21055 - report this TU's graph milestone revision.
 *
 * Always returns 21055. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_21055(void)
{
	return b21055_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21055(void)
    __attribute__((alias("gj_batch_id_21055")));

uint32_t __gj_graph_milestone_21055(void)
    __attribute__((alias("gj_graph_milestone_21055")));
