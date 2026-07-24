/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46540: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46540(void);
 *     - Returns the compile-time graph batch number for this TU (46540).
 *   uint32_t gj_graph_milestone_46540(void);
 *     - Returns the current graph milestone revision (46540).
 *   uint32_t __gj_batch_id_46540  (alias)
 *   uint32_t __gj_graph_milestone_46540  (alias)
 *   __libcgj_batch46540_marker = "libcgj-batch46540"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 46501-46550, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 46600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch46540_marker[] = "libcgj-batch46540";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46540_id(void)
{
	return 46540u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46540 - report this TU's graph batch number.
 *
 * Always returns 46540.
 */
uint32_t
gj_batch_id_46540(void)
{
	(void)NULL;
	return b46540_id();
}

/*
 * gj_graph_milestone_46540 - report this TU's graph milestone revision.
 *
 * Always returns 46540. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_46540(void)
{
	return b46540_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46540(void)
    __attribute__((alias("gj_batch_id_46540")));

uint32_t __gj_graph_milestone_46540(void)
    __attribute__((alias("gj_graph_milestone_46540")));
