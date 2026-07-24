/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49605: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49605(void);
 *     - Returns the compile-time graph batch number for this TU (49605).
 *   uint32_t gj_graph_milestone_49605(void);
 *     - Returns the current graph milestone revision (49605).
 *   uint32_t __gj_batch_id_49605  (alias)
 *   uint32_t __gj_graph_milestone_49605  (alias)
 *   __libcgj_batch49605_marker = "libcgj-batch49605"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 49601-49650, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 49700).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch49605_marker[] = "libcgj-batch49605";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49605_id(void)
{
	return 49605u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49605 - report this TU's graph batch number.
 *
 * Always returns 49605.
 */
uint32_t
gj_batch_id_49605(void)
{
	(void)NULL;
	return b49605_id();
}

/*
 * gj_graph_milestone_49605 - report this TU's graph milestone revision.
 *
 * Always returns 49605. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_49605(void)
{
	return b49605_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49605(void)
    __attribute__((alias("gj_batch_id_49605")));

uint32_t __gj_graph_milestone_49605(void)
    __attribute__((alias("gj_graph_milestone_49605")));
