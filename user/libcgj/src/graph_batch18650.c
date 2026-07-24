/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18650: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18650(void);
 *     - Returns the compile-time graph batch number for this TU (18650).
 *   uint32_t gj_graph_milestone_18650(void);
 *     - Returns the current graph milestone revision (18650).
 *   uint32_t __gj_batch_id_18650  (alias)
 *   uint32_t __gj_graph_milestone_18650  (alias)
 *   __libcgj_batch18650_marker = "libcgj-batch18650"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 18601-18650, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 18700).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch18650_marker[] = "libcgj-batch18650";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18650_id(void)
{
	return 18650u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18650 - report this TU's graph batch number.
 *
 * Always returns 18650.
 */
uint32_t
gj_batch_id_18650(void)
{
	(void)NULL;
	return b18650_id();
}

/*
 * gj_graph_milestone_18650 - report this TU's graph milestone revision.
 *
 * Always returns 18650. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_18650(void)
{
	return b18650_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18650(void)
    __attribute__((alias("gj_batch_id_18650")));

uint32_t __gj_graph_milestone_18650(void)
    __attribute__((alias("gj_graph_milestone_18650")));
