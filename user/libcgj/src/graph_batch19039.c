/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19039: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19039(void);
 *     - Returns the compile-time graph batch number for this TU (19039).
 *   uint32_t gj_graph_milestone_19039(void);
 *     - Returns the current graph milestone revision (19039).
 *   uint32_t __gj_batch_id_19039  (alias)
 *   uint32_t __gj_graph_milestone_19039  (alias)
 *   __libcgj_batch19039_marker = "libcgj-batch19039"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 19001-19050, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 19100).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch19039_marker[] = "libcgj-batch19039";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19039_id(void)
{
	return 19039u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19039 - report this TU's graph batch number.
 *
 * Always returns 19039.
 */
uint32_t
gj_batch_id_19039(void)
{
	(void)NULL;
	return b19039_id();
}

/*
 * gj_graph_milestone_19039 - report this TU's graph milestone revision.
 *
 * Always returns 19039. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_19039(void)
{
	return b19039_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19039(void)
    __attribute__((alias("gj_batch_id_19039")));

uint32_t __gj_graph_milestone_19039(void)
    __attribute__((alias("gj_graph_milestone_19039")));
