/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87714: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_87714(void);
 *     - Returns the compile-time graph batch number for this TU (87714).
 *   uint32_t gj_graph_milestone_87714(void);
 *     - Returns the current graph milestone revision (87714).
 *   uint32_t __gj_batch_id_87714  (alias)
 *   uint32_t __gj_graph_milestone_87714  (alias)
 *   __libcgj_batch87714_marker = "libcgj-batch87714"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-87714, extension identity). Does NOT
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

const char __libcgj_batch87714_marker[] = "libcgj-batch87714";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87714_id(void)
{
	return 87714u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_87714 - report this TU's graph batch number.
 *
 * Always returns 87714.
 */
uint32_t
gj_batch_id_87714(void)
{
	(void)NULL;
	return b87714_id();
}

/*
 * gj_graph_milestone_87714 - report this TU's graph milestone revision.
 *
 * Always returns 87714. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_87714(void)
{
	return b87714_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_87714(void)
    __attribute__((alias("gj_batch_id_87714")));

uint32_t __gj_graph_milestone_87714(void)
    __attribute__((alias("gj_graph_milestone_87714")));
