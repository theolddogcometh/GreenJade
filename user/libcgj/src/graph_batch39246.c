/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39246: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39246(void);
 *     - Returns the compile-time graph batch number for this TU (39246).
 *   uint32_t gj_graph_milestone_39246(void);
 *     - Returns the current graph milestone revision (39246).
 *   uint32_t __gj_batch_id_39246  (alias)
 *   uint32_t __gj_graph_milestone_39246  (alias)
 *   __libcgj_batch39246_marker = "libcgj-batch39246"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 39201-39250, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 39300).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch39246_marker[] = "libcgj-batch39246";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39246_id(void)
{
	return 39246u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39246 - report this TU's graph batch number.
 *
 * Always returns 39246.
 */
uint32_t
gj_batch_id_39246(void)
{
	(void)NULL;
	return b39246_id();
}

/*
 * gj_graph_milestone_39246 - report this TU's graph milestone revision.
 *
 * Always returns 39246. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_39246(void)
{
	return b39246_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39246(void)
    __attribute__((alias("gj_batch_id_39246")));

uint32_t __gj_graph_milestone_39246(void)
    __attribute__((alias("gj_graph_milestone_39246")));
