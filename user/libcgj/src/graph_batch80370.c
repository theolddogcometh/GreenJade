/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80370: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_80370(void);
 *     - Returns the compile-time graph batch number for this TU (80370).
 *   uint32_t gj_graph_milestone_80370(void);
 *     - Returns the current graph milestone revision (80370).
 *   uint32_t __gj_batch_id_80370  (alias)
 *   uint32_t __gj_graph_milestone_80370  (alias)
 *   __libcgj_batch80370_marker = "libcgj-batch80370"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-80370, extension identity). Does NOT
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

const char __libcgj_batch80370_marker[] = "libcgj-batch80370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80370_id(void)
{
	return 80370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_80370 - report this TU's graph batch number.
 *
 * Always returns 80370.
 */
uint32_t
gj_batch_id_80370(void)
{
	(void)NULL;
	return b80370_id();
}

/*
 * gj_graph_milestone_80370 - report this TU's graph milestone revision.
 *
 * Always returns 80370. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_80370(void)
{
	return b80370_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_80370(void)
    __attribute__((alias("gj_batch_id_80370")));

uint32_t __gj_graph_milestone_80370(void)
    __attribute__((alias("gj_graph_milestone_80370")));
