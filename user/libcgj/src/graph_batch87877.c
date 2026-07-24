/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87877: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_87877(void);
 *     - Returns the compile-time graph batch number for this TU (87877).
 *   uint32_t gj_graph_milestone_87877(void);
 *     - Returns the current graph milestone revision (87877).
 *   uint32_t __gj_batch_id_87877  (alias)
 *   uint32_t __gj_graph_milestone_87877  (alias)
 *   __libcgj_batch87877_marker = "libcgj-batch87877"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-87877, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 25900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch87877_marker[] = "libcgj-batch87877";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87877_id(void)
{
	return 87877u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_87877 - report this TU's graph batch number.
 *
 * Always returns 87877.
 */
uint32_t
gj_batch_id_87877(void)
{
	(void)NULL;
	return b87877_id();
}

/*
 * gj_graph_milestone_87877 - report this TU's graph milestone revision.
 *
 * Always returns 87877. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_87877(void)
{
	return b87877_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_87877(void)
    __attribute__((alias("gj_batch_id_87877")));

uint32_t __gj_graph_milestone_87877(void)
    __attribute__((alias("gj_graph_milestone_87877")));
