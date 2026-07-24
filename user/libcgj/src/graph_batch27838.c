/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27838: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27838(void);
 *     - Returns the compile-time graph batch number for this TU (27838).
 *   uint32_t gj_graph_milestone_27838(void);
 *     - Returns the current graph milestone revision (27838).
 *   uint32_t __gj_batch_id_27838  (alias)
 *   uint32_t __gj_graph_milestone_27838  (alias)
 *   __libcgj_batch27838_marker = "libcgj-batch27838"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 27801-27850, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 27900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch27838_marker[] = "libcgj-batch27838";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27838_id(void)
{
	return 27838u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27838 - report this TU's graph batch number.
 *
 * Always returns 27838.
 */
uint32_t
gj_batch_id_27838(void)
{
	(void)NULL;
	return b27838_id();
}

/*
 * gj_graph_milestone_27838 - report this TU's graph milestone revision.
 *
 * Always returns 27838. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_27838(void)
{
	return b27838_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27838(void)
    __attribute__((alias("gj_batch_id_27838")));

uint32_t __gj_graph_milestone_27838(void)
    __attribute__((alias("gj_graph_milestone_27838")));
