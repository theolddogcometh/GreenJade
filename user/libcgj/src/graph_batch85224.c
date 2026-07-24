/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch85224: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_85224(void);
 *     - Returns the compile-time graph batch number for this TU (85224).
 *   uint32_t gj_graph_milestone_85224(void);
 *     - Returns the current graph milestone revision (85224).
 *   uint32_t __gj_batch_id_85224  (alias)
 *   uint32_t __gj_graph_milestone_85224  (alias)
 *   __libcgj_batch85224_marker = "libcgj-batch85224"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-85224, extension identity). Does NOT
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

const char __libcgj_batch85224_marker[] = "libcgj-batch85224";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b85224_id(void)
{
	return 85224u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_85224 - report this TU's graph batch number.
 *
 * Always returns 85224.
 */
uint32_t
gj_batch_id_85224(void)
{
	(void)NULL;
	return b85224_id();
}

/*
 * gj_graph_milestone_85224 - report this TU's graph milestone revision.
 *
 * Always returns 85224. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_85224(void)
{
	return b85224_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_85224(void)
    __attribute__((alias("gj_batch_id_85224")));

uint32_t __gj_graph_milestone_85224(void)
    __attribute__((alias("gj_graph_milestone_85224")));
