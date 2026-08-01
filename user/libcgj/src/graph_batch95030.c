/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95030: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95030(void);
 *     - Returns the compile-time graph batch number for this TU (95030).
 *   uint32_t gj_graph_milestone_95030(void);
 *     - Returns the current graph milestone revision (95030).
 *   uint32_t __gj_batch_id_95030  (alias)
 *   uint32_t __gj_graph_milestone_95030  (alias)
 *   __libcgj_batch95030_marker = "libcgj-batch95030"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-95030, extension identity). Does NOT
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

const char __libcgj_batch95030_marker[] = "libcgj-batch95030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95030_id(void)
{
	return 95030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95030 - report this TU's graph batch number.
 *
 * Always returns 95030.
 */
uint32_t
gj_batch_id_95030(void)
{
	(void)NULL;
	return b95030_id();
}

/*
 * gj_graph_milestone_95030 - report this TU's graph milestone revision.
 *
 * Always returns 95030. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_95030(void)
{
	return b95030_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_95030(void)
    __attribute__((alias("gj_batch_id_95030")));

uint32_t __gj_graph_milestone_95030(void)
    __attribute__((alias("gj_graph_milestone_95030")));
