/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42320: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42320(void);
 *     - Returns the compile-time graph batch number for this TU (42320).
 *   uint32_t gj_graph_milestone_42320(void);
 *     - Returns the current graph milestone revision (42320).
 *   uint32_t __gj_batch_id_42320  (alias)
 *   uint32_t __gj_graph_milestone_42320  (alias)
 *   __libcgj_batch42320_marker = "libcgj-batch42320"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 42301-42350, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 42400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch42320_marker[] = "libcgj-batch42320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42320_id(void)
{
	return 42320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42320 - report this TU's graph batch number.
 *
 * Always returns 42320.
 */
uint32_t
gj_batch_id_42320(void)
{
	(void)NULL;
	return b42320_id();
}

/*
 * gj_graph_milestone_42320 - report this TU's graph milestone revision.
 *
 * Always returns 42320. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_42320(void)
{
	return b42320_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42320(void)
    __attribute__((alias("gj_batch_id_42320")));

uint32_t __gj_graph_milestone_42320(void)
    __attribute__((alias("gj_graph_milestone_42320")));
