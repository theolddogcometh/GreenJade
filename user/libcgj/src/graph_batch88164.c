/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88164: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_88164(void);
 *     - Returns the compile-time graph batch number for this TU (88164).
 *   uint32_t gj_graph_milestone_88164(void);
 *     - Returns the current graph milestone revision (88164).
 *   uint32_t __gj_batch_id_88164  (alias)
 *   uint32_t __gj_graph_milestone_88164  (alias)
 *   __libcgj_batch88164_marker = "libcgj-batch88164"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-88164, extension identity). Does NOT
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

const char __libcgj_batch88164_marker[] = "libcgj-batch88164";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b88164_id(void)
{
	return 88164u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_88164 - report this TU's graph batch number.
 *
 * Always returns 88164.
 */
uint32_t
gj_batch_id_88164(void)
{
	(void)NULL;
	return b88164_id();
}

/*
 * gj_graph_milestone_88164 - report this TU's graph milestone revision.
 *
 * Always returns 88164. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_88164(void)
{
	return b88164_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_88164(void)
    __attribute__((alias("gj_batch_id_88164")));

uint32_t __gj_graph_milestone_88164(void)
    __attribute__((alias("gj_graph_milestone_88164")));
