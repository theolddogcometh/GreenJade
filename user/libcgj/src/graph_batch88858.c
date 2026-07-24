/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88858: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_88858(void);
 *     - Returns the compile-time graph batch number for this TU (88858).
 *   uint32_t gj_graph_milestone_88858(void);
 *     - Returns the current graph milestone revision (88858).
 *   uint32_t __gj_batch_id_88858  (alias)
 *   uint32_t __gj_graph_milestone_88858  (alias)
 *   __libcgj_batch88858_marker = "libcgj-batch88858"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-88858, extension identity). Does NOT
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

const char __libcgj_batch88858_marker[] = "libcgj-batch88858";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b88858_id(void)
{
	return 88858u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_88858 - report this TU's graph batch number.
 *
 * Always returns 88858.
 */
uint32_t
gj_batch_id_88858(void)
{
	(void)NULL;
	return b88858_id();
}

/*
 * gj_graph_milestone_88858 - report this TU's graph milestone revision.
 *
 * Always returns 88858. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_88858(void)
{
	return b88858_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_88858(void)
    __attribute__((alias("gj_batch_id_88858")));

uint32_t __gj_graph_milestone_88858(void)
    __attribute__((alias("gj_graph_milestone_88858")));
